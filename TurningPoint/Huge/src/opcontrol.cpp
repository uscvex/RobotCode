#include "main.h"

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
using namespace pros;

void opcontrol() {
	pros::Controller master(pros::E_CONTROLLER_MASTER);

    
    Motor drive_left_1(20);
    Motor drive_left_2(7);
    Motor drive_left_3(9);
    Motor drive_left_4(17);
    Motor drive_right_1(5);
    Motor drive_right_2(4);
    Motor drive_right_3(1);
    Motor drive_right_4(3);
    
	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);
		int left = -master.get_analog(ANALOG_LEFT_Y);
		int right = -master.get_analog(ANALOG_RIGHT_Y);

        drive_right_1.move_voltage(12000*right/127);
        drive_right_2.move_voltage(-12000*right/127);
        drive_right_3.move_voltage(12000*right/127);
        drive_right_4.move_voltage(12000*right/127);
        
        drive_left_1.move_voltage(12000*left/127);
        drive_left_2.move_voltage(12000*left/127);
        drive_left_3.move_voltage(-12000*left/127);
        drive_left_4.move_voltage(-12000*left/127);
		delay(20);
	}
}
