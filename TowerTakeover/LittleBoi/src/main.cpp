#include "main.h"

using namespace pros;

// Initialise controller object
Controller controller(E_CONTROLLER_MASTER);


// Init function runs once when robot starts up
void initialize() {
    // Start the screen
	pros::lcd::initialize();
	pros::lcd::print(1, "Speedy Boi do be calibrating");
    
    // Calibrate motor positions (and gyro)
    liftR.move_voltage(-2000);
    liftL.move_voltage(-2000);
    claw.move_voltage(-2000);
    delay(2000);
    liftR.tare_position();
    liftL.tare_position();
    claw.tare_position();

    // Start the drive and lift task
    pros::Task drive_task (runDrive, (void*)"PROS", TASK_PRIORITY_DEFAULT,
                        TASK_STACK_DEPTH_DEFAULT, "Drive task");
    pros::Task lift_task (runLift, (void*)"PROS", TASK_PRIORITY_DEFAULT,
                        TASK_STACK_DEPTH_DEFAULT, "Lift task");
}

// Runs when robot is in disabled state
void disabled() {}

// Runs just before autonomous when robot connected to field switch
void competition_initialize() {}

// Runs when robot in opcontrol state
void opcontrol() {
	while (true) {
		pros::delay(20);
	}
}
