////////////////////////////////////////////////
// TRAY BOT CONFIG
//

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

#define RED_AUTON 0
#define BLUE_AUTON 1
#define PROGRAMMING_SKILLS 2

#define GYRO_PORT 1

#define USER 0
#define TURN 1
#define DRIVE_TIME 2
#define DRIVE_DIST 3
#define DRIVE_WHITE 4

#define TICKS_PER_INCH 57

extern ADIGyro sensor_gyro;
extern ADILineSensor whiteLineR;
extern ADILineSensor whiteLineL;

// Function prototypes
void runDrive(void* params);
void runLift(void* params);
void runGyro(void* params);

void setDirection(double dir);
double getDriveEncoderAvg();


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

extern Motor trayR;
extern Motor trayL;

extern Motor intakeR;
extern Motor intakeL;

// Controller
extern Controller controller;

// Global variables
extern double liftPos;
extern double liftSeek;
extern double trayPos;
extern double traySeek;
extern double runIntake;
extern int deployStep;
extern int depositStep;

extern double direction;

extern int driveMode;
extern double faceDir;
extern double driveSpeed;
extern double targetDrivePos;
extern bool slowTurn;

extern int autonSelect;
extern int numAutons;


// Auton routines

extern double redAuton[];
extern double blueAuton[];
extern double programmingSkills[];


#endif
