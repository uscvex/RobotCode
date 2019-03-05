#include "main.h"

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
	
    // Sets our port as 'generic device'
    c::registry_bind_port(1, c::v5_device_e(129));  // 129 = generic device
    
    // Set BAUD rate to 115200
    int rate = 115200;
    c::serctl(DEVCTL_SET_BAUDRATE, &rate);
    
    
    
	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);
		
		pros::delay(20);
	}
}
