#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"
#include "../Common/easing.h"
#include "../Common/linefollower.h"
#include "smartmotor.h"
#include "vexspi.h"

// Motor mappings

//Flipper
#define M_FLIP   kVexMotor_2

//Roller
#define M_ROLLER kVexMotor_9

//Chain lift
#define M_CHAIN_LIFT_L kVexMotor_3
#define M_CHAIN_LIFT_R kVexMotor_8

//Drive
#define M_DRIVE_RIGHT_1  kVexMotor_1
#define M_DRIVE_RIGHT_2  kVexMotor_5
#define M_DRIVE_LEFT_1   kVexMotor_10
#define M_DRIVE_LEFT_2   kVexMotor_6

//Mobile goal
#define M_MOBILE_GOAL_R   kVexMotor_4
#define M_MOBILE_GOAL_L   kVexMotor_7

// Sensor mappings
#define P_DRIVE_ENC_RIGHT_A kVexDigital_1
#define P_DRIVE_ENC_RIGHT_B kVexDigital_2
#define P_DRIVE_ENC_LEFT_A  kVexDigital_3
#define P_DRIVE_ENC_LEFT_B  kVexDigital_4
#define P_LIFT_ENC_RIGHT_A  kVexDigital_5
#define P_LIFT_ENC_RIGHT_B  kVexDigital_6
#define P_LIFT_ENC_LEFT_A   kVexDigital_7
#define P_LIFT_ENC_LEFT_B   kVexDigital_8
#define P_ENC_TOP_ROLLER_A  kVexDigital_9
#define P_ENC_TOP_ROLLER_B  kVexDigital_10

#define S_DRIVE_ENC_LEFT  	kVexSensorDigital_1
#define S_DRIVE_ENC_RIGHT 	kVexSensorDigital_2
#define S_LIFT_ENC_RIGHT    kVexSensorDigital_3
#define S_LIFT_ENC_LEFT     kVexSensorDigital_4
#define S_ROLLER_ENC        kVexSensorDigital_5

#define FLIP_POT            kVexAnalog_1 //Starts at value 5.
#define R_MOBILE_POT        kVexAnalog_2 //Starts at value 700
#define L_MOBILE_POT        kVexAnalog_3 //Starts at value 3200
#define GYRO                kVexAnalog_4
#define POWER_EXPANDER      kVexAnalog_5

#define FLIP_HOLD_ANGLE      500
#define CHAIN_TICKS_PER_CONE 80
#define FLIPPER_SEEK_RATE    7
#define FLIPPER_CLOSE_ENOUGH 200
#define FLIPPER_STACK_POS    3500
#define LIFT_CLOSE_ENOUGH    30
#define LIFT_SEEK_RATE       1.5
#define FLIPPER_SAFE_ZONE    100

// Controller mappings
#define J_DRIVE_RIGHT Ch2
#define J_DRIVE_LEFT  Ch3

#define J_AUTOSTACK        Btn5U
#define J_FLIP_LIFT_HOME   Btn5D
#define J_MOBILE_BASE_DOWN Btn6D
#define J_MOBILE_BASE_UP   Btn6U
#define J_CANCEL_AUTOSTACK Btn7U
#define J_AUTO_PRELOADS    Btn8D //Toggle
#define J_CHAIN_LIFT_UP    Btn8R
#define J_CHAIN_LIFT_DOWN  Btn8L
#define J_FLIP_DOWN        Btn7L
#define J_FLIP_UP          Btn7R
#define J_SPIN_ROLL        Btn8U

// PID Controls
EPidController *leftDrivePid;
EPidController *rightDrivePid;

bool autoStackMode;
bool justChangedMode;
short stackCount = 0;
short stackStep = 1;
int flipperDesiredPos = -1;
int liftDesiredPos = -1;
systime_t currTime;

//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {

    { M_MOBILE_GOAL_L,       kVexMotor393S, kVexMotorReversed,    	kVexSensorQuadEncoder,  kVexQuadEncoder_2 },
    { M_MOBILE_GOAL_R,       kVexMotor393S, kVexMotorReversed,  	  kVexSensorQuadEncoder,  kVexQuadEncoder_2 },

    { M_DRIVE_RIGHT_1,       kVexMotor393S, kVexMotorNormal,  	    kVexSensorQuadEncoder,  kVexQuadEncoder_1 },
    { M_DRIVE_RIGHT_2,       kVexMotor393S, kVexMotorNormal,    	  kVexSensorQuadEncoder,  kVexQuadEncoder_1 },
    { M_DRIVE_LEFT_1,        kVexMotor393S, kVexMotorReversed,    	kVexSensorQuadEncoder,  kVexQuadEncoder_3 },
    { M_DRIVE_LEFT_2,        kVexMotor393S, kVexMotorNormal,  	    kVexSensorQuadEncoder,  kVexQuadEncoder_3 },

    { M_CHAIN_LIFT_R,        kVexMotor393S, kVexMotorNormal,    	  kVexSensorQuadEncoder,  kVexQuadEncoder_5 },
    { M_CHAIN_LIFT_L,        kVexMotor393S, kVexMotorNormal,  	    kVexSensorQuadEncoder,  kVexQuadEncoder_4 },

    { M_ROLLER,              kVexMotor393S, kVexMotorNormal,  	    kVexSensorQuadEncoder,  kVexQuadEncoder_5 },
    { M_FLIP,                kVexMotor393S, kVexMotorNormal,  	    kVexSensorQuadEncoder,  kVexQuadEncoder_5 }
};

static vexDigiCfg dConfig[] = {
    { P_DRIVE_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_1 },
    { P_DRIVE_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_1 },

    { P_DRIVE_ENC_LEFT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_3 },
    { P_DRIVE_ENC_LEFT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },

    { P_LIFT_ENC_LEFT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_4 },
    { P_LIFT_ENC_LEFT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_4 },

    { P_LIFT_ENC_RIGHT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_5 },
    { P_LIFT_ENC_RIGHT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_5 },

    { P_ENC_TOP_ROLLER_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_2 },
    { P_ENC_TOP_ROLLER_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_2 }

 };

//-------------------Setup----------------------------------------------------//
void vexUserSetup()
{
    vexMotorConfigure   ( mConfig,   MOT_CONFIG_SIZE( mConfig ) )  ;
    vexDigitalConfigure ( dConfig, DIG_CONFIG_SIZE( dConfig ) );
}

void vexUserInit()
{
    //bStopTasksBetweenModes = false;

	  SmartMotorsInit();
    SmartMotorSetPowerExpanderStatusPort(POWER_EXPANDER);
    SmartMotorsAddPowerExtender(M_DRIVE_RIGHT_2, M_DRIVE_LEFT_2);
    SmartMotorLinkMotors(M_DRIVE_RIGHT_1, M_DRIVE_RIGHT_2);
    SmartMotorLinkMotors(M_DRIVE_LEFT_1, M_DRIVE_LEFT_2);
    SmartMotorPtcMonitorEnable();
    SmartMotorRun();

	leftDrivePid  =  EPidInit(kMinJerk, 0.01, 0, 0.01, S_DRIVE_ENC_LEFT,  false);
	rightDrivePid =  EPidInit(kMinJerk, 0.01, 0, 0.01, S_DRIVE_ENC_RIGHT,  true);

}

//-------------Miscellaneous functions----------------------------------------//

bool driveMotors(void) {
	short ld, rd ;
	//Calculate Motor Power
	rd = VALLEY(vexControllerGet(J_DRIVE_RIGHT), 20, 127);
	ld =  VALLEY(vexControllerGet(J_DRIVE_LEFT), 20, 127);
  //ld = 0;

	SetMotor(M_DRIVE_LEFT_1,  ld);
	SetMotor(M_DRIVE_LEFT_2,  ld);
	SetMotor(M_DRIVE_RIGHT_1, rd);
	SetMotor(M_DRIVE_RIGHT_2, rd);

	//vex_printf("%f %f\n", rd, ld);
	return (ld != 0 || rd != 0);
}

int getTimeDifference(systime_t startTime) {
  return (int) chTimeNow() - startTime;
}
void autostack() {

  currTime = chTimeNow();
  while(autoStackMode) {

    vex_printf("flipperDesiredPos: %d\n", flipperDesiredPos);
    if(vexControllerGet(J_CANCEL_AUTOSTACK)) {
      if(justChangedMode == false) {
  	    autoStackMode = !autoStackMode;
        justChangedMode = true;
      }
	  } else {
      justChangedMode = false;
    }
    if(vexControllerGet(Btn6D)) {
      stackStep = 4;
      flipperDesiredPos = -1;
    }
    if(vexControllerGet(Btn8D)) {
      stackCount--;
      if(stackCount < 0) stackCount = 0;
    }

    //Step 1: Raise flipper to Hold position
    if(stackStep == 1){
      flipperDesiredPos = FLIP_HOLD_ANGLE;
      stackStep = 2;
    }
    //Step 2: Lift to appropriate height
    if(stackStep == 2){
      if((abs(flipperDesiredPos - vexSensorValueGet(FLIP_POT) < FLIPPER_CLOSE_ENOUGH)
        && getTimeDifference(currTime) > 500) || getTimeDifference(currTime) > 1500) {
          flipperDesiredPos = FLIP_HOLD_ANGLE;
          liftDesiredPos = CHAIN_TICKS_PER_CONE * stackCount;
          currTime = chTimeNow();
          stackStep = 3;
      }
    }
    //Step 3: Engage flipper
    if(stackStep == 3){
      if((abs(liftDesiredPos - vexSensorValueGet(S_LIFT_ENC_LEFT) < LIFT_CLOSE_ENOUGH)
        && getTimeDifference(currTime) > 1000) || getTimeDifference(currTime) > 2000) {
          currTime = chTimeNow();
          flipperDesiredPos = FLIPPER_STACK_POS;
          stackCount++;
          stackStep = -1;
      }
    }
    //Step 4: Return flipper to Hold positions and lower chain
    if(stackStep == 4) {
      if((abs(flipperDesiredPos - vexSensorValueGet(FLIP_POT) < FLIPPER_CLOSE_ENOUGH) || flipperDesiredPos==-1)) {
        liftDesiredPos = 1;
        flipperDesiredPos = FLIP_HOLD_ANGLE;
        stackStep = 5;
      }
    }

    //Step 5: Drop flipper
    if(stackStep == 5){
      if(abs(vexSensorValueGet(S_LIFT_ENC_LEFT)) < FLIPPER_SAFE_ZONE) {
        flipperDesiredPos = -1;
        stackStep = -1;
      }
    }

    wait1Msec(25);
  }
}

//---------------------Autonomous Functions ----------------------------------//

/**
 drive_forward(double ratio);
 param:
 @ratio is the ditance you want to move. One block take a second to move
 */

task slewMotors(void *arg)
{
  vexTaskRegister("slewMotors");
  vex_printf("Starting task slewMotors");
  int flipperPos;
  int liftPos;
  while(1) {
    if(autoStackMode){
      if(flipperDesiredPos > 0) {
        flipperPos = vexSensorValueGet(FLIP_POT);
        SetMotor(M_FLIP, (flipperDesiredPos - flipperPos)/FLIPPER_SEEK_RATE);
      } else {
        SetMotor(M_FLIP, 0);
      }
      if(liftDesiredPos> 0) {
        liftPos = vexSensorValueGet(S_LIFT_ENC_LEFT);
        SetMotor(M_CHAIN_LIFT_L, (liftDesiredPos - liftPos)/LIFT_SEEK_RATE);
        SetMotor(M_CHAIN_LIFT_R, (liftDesiredPos - liftPos)/  LIFT_SEEK_RATE);
      } else {
        SetMotor(M_CHAIN_LIFT_R, 0);
        SetMotor(M_CHAIN_LIFT_L, 0);
      }
    }
    wait1Msec(20);
  }
}


void drive_forward(double ratio){
	systime_t init_time = chTimeNow();
		// int backupLevel = BackupBatteryLevel;
	//vex_printf("%d    %d   ", vexSpiGetMainBattery(), vexSpiGetBackupBattery());

	systime_t duration = abs(ratio*700);
	int32_t target = 1500*ratio;

	EPidEnable(rightDrivePid,duration, target);
	EPidEnable(leftDrivePid,duration, target);
	// systime_t end_time = chTimeNow() + duration;

	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			SetMotor(M_DRIVE_LEFT_1,  0);
			SetMotor(M_DRIVE_RIGHT_1, 0);
			SetMotor(M_DRIVE_LEFT_2,  0);
			SetMotor(M_DRIVE_RIGHT_2, 0);
			break;
		}

		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			int16_t ld = EPidUpdate(leftDrivePid);
			int16_t rd = EPidUpdate(rightDrivePid);
			SetMotor(M_DRIVE_LEFT_1,  ld);
			SetMotor(M_DRIVE_RIGHT_1, rd);
			SetMotor(M_DRIVE_LEFT_2,  ld);
			SetMotor(M_DRIVE_RIGHT_2, rd);
			vexSleep(10);
		}
		else{
			SetMotor(M_DRIVE_LEFT_1,  0);
			SetMotor(M_DRIVE_RIGHT_1, 0);
			SetMotor(M_DRIVE_LEFT_2,  0);
			SetMotor(M_DRIVE_RIGHT_2, 0);
			break;
		}
	}
	SetMotor(M_DRIVE_LEFT_1,  0);
	SetMotor(M_DRIVE_RIGHT_2, 0);
	SetMotor(M_DRIVE_LEFT_1,  0);
	SetMotor(M_DRIVE_RIGHT_2, 0);
	//vex_printf("%d\n", vexSensorValueGet(S_DRIVE_ENC_RIGHT));
}

void turn_deg(double ratio){
	systime_t init_time = chTimeNow();


	systime_t duration = abs(ratio*1700);
	int32_t target = 4200*ratio;
	EPidEnable(rightDrivePid,duration, -target);
	EPidEnable(leftDrivePid,duration, target);
	// systime_t end_time = chTimeNow() + duration;

	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			SetMotor(M_DRIVE_LEFT_1,  0);
			SetMotor(M_DRIVE_RIGHT_1, 0);
			SetMotor(M_DRIVE_LEFT_2,  0);
			SetMotor(M_DRIVE_RIGHT_2, 0);

			break;
		}

		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			int16_t ld = EPidUpdate(leftDrivePid);
			int16_t rd = EPidUpdate(rightDrivePid);
			SetMotor(M_DRIVE_LEFT_1,  ld);
			SetMotor(M_DRIVE_RIGHT_1, rd);
			SetMotor(M_DRIVE_LEFT_2,  ld);
			SetMotor(M_DRIVE_RIGHT_2, rd);
			vexSleep(10);
		}
		else{

			break;
		}
	}
	SetMotor(M_DRIVE_LEFT_1,   0);
	SetMotor(M_DRIVE_RIGHT_1, 0);
	SetMotor(M_DRIVE_LEFT_2,  0);
	SetMotor(M_DRIVE_RIGHT_2, 0);
}


void wait(double ratio){
	systime_t init_time = chTimeNow();
	systime_t duration = abs(ratio*1000);
	SetMotor(M_DRIVE_LEFT_1,   0);
	SetMotor(M_DRIVE_RIGHT_1, 0);
	SetMotor(M_DRIVE_LEFT_2,  0);
	SetMotor(M_DRIVE_RIGHT_2, 0);

	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			break;
		}

		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){

			vexSleep(10);
		}
		else{
			break;
		}
	}
}


//---------------------Autonomous routine-------------------------------------//

msg_t vexAutonomous( void *arg )
{
	(void)arg;
	vexTaskRegister("auton");

	return (msg_t)0;
}

//---------------------User control settings----------------------------------//


msg_t vexOperator( void *arg )
{
	(void)arg;
	vexTaskRegister("operator");

  StartTask(slewMotors);
  autoStackMode = false;

	while(!chThdShouldTerminate())
	{
    //vex_printf("In operator task");

    if(autoStackMode) {
      stackStep = 1;
      autostack();
    }

    else {
	  driveMotors();

    if(vexControllerGet(J_AUTOSTACK)) {
      if(justChangedMode == false) {
        stackStep = 1;
        autoStackMode = !autoStackMode;
        justChangedMode = true;
      }
    } else {
      justChangedMode = false;
    }


	  if (vexControllerGet(J_FLIP_UP)) {
	  	vexMotorSet(M_FLIP, 127);
	  } else if (vexControllerGet(J_FLIP_DOWN)) {
	  	vexMotorSet(M_FLIP, -127);
	  } else {
	  	vexMotorSet(M_FLIP, 0);
	  }

	  if (vexControllerGet(J_MOBILE_BASE_UP)) {
        vexMotorSet(M_MOBILE_GOAL_R, 127);
        vexMotorSet(M_MOBILE_GOAL_L, 127);
      } else if (vexControllerGet(J_MOBILE_BASE_DOWN)) {
        vexMotorSet(M_MOBILE_GOAL_R, -127);
        vexMotorSet(M_MOBILE_GOAL_L, -127);
      } else {
        vexMotorSet(M_MOBILE_GOAL_R, 0);
        vexMotorSet(M_MOBILE_GOAL_L	, 0);
      }

      if (vexControllerGet(J_CHAIN_LIFT_UP)) {
        vexMotorSet(M_CHAIN_LIFT_L, 127);
        vexMotorSet(M_CHAIN_LIFT_R, 127);
	  } else if (vexControllerGet(J_CHAIN_LIFT_DOWN)) {
        vexMotorSet(M_CHAIN_LIFT_L, -127);
        vexMotorSet(M_CHAIN_LIFT_R, -127);
      } else {
        vexMotorSet(M_CHAIN_LIFT_L, 0);
        vexMotorSet(M_CHAIN_LIFT_R, 0);
      }

      if (vexControllerGet(J_SPIN_ROLL)) {
      	vexMotorSet(M_ROLLER, 127);
      } else {
      	vexMotorSet(M_ROLLER, 0);
      }

  		//Don't hog cpu
  		vexSleep(10);
	   }
  }

	return (msg_t)0;

}
