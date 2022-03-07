#include "main.h"
#include <string>

using namespace pros;
using namespace std;

void run_display(void* params) {
    int display_count = 0;
    screen::erase();
    controller.clear();
    while (true) {

        display_count++;

        // Main screen
        screen::erase();
        // Print position info to screen
        // screen::print(TEXT_MEDIUM, 1, "(%d, %d, %d)", left_encoder.get_value(), middle_encoder.get_value(), right_encoder.get_value());
        // screen::print(TEXT_MEDIUM, 2, "(%.2f, %.2f, %.2f)", robot_x, robot_y, robot_theta);

        // screen::print(TEXT_MEDIUM, 3, "Belly Grab: %.2f %.2f", belly_grab_pos, belly_grab_target);
        // screen::print(TEXT_MEDIUM, 4, "Base Lift: %.2f %.2f", base_lift_pos, base_lift_target);
        // screen::print(TEXT_MEDIUM, 5, "Base Release: %.2f %.2f", base_release_pos, base_release_target);

        // screen::print(TEXT_MEDIUM, 6, "Spike Wrist: %.2f %.2f", spike_wrist_pos, spike_wrist_target);
        // screen::print(TEXT_MEDIUM, 7, "Spike Arm: %.2f %.2f", spike_arm_pos, spike_arm_target);

        // screen::print(TEXT_MEDIUM, 8, "Lift: %.2f %.2f", lift_pos, lift_target);

        // screen::print(TEXT_MEDIUM, 9, "Yeet: %d", yeet_state);
        // screen::print(TEXT_MEDIUM, 1, "Brightness: %f", brightness);
        // screen::print(TEXT_MEDIUM, 2, "Hue: %f", hue);
        
        string output = "";
        if (!close){
            output = "Far Away";
        }
        else if (yellow) output = "Yellow";
        else output = "Sticker";
        screen::print(TEXT_MEDIUM, 1, "State: %s", output);
        screen::print(TEXT_MEDIUM, 2, "Red value: %f \n", RGB_values.red);
        screen::print(TEXT_MEDIUM, 3, "green value: %f \n", RGB_values.green);
        screen::print(TEXT_MEDIUM, 4, "blue value: %f \n", RGB_values.blue);
        screen::print(TEXT_MEDIUM, 5, "brightness: %f \n", RGB_values.brightness);
        screen::print(TEXT_MEDIUM, 6, "Proximity: %d", proximity);

        // if (!(display_count % 45)) {
        //     cout << "==============================================" << endl;
        //     // cout << "Encoders\t" << left_encoder.get_value() << mtiddle_encoder.get_value() << right_encoder.get_value() << endl;
        //     cout << "Position\t" << robot_x << "\t" << robot_y << "\t" << robot_theta << endl;
        //     cout << "Belly Grab:\t" << belly_grab_pos << " Seek:\t" << belly_grab_target << endl;
        //     cout << "Base Lift:\t" << base_lift_pos << " Seek:\t" << base_lift_target << endl;
        //     cout << "Base Release:\t" << base_release_pos << " Seek:\t" << base_release_target << endl;
        //     cout << "Spike Wrist:\t" << spike_wrist_pos << " Seek:\t" << spike_wrist_target << endl;
        //     cout << "Spike Arm:\t" << spike_arm_pos << " Seek:\t" << spike_arm_target << endl;
        //     cout << "Lift:\t" << lift_pos << " Seek:\t" << lift_target << endl;
        //     cout << "Yeet State:\t" << yeet_state << endl;
        // }

        // Controller display
        if (display_count % 3 == 0)
            controller.print(2, 0, "%.2f, %.2f, %.3f                     ", robot_x, robot_y, robot_theta);
        if (display_count % 3 == 1)
            controller.print(0, 0, "%s %s: %s           ", this_robot.name, robot_names[which_robot], auton_names[which_auton]);
        if (display_count % 3 == 2) {
            string grab_mode = "Not Ready";
            if (base_lift_state == 1)
                grab_mode = "Ready";
            if (base_lift_state == 2)
                grab_mode = "Holding";
            if (base_lift_state == 3)
                grab_mode = "Released";
            controller.print(1, 0, "Grab %s         ", grab_mode);
        }

        delay(60);
    }

}
