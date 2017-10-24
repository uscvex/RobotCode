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
#define M_DRIVE_RIGHT_2  kVexMotor_6
#define M_DRIVE_LEFT_1   kVexMotor_10
#define M_DRIVE_LEFT_2   kVexMotor_5

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

#define FLIP_POT            kVexAnalog_1
#define R_MOBILE_POT        kVexAnalog_2
#define L_MOBILE_POT        kVexAnalog_3

// Controller mappings
#define J_DRIVE_RIGHT Ch2
#define J_DRIVE_LEFT  Ch3

#define J_AUTOSTACK        Btn6U
#define J_FLIP_LIFT_HOME   Btn6D
#define J_MOBILE_BASE_DOWN Btn5U
#define J_MOBILE_BASE_UP   Btn5D
#define J_CANCEL_AUTOSTACK Btn7U
#define J_AUTO_PRELOADS    Btn8R //Toggle
#define J_CHAIN_LIFT_UP    Btn8U
#define J_CHAIN_LIFT_DOWN  Btn8L
#define J_FLIP_DOWN        Btn7L
#define J_FLIP_UP          Btn7R
#define J_SPIN_ROLL        Btn8D

// PID Controls
EPidController *leftDrivePid;
EPidController *rightDrivePid;

// bool auton_mode;

//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {

    { M_MOBILE_GOAL_L,       kVexMotor393S, kVexMotorReversed,    	kVexSensorQuadEncoder,  kVexQuadEncoder_2 },
    { M_MOBILE_GOAL_R,       kVexMotor393S, kVexMotorReversed,  	  kVexSensorQuadEncoder,  kVexQuadEncoder_2 },

    { M_DRIVE_RIGHT_1,       kVexMotor393S, kVexMotorNormal,  	    kVexSensorQuadEncoder,  kVexQuadEncoder_1 },
    { M_DRIVE_RIGHT_2,       kVexMotor393S, kVexMotorNormal,    	  kVexSensorQuadEncoder,  kVexQuadEncoder_3 },
    { M_DRIVE_LEFT_1,        kVexMotor393S, kVexMotorNormal,    	  kVexSensorQuadEncoder,  kVexQuadEncoder_3 },
    { M_DRIVE_LEFT_2,        kVexMotor393S, kVexMotorNormal,  	    kVexSensorQuadEncoder,  kVexQuadEncoder_1 },

    { M_ROLLER,              kVexMotor393S, kVexMotorNormal,  	    kVexSensorQuadEncoder,  kVexQuadEncoder_1 },
    { M_FLIP,                kVexMotor393S, kVexMotorNormal,  	    kVexSensorQuadEncoder,  kVexQuadEncoder_1 }
};

static vexDigiCfg dConfig[] = {
    { P_DRIVE_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_1 },
    { P_DRIVE_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_1 },

    { P_DRIVE_ENC_LEFT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_3 },
    { P_DRIVE_ENC_LEFT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },

    { P_LIFT_ENC_LEFT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },
    { P_LIFT_ENC_LEFT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },

    { P_LIFT_ENC_RIGHT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },
    { P_LIFT_ENC_RIGHT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },

    { P_ENC_TOP_ROLLER_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },
    { P_ENC_TOP_ROLLER_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 }

 };

//-------------------Setup----------------------------------------------------//
void vexUserSetup()
{
    vexMotorConfigure   ( mConfig,   MOT_CONFIG_SIZE( mConfig ) )  ;
    vexDigitalConfigure ( dConfig, DIG_CONFIG_SIZE( dConfig ) );
}

void vexUserInit()
{
	  SmartMotorsInit();
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

	SetMotor(M_DRIVE_LEFT_1,  ld);
	SetMotor(M_DRIVE_LEFT_2,  ld);
	SetMotor(M_DRIVE_RIGHT_1, rd);
	SetMotor(M_DRIVE_RIGHT_2, rd);

	vex_printf("%f %f\n", rd, ld);
	return (ld != 0 || rd != 0);
}

void autoStack() {

}

//---------------------Autonomous Functions ----------------------------------//

/**
 drive_forward(double ratio);
 param:
 @ratio is the ditance you want to move. One block take a second to move
 */
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

  bool autoStackMode = false;

	while(!chThdShouldTerminate())
	{
    if(autoStackMode) {

    }

    else {

      driveMotors();

      if(vexControllerGet(J_AUTOSTACK)) {
        autoStackMode = !autoStackMode;
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
        vexMotorSet(M_MOBILE_GOAL_L, 0);
      }

      if (vexControllerGet(J_CHAIN_LIFT_UP)) {
        vexMotorSet(M_CHAIN_LIFT_R, 127);
        vexMotorSet(M_CHAIN_LIFT_L, 127);
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
