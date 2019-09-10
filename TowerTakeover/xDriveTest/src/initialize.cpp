#include "main.h"

// Declare motors and controller objects
Controller controller(E_CONTROLLER_MASTER);


// Have we initialized yet?
bool haveInitialized = false;

// Initialization function
void init() {
    // Do every-time init here (clear state vars, etc.)
    
    // Do one-time init here (start tasks, clear encoder values, etc.)
    if (!haveInitialized) {
        haveInitialized = true;
        rightEncoder.reset();
        leftEncoder.reset();
        backEncoder.reset();
        
        // Play a song
        mLiftL.move_voltage(-2000);
        mLiftR.move_voltage(-2000);
        delay(1000);
        mLiftL.move_voltage(0);
        mLiftR.move_voltage(0);
        
        mClamp.tare_position();
        mLiftR.tare_position();
        mLiftL.tare_position();
        
        pros::Task driveTask (runDrive);
        pros::Task liftTask (runLift);
    }
}


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
    init();
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
