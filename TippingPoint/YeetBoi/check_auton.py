from pprint import pprint

SWITCH_START = "switch ((int) process_entry()) {"


class Command:
    def __init__(self, source):
        self.name = source[1:source.find(":")]
        self.description = ""
        self.warnings = []
        self.args = []
        self.arg_descs =[]
        self.has_time_out = False
        self.blocking = True
        has_cout = False
        if (source.split("\n")[1].strip()[0:2] != "//"):
            self.warnings.append("Command is missing description")
        else:
            self.description = source.split("\n")[1].strip()[2:].strip()
        for line in source.split("\n")[1:]:
            # Make sure command is printed
            if ("cout<<\"" + self.name + "\"<<endl;" in line.replace(" ", "")):
                has_cout = True

            # Read each argument
            if ("process_entry()" in line):
                pre_equals = line[:line.find("=")].strip()
                post_equals = line[line.find("=") + 1:].strip().replace("process_entry()", "ARG")

                self.args.append(pre_equals)
                has_desc = line.find("//") != -1
                if (pre_equals == "command_time_out"):
                    self.arg_descs.append("Timout for command (seconds)")
                else:
                    if has_desc:
                        self.arg_descs.append(line[line.find("//") + 2:].strip())
                    else:
                        self.arg_descs.append(post_equals)
                        self.warnings.append("Argument " + pre_equals + " is missing a description")

            # Check if there is a timeout
            if ("command_time_out" in line):
                self.has_time_out = True

            # Check if command is blocking
            if ("next_command=true" in line.replace(" ", "")):
                self.blocking = False

        # Check that command is printed
        if (not has_cout):
            self.warnings.append("Command needs to print name: " + "cout << \"" + self.name + "\" << endl;")
    
    def check(self, args):
        if len(args) > len(self.args):
            return "too MANY args (" + str(len(args)) + " != " + str(len(self.args)) + ")"
        if len(args) < len(self.args):
            return "too FEW args (" + str(len(args)) + " != " + str(len(self.args)) + ")"
        return "OK"

    def __str__(self):
        result = "\n"
        result += self.name + ":" 
        result += "\n\t" + ("BLOCKING" if self.blocking else "non-blocking")
        result += ", " + ("has timeout" if self.has_time_out else "NO TIMEOUT")
        result += "\n\t" + self.description
        result += "\nUsage:\n\t" + self.name + ", " + (str(self.args)[1:-1].replace("'", "") + "," if (len(self.args) > 0) else "")
        if (len(self.args) > 0):
            result += "\nArguments: (" + str(len(self.args)) + ")"
            for arg, arg_desc in zip(self.args, self.arg_descs):
                result += "\n\t" + arg + ":\t"
                result += arg_desc
        if (len(self.warnings) > 0):
            result += "\nWarnings: (" + str(len(self.warnings)) + ")"
            for warning in self.warnings:
                result += "\n\t" + warning
        return result

    def to_md(self):
        result = ""
        result += "## " + self.name + ":" 
        result += "  \n" + self.description
        result += "  \n*" + ("BLOCKING" if self.blocking else "non-blocking")
        result += ", " + ("has timeout" if self.has_time_out else "NO TIMEOUT") + "*"
        result += "  \n### Usage:\n  > " + self.name + ", " + (str(self.args)[1:-1].replace("'", "") + "," if (len(self.args) > 0) else "")
        if (len(self.args) > 0):
            result += "  \n### Arguments: (" + str(len(self.args)) + ")"
            for arg, arg_desc in zip(self.args, self.arg_descs):
                result += "\n  * **" + arg + "**:\t"
                result += arg_desc
        if (len(self.warnings) > 0):
            result += "  \n### Warnings: (" + str(len(self.warnings)) + ")"
            for warning in self.warnings:
                result += "  \n* " + warning
        result += "  \n  \n"
        return result


class AutonChecker:
    def __init__(self, path_to_file="src/auton.cpp"):
        self.warnings = []
        self.path_to_file = path_to_file
        self.auton_file = open(self.path_to_file).read()
        self.commands = {}
        self.read_commands()
        self.routines = {}
        self.read_routines()

    def read_routines(self):
        number_routines = self.auton_file[self.auton_file.find("int num_autons = ") + len("int num_autons = "):].strip()
        number_routines = int(number_routines[:number_routines.find(";")].strip())
        routine_names = self.auton_file[self.auton_file.find("string auton_names[] = {") + len("string auton_names[] = {"):].strip()
        routine_names = routine_names[:routine_names.find("}")].strip().replace("\"", "").replace(" ", "").split(",")
        routine_pointers = self.auton_file[self.auton_file.find("double* auton_ptr[] = {") + len("double* auton_ptr[] = {"):].strip()
        routine_pointers = routine_pointers[:routine_pointers.find("}")].strip().replace("&", "").replace("[0]", "").replace(" ", "").split(",")
        if (len(routine_names) > number_routines):
            self.warnings.append("Unaccessable routines: increase num_autons")
        if (len(routine_names) < number_routines):
            self.warnings.append("Non-existant routines: decrease num_autons")
        if (len(routine_names) != len(routine_pointers)):
            self.warnings.append("Number of auton names does not match number of auton pointers")
        
        for name, pointer in zip(routine_names, routine_pointers):
            this_routine = {
                "name": name,
                "pointer": pointer,
                "warnings": [],
                "num_commands": 0,
            }
            this_list = number_routines = self.auton_file[self.auton_file.find("double " + pointer + "[] = {") + len("double " + pointer + "[] = {"):].strip()
            this_list = this_list[:this_list.find("}")].strip().split("\n")
            new_list = []
            for item in this_list:
                if item.strip() == "":
                    continue
                new_list.append(item.strip().split(",")[:-1])
            
            start_idx = 1
            # Check that first line is starting position
            if (len(new_list[0]) != 3):
                this_routine["warnings"].append("First line should be starting coords (x, y, theta)")
                start_idx = 0
            for num in new_list[0]:
                try: 
                    num = float(num)
                except:
                    start_idx = 0
                    this_routine["warnings"].append("First line should be starting coords (x, y, theta), \"" + num + "\" isn't valid")
            
            seen_end = False
            for line in new_list[start_idx:]:

                # Ignore blank lines
                if line == []:
                    continue

                # Ignore comments
                if line[0].strip()[0:2] == "//":
                    continue
                for x in range(len(line)):
                    if line[x].strip()[0:2] == "//":
                        line = line[0:x]
                        break

                this_routine["num_commands"] += 1
                
                # Check for premature END
                if line[0] == "END":
                    seen_end = True
                else:
                    if seen_end:
                        if "Commands continue after END" not in this_routine["warnings"]:
                            this_routine["warnings"].append("Commands continue after END")
                
                # Check if command is real
                if line[0].strip() not in self.commands.keys():
                    this_routine["warnings"].append("Command \"" + line[0].strip() + "\" not found")
                    continue

                # Check for correct number of args
                check_command = self.commands[line[0].strip()].check(line[1:])
                if check_command != "OK":
                    this_routine["warnings"].append(str(line).replace("'", "").replace("[", "").replace("]", "") + ": " + check_command)
            self.routines[this_routine["pointer"]] = this_routine

    def read_commands(self):
        start_of_switch = self.auton_file.find(SWITCH_START) + len(SWITCH_START)
        end_of_switch = start_of_switch
        number_brackets = 1
        while (number_brackets > 0):
            end_of_switch += 1
            this_char = self.auton_file[end_of_switch]
            if (this_char == "{"):
                number_brackets += 1
            if (this_char == "}"):
                number_brackets -= 1
        cases = self.auton_file[start_of_switch:end_of_switch].split("case")[1:]
        for case in cases:
            this_command = Command(case)
            self.commands[this_command.name] = this_command
            
    def write_md(self):
        md_file = open("commands.md", "w")
        md_file.write("# Auton Commands  \n  \n*" + str(len(self.commands)) + " Commands auto-generated from \"" + self.path_to_file + "\"*  \n\n---\n\n")
        for command in self.commands.values():
            md_file.write(command.to_md())
        md_file.close

    def output_results(self):
        print("Checked autons:")
        for key, routine in self.routines.items():
            print(routine["name"] + "......" + ("ERROR" if len(routine["warnings"]) != 0 else "OK"))
            # print("\tNum Commands: " + str(routine["num_commands"]))
            if len(routine["warnings"]) != 0:
                print("\t" + str(len(routine["warnings"])) + " Warnings:")
                for warn in routine["warnings"]:
                    print("\t* " + warn)

if __name__ == "__main__":
    auton_checker = AutonChecker("src/auton.cpp")
    auton_checker.write_md()
    auton_checker.output_results()
