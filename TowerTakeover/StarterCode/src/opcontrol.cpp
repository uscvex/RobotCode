// Starter Code VEX Zeus Bot


#include "main.h"


using namespace pros;

// Defines for different gearings for motors
#define TURBO E_MOTOR_GEARSET_06
#define SPEED E_MOTOR_GEARSET_18
#define TORQUE E_MOTOR_GEARSET_36
#define DEGREES E_MOTOR_ENCODER_DEGREES

Controller controller(E_CONTROLLER_MASTER);     // Controller object

///////////////////////////////////////////////////////////////////////////////
// Motors
// Motor name(port, gearing, reversed?, encoder_units);

// Drive Motors
Motor drive_left_1(1, SPEED, 0, DEGREES);
Motor drive_left_2(2, SPEED, 1, DEGREES);
Motor drive_left_3(3, SPEED, 0, DEGREES);
Motor drive_right_1(4, SPEED, 1, DEGREES);
Motor drive_right_2(5, SPEED, 0, DEGREES);
Motor drive_right_3(6, SPEED, 1, DEGREES);
// Flywheel Motors
Motor flywheel_1(19, TURBO, 1, DEGREES);
Motor flywheel_2(10, TURBO ,0, DEGREES);
// Intake
Motor intake_in(8, SPEED, 1, DEGREES);
Motor intake_out(9, SPEED, 1, DEGREES);
// Arm Motors
Motor arm_1(7, TORQUE, 0, DEGREES);
Motor arm_2(18, TORQUE, 1, DEGREES);
// Flipper
Motor wrist(17, SPEED, 1, DEGREES);
Motor flip(20, SPEED, 0, DEGREES);
// Skills Scraper
Motor scraper(13,TORQUE,0,DEGREES);

// Gyro Sensor
ADIGyro sensor_gyro(1, 1);  // A
// Other sensors
ADIDigitalIn upper_IR (2);  // B
ADIDigitalIn lower_IR (3);  // C
ADIDigitalIn left_IR (4);   // D
ADIDigitalIn right_IR (5);  // E
ADIDigitalOut ballLight(6); // F
ADIUltrasonic sonar (7,8);  // G,H
// Vision sensor
Vision camera(16);


/////////////////////////////
// Drive task
//
void run_drive(void* params) {
    
    double leftSpeed = 0;
    double rightSpeed = 0;
    
    while (true) {
        
        //
        // Add code here to control drive
        //
        
        // Send powers to drive motors
        drive_left_1.move_voltage(leftSpeed * 12000 / 127);
        drive_left_2.move_voltage(leftSpeed * 12000 / 127);
        drive_left_3.move_voltage(leftSpeed * 12000 / 127);
        drive_right_1.move_voltage(rightSpeed * 12000 / 127);
        drive_right_2.move_voltage(rightSpeed * 12000 / 127);
        drive_right_3.move_voltage(rightSpeed * 12000 / 127);
        
        pros::delay(10);   // Don't hog cpu
    }
}


//////////////////////////////
// Flywheel Task
//
void run_flywheel(void* params) {
    
    double flywheelSpeed = 0;
    double intakeSpeedInner = 0;
    double intakeSpeedOuter = 0;
    double scraperSpeed = 0;
    
    while (true) {
        
        //
        // Add code here to control flywheel & intake
        //
        
        // Set motors on flywheel
        flywheel_1.move_voltage(flywheelSpeed * 12000 / 127);
        flywheel_2.move_voltage(flywheelSpeed * 12000 / 127);
        
        // Send speeds to intake motors
        intake_in.move_voltage(intakeSpeedInner*12000 / 127);
        intake_out.move_voltage(intakeSpeedOuter*12000 / 127);
        
        scraper.move_voltage(scraperSpeed*12000/127);
        
        
        pros::delay(20);   // don't hog cpu
    }
}



///////////////////////////////
// Arm Task
//
void run_arm(void* params) {
    
    double armSpeed = 0;
    double wristSpeed = 0;
    double flipperSpeed = 0;
    
    while (true) {
        
        //
        // Add code here to control arm, wrist, and flipper
        //
        
        // Send values to motors
        flip.move_voltage(flipperSpeed * 12000 / 127);
        wrist.move_voltage(wristSpeed * 12000 / 127);
        arm_1.move_voltage(armSpeed * 12000 / 127);
        arm_2.move_voltage(armSpeed * 12000 / 127);
        
        pros::delay(20);   // don't hog cpu
    }
}



// This function called at the start of auton
void run_auton() {
    
    
    while (true) {
        
        pros::delay(20);   // let other tasks use cpu
    }
}


// Initialize function - called when robot first turns on
// Start drive, flywheel, and arm tasks here
bool hasInit = false;
void initAll() {
    
    if (!hasInit) {
        
        //
        // Add code here to start tasks, reset encoders, etc.
        //
        
    }
    hasInit = true;
    
}


// Probably don't need much code here
void opcontrol() {
    
    while (true) {
       
        pros::delay(20);
    }
}
