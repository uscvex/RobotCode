#include "main.h"
using namespace pros;
#include <cstdlib>


// Defines for different gearings for motors
#define TURBO E_MOTOR_GEARSET_06
#define SPEED E_MOTOR_GEARSET_18
#define TORQUE E_MOTOR_GEARSET_36
#define DEGREES E_MOTOR_ENCODER_DEGREES
#define CLAW_OPEN_POS 1
#define CLAW_CLOSE_POS 600
#define CLAW_SEEK_RATE 0.5
//seek rates
#define LIFT_SEEK_RATE 1
#define LOW_TOWER_POS 1800
#define MID_TOWER_POS 2250
#define HIGH_TOWER_POS 3600
#define LIFT_CUBE_PUSH_POS 650


#define LIFT_DOWN_POS 1

#define LIFT_SEEK_RATE 1

#define LIFT_DOWN_INCREMENT 50


#define CLAW_OPEN_POS 1
#define CLAW_CLOSE_POS 600
#define CLAW_SEEK_RATE 0.5

#define BUTTON_HOLD_TIME 250
/*
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		lcd::set_text(2, "I was pressed!");
	} else {
		lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	lcd::initialize();
	lcd::set_text(1, "Hello PROS User!");

	lcd::register_btn1_cb(on_center_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

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
	// Initialise controller object
Controller controller(E_CONTROLLER_MASTER);
	// if you want to change the direction the wheel is going
	// then you add the bool parameter. true is forwards, false
	// is backward. if you want to go forward straight you set
	// one to true and one to false. if you want to turn, you
	// set them both to true or both to false. both positive
	// would be turning right, negative left
	// Drive Motors
Motor driveR1(1,SPEED,0);
Motor driveR2(2,SPEED,1);
Motor driveR3(3,SPEED,0);
Motor driveR4(4,SPEED,1);

Motor driveL1(10,SPEED,1);
Motor driveL2(9,SPEED,0);
Motor driveL3(8,SPEED,1);
Motor driveL4(7,SPEED,0);

Motor liftR(5,TORQUE,1);
Motor liftL(11,TORQUE,0);

Motor claw(16,SPEED,1);

double liftPos = 0;
double clawPos = 0;
double clawSeek = 0;
double liftSeek = 0;
// Defining the buttons

#define BTN_LOWER_HEIGHT DIGITAL_R1                     // Button assignment for auto flip
#define BTN_HIGHER_HEIGHT DIGITAL_R2                           // Button assignment for abort
	int moveArm = -1;
        
    // Toggle bools
    bool justClawed = false;
    bool justLiftedUp = false;
    bool justLiftedDown = false;
    
    // Var to measure how long button held
    double lastReleasedTime = millis();
	while (true) {
		// Calculate positions
        liftPos = (liftR.get_position() + liftL.get_position()) / 2;
        clawPos = claw.get_position();
        // User input
        double forwardSpeed = controller.get_analog(ANALOG_LEFT_Y);
        double turnSpeed = controller.get_analog(ANALOG_RIGHT_X);

		liftPos = (liftR.get_position() + liftL.get_position()) / 2;

        // Drive Motors
        driveR1.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveR2.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveR3.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveR4.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveL1.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
        driveL2.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
        driveL3.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
        driveL4.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);

       
        
        // Start speeds at 0 for safety
        double liftSpeed = 0;
        double clawSpeed = 0;
        
        // If we are asking to seek to position, perform the seek
        if (clawSeek >= 0) {
            clawSpeed = (clawSeek - clawPos) / CLAW_SEEK_RATE;
        }
        if (liftSeek >= 0) {
            liftSpeed = (liftSeek - liftPos) / LIFT_SEEK_RATE;
        }
        

        
        // Semi auto controls
        
        // Toggle claw open/close
        if (controller.get_digital(DIGITAL_L1)) {
            if (!justClawed) {
                if (clawSeek != CLAW_OPEN_POS)
                    clawSeek = CLAW_OPEN_POS;
                else
                    clawSeek = CLAW_CLOSE_POS;
            }
            justClawed = true;
        }
        else {
            justClawed = false;
        }
        
        if (controller.get_digital(DIGITAL_L2))
            clawSeek = CLAW_OPEN_POS;
        
        // Move lift up
        if (controller.get_digital(DIGITAL_R1)) {
            // If this is a new press
            if (!justLiftedUp) {
                // Then move lift up to next position
                if (liftSeek < LOW_TOWER_POS && liftPos < LOW_TOWER_POS)
                    liftSeek = LOW_TOWER_POS;
                else if (liftSeek < MID_TOWER_POS && liftPos < MID_TOWER_POS)
                    liftSeek = MID_TOWER_POS;
                else if (liftSeek < HIGH_TOWER_POS)
                    liftSeek = HIGH_TOWER_POS;
            }
            // Don't repeat until released & re-pressed
            justLiftedUp = true;
        }
        else {
            justLiftedUp = false;
        }
        
        // Move lift down
        if (controller.get_digital(DIGITAL_R2)) {
            // If we've been holding the button for time then move all the way down
            if (millis() - lastReleasedTime > BUTTON_HOLD_TIME) {
                liftSeek = -1;
                if (liftPos > LOW_TOWER_POS/2)
                    liftSpeed = -127;
            }
            // Otherwise move down by a set amount
            else if (!justLiftedDown) {
                if (liftSeek > LOW_TOWER_POS - 500) {
                    liftSeek = liftSeek - LIFT_DOWN_INCREMENT;
                    if (liftSeek < 0)
                        liftSeek = -1;
                }
                else {
                    if (liftSeek != LIFT_CUBE_PUSH_POS) {
                        liftSeek = LIFT_CUBE_PUSH_POS;
                    }
                    else {
                        liftSeek = 1;
                    }
                }
            }
            // Don't let us repeat until we've let go
            justLiftedDown = true;
        }
        else {
            justLiftedDown = false;
            lastReleasedTime = millis();
        }
        
        // Button to lift to claw push height
        if (controller.get_digital(DIGITAL_DOWN)) {
            liftSeek = LIFT_CUBE_PUSH_POS;
        }
        
        // Manual overrides
        // Lift up
        if (controller.get_digital(DIGITAL_A)) {
            liftSpeed = 127;
            liftSeek = -1;
        }
        // Lift down
        if (controller.get_digital(DIGITAL_Y)) {
            liftSpeed = -127;
            liftSeek = -1;
        }
        // Claw close
        if (controller.get_digital(DIGITAL_LEFT)) {
            clawSpeed = 127;
            clawSeek = -1;
        }
        // Claw open
        if (controller.get_digital(DIGITAL_RIGHT)) {
            clawSpeed = -127;
            clawSeek = -1;
        }
        
        // Abort!
        if (controller.get_digital(DIGITAL_UP)) {
            clawSeek = -1;
            liftSeek = -1;
        }

		// Finally set motor voltages based on speed
        liftR.move_voltage(12000 * liftSpeed / 127);
        liftL.move_voltage(12000 * liftSpeed / 127);
        
        claw.move_voltage(12000 * clawSpeed / 127);
		delay(20);
	}
}
