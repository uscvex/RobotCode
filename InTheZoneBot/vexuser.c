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

//Sweep
#define M_SWEEP      kVexMotor_2

//Chain lift
#define M_CHAIN_LIFT kVexMotor_9

//Drive
#define M_DRIVE_RIGHT_F  kVexMotor_5
#define M_DRIVE_RIGHT_B  kVexMotor_8
#define M_DRIVE_LEFT_F   kVexMotor_6
#define M_DRIVE_LEFT_B   kVexMotor_3

//Mobile goal
#define M_MOBILE_GOAL_R   kVexMotor_4
#define M_MOBILE_GOAL_L   kVexMotor_7

// Sensor mappings
#define P_DRIVE_ENC_LEFT_A  kVexDigital_1
#define P_DRIVE_ENC_LEFT_B  kVexDigital_2
#define P_DRIVE_ENC_RIGHT_A kVexDigital_3
#define P_DRIVE_ENC_RIGHT_B kVexDigital_4
#define P_LIFT_ENC_A        kVexDigital_6
#define P_LIFT_ENC_B        kVexDigital_7
#define P_UP_LIMIT_SWITCH   kVexDigital_5
#define P_DOWN_LIMIT_SWITCH kVexDigital_10

#define S_DRIVE_ENC_LEFT  	kVexSensorDigital_1
#define S_DRIVE_ENC_RIGHT 	kVexSensorDigital_3
#define S_UP_LIMIT_SWITCH   kVexSensorDigital_5
#define S_CHAIN_LIFT_ENC    kVexSensorDigital_7
#define S_DOWN_LIMIT_SWITCH kVexSensorDigital_8

#define SWEEP_POT           kVexAnalog_1
#define POWER_EXPANDER      kVexAnalog_2

// Controller mappings
#define J_DRIVE Ch3
#define J_TURN  Ch1

#define J_MOBILE_BASE_UP   Btn5U
#define J_MOBILE_BASE_DOWN Btn5D
#define J_CHAIN_LIFT_UP    Btn6U
#define J_CHAIN_LIFT_DOWN  Btn6D
#define J_SWEEP_IN         Btn8L
#define J_SWEEP_OUT        Btn8R
#define J_MODE_TOGGLE      Btn7U
#define J_AUTOSTACK        Btn6U

#define DIRECTION_UP       0
#define DIRECTION_FALLING  1
#define DIRECTION_BOTTOM   2

#define LIFT_START_HEIGHT    250
#define CONE_PICKUP_HEIGHT   100
#define SWEEP_IN_POS         2500
#define SWEEP_OUT_POS        0
#define SWEEP_PRELOAD_POS    1250
#define JIGGLE_CONE_UP       100
#define LIFT_JIGGLE_RANGE    60
#define BASE_DROP_HEIGHT     500
#define CHAIN_TICKS_PER_CONE 80

#define LIFT_CLOSE_ENOUGH    20
#define SWEEP_CLOSE_ENOUGH   30
#define LIFT_SEEK_RATE       3
#define SWEEP_SEEK_RATE      3



// PID Controls
EPidController *leftDrivePid;
EPidController *rightDrivePid;

bool autoStackMode;
bool justChangedMode;
short stackCount = 0;
short stackStep = 1;
int sweepDesiredPos = -1;
int liftDesiredPos = -1;
systime_t currTime;

//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {

    { M_MOBILE_GOAL_L,       kVexMotor393S, kVexMotorReversed,    	kVexSensorQuadEncoder,  kVexQuadEncoder_2 },
    { M_MOBILE_GOAL_R,       kVexMotor393S, kVexMotorReversed,  	  kVexSensorQuadEncoder,  kVexQuadEncoder_2 },

    { M_DRIVE_RIGHT_F,       kVexMotor393S, kVexMotorNormal,   	    kVexSensorQuadEncoder,  kVexQuadEncoder_1 },
    { M_DRIVE_RIGHT_B,       kVexMotor393S, kVexMotorNormal,    	  kVexSensorQuadEncoder,  kVexQuadEncoder_1 },
    { M_DRIVE_LEFT_F,        kVexMotor393S, kVexMotorNormal,    	  kVexSensorQuadEncoder,  kVexQuadEncoder_3 },
    { M_DRIVE_LEFT_B,        kVexMotor393S, kVexMotorNormal,  	    kVexSensorQuadEncoder,  kVexQuadEncoder_3 },

    { M_CHAIN_LIFT,          kVexMotor393S, kVexMotorNormal,    	  kVexSensorQuadEncoder,  kVexQuadEncoder_5 },

    { M_SWEEP,               kVexMotor393S, kVexMotorNormal,  	    kVexSensorQuadEncoder,  kVexQuadEncoder_5 }
};

static vexDigiCfg dConfig[] = {
    { P_DRIVE_ENC_RIGHT_A, kVexSensorQuadEncoder,  kVexConfigQuadEnc1,  kVexQuadEncoder_1 },
    { P_DRIVE_ENC_RIGHT_B, kVexSensorQuadEncoder,  kVexConfigQuadEnc2,  kVexQuadEncoder_1 },

    { P_DRIVE_ENC_LEFT_A,  kVexSensorQuadEncoder,  kVexConfigQuadEnc1,  kVexQuadEncoder_3 },
    { P_DRIVE_ENC_LEFT_B,  kVexSensorQuadEncoder,  kVexConfigQuadEnc2,  kVexQuadEncoder_3 },

    { P_LIFT_ENC_A,        kVexSensorQuadEncoder,  kVexConfigQuadEnc1,  kVexQuadEncoder_5 },
    { P_LIFT_ENC_B,        kVexSensorQuadEncoder,  kVexConfigQuadEnc2,  kVexQuadEncoder_5 },

    {P_UP_LIMIT_SWITCH,    kVexSensorDigitalInput, kVexConfigInput,     0},
    {P_DOWN_LIMIT_SWITCH,  kVexSensorDigitalInput, kVexConfigInput,     0}
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
    SmartMotorsAddPowerExtender(M_DRIVE_RIGHT_F, M_DRIVE_LEFT_F, M_MOBILE_GOAL_R, M_MOBILE_GOAL_L);
    SmartMotorLinkMotors(M_DRIVE_RIGHT_F, M_DRIVE_RIGHT_B);
    SmartMotorLinkMotors(M_DRIVE_LEFT_F, M_DRIVE_LEFT_B);
    SmartMotorPtcMonitorEnable();
    SmartMotorRun();

	leftDrivePid  =  EPidInit(kMinJerk, 0.01, 0, 0.01, S_DRIVE_ENC_LEFT,  false);
	rightDrivePid =  EPidInit(kMinJerk, 0.01, 0, 0.01, S_DRIVE_ENC_RIGHT,  true);

}

//-------------Miscellaneous functions----------------------------------------//

bool driveMotors(void) {
	short ld, rd;
	//Calculate Motor Power
	int forward = VALLEY(vexControllerGet(J_DRIVE), 20, 127);
	float turn  =  (0.6)*VALLEY(vexControllerGet(J_TURN), 20, 127);

  if(vexSensorValueGet(P_UP_LIMIT_SWITCH) == 0) {
    forward *= -1;
    turn *= -1;
  }

  ld = VALLEY(forward + (int)turn, 25, 127);
  rd = VALLEY(forward - (int)turn, 25, 127);

	SetMotor(M_DRIVE_LEFT_F,  ld);
	SetMotor(M_DRIVE_LEFT_B,  ld);
	SetMotor(M_DRIVE_RIGHT_F, rd);
	SetMotor(M_DRIVE_RIGHT_B, rd);

	//vex_printf("%f %f\n", rd, ld);
	return (ld != 0 || rd != 0);
}

int getTimeDifference(systime_t startTime) {
  return (int) chTimeNow() - startTime;
}
void autostack() {

  currTime = chTimeNow();
  while(autoStackMode) {
    if(vexControllerGet(J_MODE_TOGGLE)) {
      if(justChangedMode == false) {
        autoStackMode = !autoStackMode;
        justChangedMode = true;
      }
    } else {
      justChangedMode = false;
    }
  }

  //Drop crane and pick up cone
  if(stackStep == 1) {
    liftDesiredPos = CONE_PICKUP_HEIGHT;
    stackStep = 2;
  }
  //jiggle to hold cone tight
  if(stackStep == 2) {
    if((abs(liftDesiredPos - vexSensorValueGet(S_CHAIN_LIFT_ENC) < LIFT_CLOSE_ENOUGH)
      && getTimeDifference(currTime) > 300) || getTimeDifference(currTime > 800)) {
        liftDesiredPos = JIGGLE_CONE_UP;
        currTime = chTimeNow();
        stackStep = 3;
      }
  }
  if(stackStep == 2) {
    if((abs(liftDesiredPos - vexSensorValueGet(S_CHAIN_LIFT_ENC) < LIFT_JIGGLE_RANGE)
      && getTimeDifference(currTime) > 200) || getTimeDifference(currTime > 400)) {
        liftDesiredPos = CONE_PICKUP_HEIGHT;
        currTime = chTimeNow();
        stackStep = 4;
      }
  }

  //Raise cone to appropriate height
  if(stackStep == 4) {
    if((abs(liftDesiredPos - vexSensorValueGet(S_CHAIN_LIFT_ENC) < LIFT_JIGGLE_RANGE)
      && getTimeDifference(currTime) > 200) || getTimeDifference(currTime > 400)) {
        liftDesiredPos = BASE_DROP_HEIGHT + stackCount*CHAIN_TICKS_PER_CONE;
        currTime = chTimeNow();
        stackStep = 5;
      }
  }

  //Spin around
  if(stackStep == 5) {
    if((abs(liftDesiredPos - vexSensorValueGet(S_CHAIN_LIFT_ENC) < LIFT_CLOSE_ENOUGH)
      && getTimeDifference(currTime) > 500) || getTimeDifference(currTime > 1200)) {
        sweepDesiredPos = SWEEP_IN_POS;
        currTime = chTimeNow();
        stackStep = 6;
      }
  }

  //Drop cone
  if(stackStep == 6) {
    if((abs(sweepDesiredPos - vexSensorValueGet(SWEEP_POT) < SWEEP_CLOSE_ENOUGH)
      && getTimeDifference(currTime) > 500) || getTimeDifference(currTime > 1200)) {
        liftDesiredPos -= CHAIN_TICKS_PER_CONE;
        currTime = chTimeNow();
        stackStep = 7;
      }
  }

  //Sweep out
  if(stackStep == 6) {
    if((abs(liftDesiredPos - vexSensorValueGet(S_CHAIN_LIFT_ENC) < LIFT_CLOSE_ENOUGH)
      && getTimeDifference(currTime) > 500) || getTimeDifference(currTime > 1000)) {
        sweepDesiredPos = SWEEP_OUT_POS;
        currTime = chTimeNow();
        stackStep = 7;
      }
  }

  //Drop to start height
  if(stackStep == 7) {
    if((abs(sweepDesiredPos - vexSensorValueGet(SWEEP_POT) < SWEEP_CLOSE_ENOUGH)
      && getTimeDifference(currTime) > 200) || getTimeDifference(currTime > 500)) {
        liftDesiredPos = LIFT_START_HEIGHT;
        currTime = chTimeNow();
        stackStep = -1;
      }
  }

  /*
  currTime = chTimeNow();
  while(autoStackMode) {

    vex_printf("flipperDesiredPos: %d\n", flipperDesiredPos);
    if(vexControllerGet(J_MODE_TOGGLE)) {
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
  */
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
  int liftPos;
  int sweepPos;
  while(1) {
    if(autoStackMode){
      if(sweepDesiredPos > 0) {
        sweepPos = vexSensorValueGet(SWEEP_POT);
        SetMotor(M_SWEEP, (sweepDesiredPos - sweepPos)/SWEEP_SEEK_RATE);
      } else {
        SetMotor(M_SWEEP, 0);
      }
      if(liftDesiredPos> 0) {
        liftPos = vexSensorValueGet(S_CHAIN_LIFT_ENC);
        SetMotor(M_CHAIN_LIFT, (liftDesiredPos - liftPos)/  LIFT_SEEK_RATE);
      } else {
        SetMotor(M_CHAIN_LIFT, 0);
      }
    }
    wait1Msec(20);
  }
  /*
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
        SetMotor(M_CHAIN_LIFT, (liftDesiredPos - liftPos)/  LIFT_SEEK_RATE);
      } else {
        SetMotor(M_CHAIN_LIFT, 0);
        SetMotor(M_CHAIN_LIFT_L, 0);
      }
    }
    wait1Msec(20);
  }
  */
}


void drive_forward(double ratio){
	systime_t init_time = chTimeNow();

	systime_t duration = abs(ratio*700);
	int32_t target = 1500*ratio;

	EPidEnable(rightDrivePid, duration, target);
	EPidEnable(leftDrivePid, duration, target);
	// systime_t end_time = chTimeNow() + duration;

	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			SetMotor(M_DRIVE_LEFT_F,  0);
			SetMotor(M_DRIVE_RIGHT_F, 0);
			SetMotor(M_DRIVE_LEFT_B,  0);
			SetMotor(M_DRIVE_RIGHT_B, 0);
			break;
		}
		// int backupLevel = BackupBatteryLevel;
	//vex_printf("%d    %d   ", vexSpiGetMainBattery(), vexSpiGetBackupBattery());
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			int16_t ld = EPidUpdate(leftDrivePid);
			int16_t rd = EPidUpdate(rightDrivePid);
			SetMotor(M_DRIVE_LEFT_F,  ld);
			SetMotor(M_DRIVE_RIGHT_F, rd);
			SetMotor(M_DRIVE_LEFT_B,  ld);
			SetMotor(M_DRIVE_RIGHT_B, rd);
			vexSleep(10);
		}
		else{
			SetMotor(M_DRIVE_LEFT_F,  0);
			SetMotor(M_DRIVE_RIGHT_F, 0);
			SetMotor(M_DRIVE_LEFT_B,  0);
			SetMotor(M_DRIVE_RIGHT_B, 0);
			break;
		}
	}
	SetMotor(M_DRIVE_LEFT_F,  0);
	SetMotor(M_DRIVE_RIGHT_B, 0);
	SetMotor(M_DRIVE_LEFT_F,  0);
	SetMotor(M_DRIVE_RIGHT_B, 0);
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
			SetMotor(M_DRIVE_LEFT_F,  0);
			SetMotor(M_DRIVE_RIGHT_F, 0);
			SetMotor(M_DRIVE_LEFT_B,  0);
			SetMotor(M_DRIVE_RIGHT_B, 0);

			break;
		}

		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			int16_t ld = EPidUpdate(leftDrivePid);
			int16_t rd = EPidUpdate(rightDrivePid);
			SetMotor(M_DRIVE_LEFT_F,  ld);
			SetMotor(M_DRIVE_RIGHT_F, rd);
			SetMotor(M_DRIVE_LEFT_B,  ld);
			SetMotor(M_DRIVE_RIGHT_B, rd);
			vexSleep(10);
		}
		else{

			break;
		}
	}
	SetMotor(M_DRIVE_LEFT_F,   0);
	SetMotor(M_DRIVE_RIGHT_F, 0);
	SetMotor(M_DRIVE_LEFT_B,  0);
	SetMotor(M_DRIVE_RIGHT_B, 0);
}


void wait(double ratio){
	systime_t init_time = chTimeNow();
	systime_t duration = abs(ratio*1000);
	SetMotor(M_DRIVE_LEFT_F,   0);
	SetMotor(M_DRIVE_RIGHT_F, 0);
	SetMotor(M_DRIVE_LEFT_B,  0);
	SetMotor(M_DRIVE_RIGHT_B, 0);

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
  short direction = DIRECTION_UP;

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

    if(vexControllerGet(J_MODE_TOGGLE)) {
      if(justChangedMode == false) {
        stackStep = 1;
        autoStackMode = !autoStackMode;
        justChangedMode = true;
      }
    } else {
      justChangedMode = false;
    }vexMotorSet(M_MOBILE_GOAL_R, 127);
        vexMotorSet(M_MOBILE_GOAL_L, 127);

	  if (vexControllerGet(J_MOBILE_BASE_UP)) {
      vexMotorSet(M_MOBILE_GOAL_R, 127);
      vexMotorSet(M_MOBILE_GOAL_L, 127);
    } else if (vexControllerGet(J_MOBILE_BASE_DOWN)) {
      vexMotorSet(M_MOBILE_GOAL_R, -127);
      vexMotorSet(M_MOBILE_GOAL_L, -127);
    } else {
      vexMotorSet(M_MOBILE_GOAL_R, 0);
      vexMotorSet(M_MOBILE_GOAL_L, 0);
    }

    if (vexControllerGet(J_CHAIN_LIFT_UP)) {
        vexMotorSet(M_CHAIN_LIFT, 127);
	  } else if (vexControllerGet(J_CHAIN_LIFT_DOWN)) {
        vexMotorSet(M_CHAIN_LIFT, -127);
      } else {
        vexMotorSet(M_CHAIN_LIFT, 0);
      }

  		//Don't hog cpu
  		vexSleep(10);
	   }
  }

	return (msg_t)0;

}
