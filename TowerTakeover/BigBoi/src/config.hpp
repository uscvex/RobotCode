#ifndef _CONFIG_H_
#define _CONFIG_H_ 1

////////////////////////////////////////////////////////////////////////////////////////
// CONFIG FILE FOR ALL GLOBAL VARS, MOTORS, SENSORS, ETC.
// Please put all in here with extern prefix, then also in file they are used in
// All #defines should be in this file too
//


//////////////////////////////////////////
// Controls
//
// Joystick dead zone
#define JOYZONE 10


//////////////////////////////////////////
// Speed Motor Gearsets
//
#define SPEED E_MOTOR_GEARSET_18
#define TORQUE E_MOTOR_GEARSET_36



// Function prototypes
void runDrive(void* params);
void runLift(void* params);
void trackPosition();
void init();

// Declare motors and controller objects
extern Controller controller;

// Drive
extern Motor mFRA;
extern Motor mFRB;
extern Motor mBRA;
extern Motor mBRB;
extern Motor mFLA;
extern Motor mFLB;
extern Motor mBLA;
extern Motor mBLB;

// Lift
extern Motor mLiftR;
extern Motor mLiftL;

// Inner intake
extern Motor mInIntakeR;
extern Motor mInIntakeL;

// Outer intake
extern Motor mOutIntakeR;
extern Motor mOutIntakeL;

// Intake arms
extern Motor mIntakeArmR;
extern Motor mIntakeArmL;

// Position tracking encoders
extern ADIEncoder rightEncoder;
extern ADIEncoder leftEncoder;
extern ADIEncoder backEncoder;

// Vars for auton control of mechanisms
extern double liftSeek;
extern double intakeArmSeekLeft;
extern double intakeArmSeekRight;
extern double runIntake;

// Have we initialized yet?
extern bool haveInitialized;

// Vars to save robot global position
extern double globalDirection;
extern double globalX;
extern double globalY;


// Vars to save previous encoder values
extern double lastRightEncoder;
extern double lastLeftEncoder;
extern double lastBackEncoder;



#endif
