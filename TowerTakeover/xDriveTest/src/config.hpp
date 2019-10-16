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
// Motor Ports
//
// Drive Motors
#define M_FRA_P 8
#define M_FRB_P 7
#define M_BRA_P 9
#define M_BRB_P 10
#define M_FLA_P 13
#define M_FLB_P 17
#define M_BLA_P 19
#define M_BLB_P 20

// Tower lift
#define M_LIFT_R_P 15
#define M_LIFT_L_P 14

// Intake Wheels
#define M_IN_INTAKE_R_P 11
#define M_IN_INTAKE_L_P 12
#define M_OUT_INTAKE_R_P 21
#define M_OUT_INTAKE_L_P 21

// Intake Arms
#define M_INTAKE_ARM_R_P 21
#define M_INTAKE_ARM_L_P 21


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
#define LIFT_DEPOSIT_POS 6000 * 5
#define LIFT_HOLD_POS 3000 * 5
#define LIFT_DOWN_POS 1
#define LIFT_SEEK_RATE 1

// Clamp tuning params
#define CLAMP_RELEASE_POS 1
#define CLAMP_HOLD_POS 2800
#define CLAMP_SEEK_RATE 1

// Intake tuning params
#define INTAKE_IN_SPEED 127
#define INTAKE_OUT_SPEED 30

// Intake arm params
#define INTAKE_ARM_SEEK_RATE 2.5
#define INTAKE_ARM_IN_POS 1
#define INTAKE_ARM_OUT_POS 60




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
