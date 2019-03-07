#include "main.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

using namespace pros;

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	
    std::fstream dataPort;
    
    
    
    
	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);
        
//        std::fstream myFile;
//        myFile.open("/dev/21");
//        if (myFile.is_open()) {
//            pros::lcd::print(2,"Open");
//        }
//        else {
//            pros::lcd::print(2,"Closed");
//        }
//
//        std::string thisLine;
//        myFile >> thisLine;
//
//        pros::lcd::print(1,"%s",thisLine);
//        pros::lcd::print(3,"%d",myFile.gcount());
//
//
//        myFile.close(); // always close files when you're done with them

        c::registry_bind_port(21,c::v5_device_e_t(129));
        FILE* usd_file_read = fopen("/dev/21", "r");
        
        bool readable = c::fdctl(0,16,0);
        
        c::fdctl(0,10,0);
        
        char buf[256]; // This just needs to be larger than the contents of the file
        
        int char1 = fgetc(usd_file_read);
        
        for (int i = 0; i < 256; i++)
            buf[i] = 'a';
        
        fread(buf, 1, 256, usd_file_read); // passing 1 because a `char` is 1 byte, and 50 b/c it's the length of buf
        printf("%s\n", buf); // print the string read from the file
        lcd::print(1,"%s",buf);
        lcd::print(2,"%d",readable);
        lcd::print(3,"%d",char1);
        // Should print "Example text" to the terminal
        fclose(usd_file_read); // always close files when you're done with them
        
		pros::delay(20);
	}
    std::cout << "Stopped\n";
}
