#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

////////////////////////////////////////////////////////////////
// Anything that we will use in multiple files
// must be declared as extern in this file.
// Must only be initialised w/o extern in one file.
// Then all files can view and use it
//


using namespace pros;

#define SPEED E_MOTOR_GEARSET_18
#define TORQUE E_MOTOR_GEARSET_36

#define LIFT_ABS_TOP_POS 3700


// Function prototypes
void runDrive(void* params);
void runLift(void* params);

// Motor declarations
extern Motor driveR1;
extern Motor driveR2;
extern Motor driveR3;
extern Motor driveR4;

extern Motor driveL1;
extern Motor driveL2;
extern Motor driveL3;
extern Motor driveL4;

extern Motor liftR;
extern Motor liftL;

extern Motor claw;

// Controller
extern Controller controller;

// Global variables
extern double liftPos;
extern double clawPos;
extern double clawSeek;
extern double liftSeek;


#endif
