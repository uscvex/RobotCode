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




//////////////////////////////////////////
// Position Tracking
//
// Encoder Ports
#define R_ENCODER_PORT 1,2
#define L_ENCODER_PORT 3,4
#define B_ENCODER_PORT 5,6

// Physical Constants
#define INCHES_PER_TICK 0.02836160034
#define TRACKING_DIAMETER 19

// Fudge Factors
#define FORWARD_FUDGE_FACTOR 1
#define STRAFE_FUDGE_FACTOR 1
#define ROTATIONAL_FUDGE_FACTOR 1



//////////////////////////////////////////
// Lift
//
// Lift tuning params
#define LIFT_DEPOSIT_POS 6000
#define LIFT_HOLD_POS 3000
#define LIFT_DOWN_POS 1
#define LIFT_SEEK_RATE 1

// Clamp tuning params
#define CLAMP_RELEASE_POS 1
#define CLAMP_HOLD_POS 2800
#define CLAMP_SEEK_RATE 1




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

// Clamp winch
extern Motor mClamp;

// Lift
extern Motor mLiftR;
extern Motor mLiftL;

// Inner intake
extern Motor mInIntakeR;
extern Motor mInIntakeL;

// Position tracking encoders
extern ADIEncoder rightEncoder;
extern ADIEncoder leftEncoder;
extern ADIEncoder backEncoder;

// Vars for auton control of mechanisms
extern double liftSeek;
extern double clampSeek;

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
