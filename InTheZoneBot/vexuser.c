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

// Lift
#define M_LIFT_LEFT   kVexMotor_2
#define M_LIFT_RIGHT  kVexMotor_8

#define M_DRIVE_RIGHT_F  kVexMotor_3
#define M_DRIVE_RIGHT_B  kVexMotor_4
#define M_DRIVE_LEFT_B   kVexMotor_7
#define M_DRIVE_LEFT_F   kVexMotor_8

// Sensor mappings
#define P_DRIVE_ENC_RIGHT_A kVexDigital_9
#define P_DRIVE_ENC_RIGHT_B kVexDigital_10
#define P_DRIVE_ENC_LEFT_A  kVexDigital_1
#define P_DRIVE_ENC_LEFT_B  kVexDigital_2

#define P_CLAW 			kVexDigital_5

#define P_LIFT_A   			kVexDigital_3
#define P_LIFT_B  			kVexDigital_4

#define S_DRIVE_ENC_LEFT  	kVexSensorDigital_1
#define S_DRIVE_ENC_RIGHT 	kVexSensorDigital_9
#define S_LIFT       		kVexSensorDigital_3


#define S_LINE_FOLLOWER_L2      1
#define S_LINE_FOLLOWER_L1      2
#define S_LINE_FOLLOWER_M       3
#define S_LINE_FOLLOWER_R1      4
#define S_LINE_FOLLOWER_R2      5

// Controller mappings
#define J_LIFT_UP     Btn5U
#define J_LIFT_DOWN   Btn5D

#define J_DRIVE       Ch3
#define J_TURN        Ch1


// PID Controls
EPidController *leftDrivePid;
EPidController *rightDrivePid;

// bool auton_mode;

//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {

    { M_LIFT_RIGHT,       kVexMotor393S, kVexMotorReversed,    	kVexSensorQuadEncoder,  kVexQuadEncoder_2 },
    { M_LIFT_LEFT,        kVexMotor393S, kVexMotorReversed,  	  kVexSensorQuadEncoder,  kVexQuadEncoder_2 },

    { M_DRIVE_RIGHT_B,      kVexMotor393S, kVexMotorNormal,  	    kVexSensorQuadEncoder,  kVexQuadEncoder_1 },
    { M_DRIVE_LEFT_B,       kVexMotor393S, kVexMotorNormal,    	  kVexSensorQuadEncoder,  kVexQuadEncoder_3 },
    { M_DRIVE_LEFT_F,       kVexMotor393S, kVexMotorNormal,    	  kVexSensorQuadEncoder,  kVexQuadEncoder_3 },
    { M_DRIVE_RIGHT_F,      kVexMotor393S, kVexMotorNormal,  	    kVexSensorQuadEncoder,  kVexQuadEncoder_1 }
};

static vexDigiCfg dConfig[] = {
    { P_DRIVE_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_1 },
    { P_DRIVE_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_1 },

    { P_DRIVE_ENC_LEFT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_3 },
    { P_DRIVE_ENC_LEFT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },

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
	int forward = VALLEY(vexControllerGet(J_DRIVE), 20, 127) * -0.8;
	int turn;


	int turnChannel = vexControllerGet(J_TURN) * -0.6;

	turn = VALLEY(turnChannel, 20, 127);
	ld = VALLEY(forward + turn, 20, 127);
	rd = VALLEY(forward - turn, 20, 127);

	//SetMotor(M_DRIVE_LEFT_B,  ld);
	//SetMotor(M_DRIVE_RIGHT_B, rd);
	//SetMotor(M_DRIVE_LEFT_F,  ld);
	//SetMotor(M_DRIVE_RIGHT_F, rd);

	vex_printf("%f %f\n", rd, ld);
	return (ld != 0 || rd != 0);
}

void Set_Lift_Motors(int val){
	//SetMotor(M_LIFT_LEFT_1, val);
	//SetMotor(M_LIFT_LEFT_2, val);
	//SetMotor(M_LIFT_RIGHT_1, val);
	//SetMotor(M_LIFT_RIGHT_2, val);
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
			SetMotor(M_DRIVE_LEFT_B,  0);
			SetMotor(M_DRIVE_RIGHT_B, 0);
			SetMotor(M_DRIVE_LEFT_F,  0);
			SetMotor(M_DRIVE_RIGHT_F, 0);
			break;
		}

		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			int16_t ld = EPidUpdate(leftDrivePid);
			int16_t rd = EPidUpdate(rightDrivePid);
			SetMotor(M_DRIVE_LEFT_B,  ld);
			SetMotor(M_DRIVE_RIGHT_B, rd);
			SetMotor(M_DRIVE_LEFT_F,  ld);
			SetMotor(M_DRIVE_RIGHT_F, rd);
			vexSleep(10);
		}
		else{
			SetMotor(M_DRIVE_LEFT_B,  0);
			SetMotor(M_DRIVE_RIGHT_B, 0);
			SetMotor(M_DRIVE_LEFT_F,  0);
			SetMotor(M_DRIVE_RIGHT_F, 0);
			break;
		}
	}
	SetMotor(M_DRIVE_LEFT_B,  0);
	SetMotor(M_DRIVE_RIGHT_B, 0);
	SetMotor(M_DRIVE_LEFT_F,  0);
	SetMotor(M_DRIVE_RIGHT_F, 0);
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
			SetMotor(M_DRIVE_LEFT_B,  0);
			SetMotor(M_DRIVE_RIGHT_B, 0);
			SetMotor(M_DRIVE_LEFT_F,  0);
			SetMotor(M_DRIVE_RIGHT_F, 0);

			break;
		}

		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			int16_t ld = EPidUpdate(leftDrivePid);
			int16_t rd = EPidUpdate(rightDrivePid);
			SetMotor(M_DRIVE_LEFT_B,  ld);
			SetMotor(M_DRIVE_RIGHT_B, rd);
			SetMotor(M_DRIVE_LEFT_F,  ld);
			SetMotor(M_DRIVE_RIGHT_F, rd);
			vexSleep(10);
		}
		else{

			break;
		}
	}
	SetMotor(M_DRIVE_LEFT_B,   0);
	SetMotor(M_DRIVE_RIGHT_B, 0);
	SetMotor(M_DRIVE_LEFT_F,  0);
	SetMotor(M_DRIVE_RIGHT_F, 0);
}


void wait(double ratio){
	systime_t init_time = chTimeNow();
	systime_t duration = abs(ratio*1000);
	SetMotor(M_DRIVE_LEFT_B,   0);
	SetMotor(M_DRIVE_RIGHT_B, 0);
	SetMotor(M_DRIVE_LEFT_F,  0);
	SetMotor(M_DRIVE_RIGHT_F, 0);

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
	bool inPlace = false;

	while(!chThdShouldTerminate())
	{
		driveMotors();

		if (vexControllerGet(Btn7L))
		{
			vexAutonomous(NULL);
		}

		if(vexControllerGet(J_LIFT_UP)){
			vexMotorSet(M_LIFT_LEFT,  127);
      vexMotorSet(M_LIFT_RIGHT, 127);
			inPlace = true;
		}
		else if(vexControllerGet(J_LIFT_DOWN)){
      vexMotorSet(M_LIFT_LEFT,  -127);
      vexMotorSet(M_LIFT_RIGHT, -127);
			inPlace = false;
		} else if (inPlace){
			Set_Lift_Motors(10);
		} else{
      vexMotorSet(M_LIFT_LEFT,  0);
      vexMotorSet(M_LIFT_RIGHT, 0);
		}

		//Don't hog cpu
		vexSleep(10);
	}
	return (msg_t)0;
}
