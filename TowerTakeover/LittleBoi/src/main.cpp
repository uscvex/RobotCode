#include "main.h"

using namespace pros;

// Initialise controller object
Controller controller(E_CONTROLLER_MASTER);

// Print current auton routine selection to controller
void printAuton() {
    if (autonSelect == RED_AUTON)
        controller.print(0,0,"Red Auton       ");
    if (autonSelect == BLUE_AUTON)
        controller.print(0,0,"Blue Auton      ");
    if (autonSelect == PROGRAMMING_SKILLS)
        controller.print(0,0,"Prog. Skills    ");
}

// Init function runs once when robot starts up
void initialize() {
    // Start the screen
	pros::lcd::initialize();
	pros::lcd::print(1, "Speedy Boi do be calibrating");
   
    controller.print(0,0,"Calibrating...5");
    
    sensor_gyro = ADIGyro(1, GYRO_PORT);
    
    delay(1000);
    controller.print(0,14,"4");
    delay(1000);
    controller.print(0,14,"3");
    delay(1000);
    

    // Calibrate motor positions (and gyro)
    liftR.move_voltage(-2000);
    liftL.move_voltage(-2000);
    claw.move_voltage(-2000);
    
    controller.print(0,14,"2");
    delay(1000);
    controller.print(0,14,"1");
    delay(1000);
    
    liftR.tare_position();
    liftL.tare_position();
    claw.tare_position();

    // Start the drive and lift task
    pros::Task drive_task (runDrive, (void*)"PROS", TASK_PRIORITY_DEFAULT,
                        TASK_STACK_DEPTH_DEFAULT, "Drive task");
    pros::Task lift_task (runLift, (void*)"PROS", TASK_PRIORITY_DEFAULT,
                        TASK_STACK_DEPTH_DEFAULT, "Lift task");
    pros::Task gyro_task (runGyro, (void*)"PROS", TASK_PRIORITY_DEFAULT,
                          TASK_STACK_DEPTH_DEFAULT, "Gyro task");
    
    printAuton();
}

// Runs when robot is in disabled state
void disabled() {}

// Runs just before autonomous when robot connected to field switch
void competition_initialize() {}

// Runs when robot in opcontrol state
void opcontrol() {
    bool justSwitchedAuton = false;
    
    // Stop auton drive
    driveMode = USER;
    
	while (true) {
        
        if (controller.get_digital(DIGITAL_UP) && controller.get_digital(DIGITAL_X)) {
            if (!justSwitchedAuton) {
                autonSelect++;
                if (autonSelect >= numAutons) {
                    autonSelect = 0;
                }
                printAuton();
            }
            justSwitchedAuton = true;
        }
        else {
            justSwitchedAuton = false;
        }
        
		pros::delay(20);
	}
}
