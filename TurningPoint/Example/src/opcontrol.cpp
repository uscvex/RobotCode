
#include "main.h"

using namespace pros;                                   // So we don't need pros:: infront of everything

#define BTN_ARM_UP DIGITAL_R1                           // Button assignment for arm up
#define BTN_ARM_DOWN DIGITAL_R2                         // Button assignment for arm down
#define BTN_AUTO_FLIP DIGITAL_L1                        // Button assignment for auto flip
#define BTN_ABORT DIGITAL_UP                            // Button assignment for abort

#define ARM_POS_FLIP 90                                 // Position to move the arm up to when flipping
#define ARM_POS_DOWN 0                                  // Position to move the arm down to when not flipping
#define ARM_CLOSE_ENOUGH 5                              // Desired accuracy for auto-seek
#define SEEKRATE 2                                      // How 'strong' the seek is --> bigger = weaker

double armSeek = -1;                                    // Var for where we want the arm to be
double armPos = 0;                                      // Var for where the arm is
int flipStep = -1;                                      // Var for what step of the auto-flip routine we're on

Controller controller (E_CONTROLLER_MASTER);            // Instantiate the controller

                                                        // Create Motor( port, gearset, reversed?, encoder units)
Motor armMotor(1, E_MOTOR_GEARSET_18, 0, E_MOTOR_ENCODER_DEGREES);


void run_arm(void* param) {                             // This function will run as a task
                                                        // It will control the movement (manual and automatic) of the arm
	
    while (true) {                                      // We want this to run forever
        
        double armPower = 0;                            // Start with 0 power
        
        if (flipStep == 1) {                            // Start of flip routine
            armSeek = ARM_POS_FLIP;                     // Tell arm to go up
            if (abs(armPos - ARM_POS_FLIP)
                < ARM_CLOSE_ENOUGH)                     // Once it's there
                flipStep = 2;                           // Move to next step
        }
        if (flipStep == 2) {                            // 2nd step
            armSeek = ARM_POS_DOWN;                     // Tell arm to go down
            if (abs(armPos - ARM_POS_DOWN)
                < ARM_CLOSE_ENOUGH) {                   // Once it's there
                flipStep = -1;                          // Finish flip routine
                armSeek = -1;                           // Stop the arm from seeking
            }
        }
        
        if (armSeek != -1) {                            // If we have asked for a seek
            armPower = (armSeek - armPos) / SEEKRATE;   // Power is proportional to distance to goal
            if (armPower > 127) armPower = 127;         // Cap max pos speed
            if (armPower < -127) armPower = -127;       // Cap max neg speed
        }
        
        if (controller.get_digital(BTN_AUTO_FLIP)) {    // If the auto flip button is pressed
            flipStep = 1;                               // Start step one of auto flip routine
        }
        if (controller.get_digital(BTN_ARM_UP)) {       // If manual arm up button is pressed
            armPower = 127;                             // Run arm up,
            armSeek = -1;                               // And stop seeking
            flipStep = -1;                              // Stop auto routine
        }
        if (controller.get_digital(BTN_ARM_DOWN)) {     // If manual arm up button is pressed
            armPower = -127;                            // Run arm down,
            armSeek = -1;                               // And stop seeking
            flipStep = -1;                              // Stop auto routine
        }
        if (controller.get_digital(BTN_ABORT)) {        // If abort button is pressed
            armSeek = -1;                               // Stop seeking
            flipStep = -1;                              // Stop auto routine
        }
        
        armMotor.move_voltage(armPower * 12000 / 127);  // Now make the motor move
                                                        // move_voltage() takes a double in the range -12000 mV to 12000 mV, armPower is in range -127 to 127, so we have to convert
        
        delay(20);                                      // This is to give other tasks a chance to run
        
    }
}

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
	
    pros::Task armTask (run_arm);
    
	while (true) {
        
        // Very little to do here, most should be done in individual tasks
        
		delay(20);
	}
}
