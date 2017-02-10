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
#define M_CLAW           kVexMotor_2
#define M_LIFT_LEFT      kVexMotor_3
#define M_DRIVE_LEFT_B   kVexMotor_4
#define M_DRIVE_RIGHT_F  kVexMotor_6
#define M_DRIVE_RIGHT_B  kVexMotor_7
#define M_LIFT_RIGHT     kVexMotor_8
#define M_DRIVE_LEFT_F   kVexMotor_9



// Sensor mappings


#define P_DRIVE_ENC_RIGHT_A kVexDigital_9
#define P_DRIVE_ENC_RIGHT_B kVexDigital_10
#define P_DRIVE_ENC_LEFT_A  kVexDigital_1
#define P_DRIVE_ENC_LEFT_B  kVexDigital_2

#define P_CLAW_A kVexDigital_7
#define P_CLAW_B kVexDigital_8

#define P_LIFT_A   kVexDigital_5
#define P_LIFT_B  	kVexDigital_6

#define S_DRIVE_ENC_LEFT  	kVexSensorDigital_3
#define S_DRIVE_ENC_RIGHT 	kVexSensorDigital_9
#define S_LIFT       		kVexSensorDigital_5
#define S_CLAW           	kVexSensorDigital_7


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


#define J_CLAW_OPEN  Btn6D
#define J_CLAW_CLOSE   Btn6U

// PID Controls
EPidController *leftDrivePid;
EPidController *rightDrivePid;



//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {

    { M_CLAW,               kVexMotor393S, kVexMotorNormal,    kVexSensorQuadEncoder,  kVexQuadEncoder_2 },
    { M_LIFT_RIGHT,         kVexMotor393S, kVexMotorNormal,    kVexSensorQuadEncoder,  kVexQuadEncoder_4 },
    { M_LIFT_LEFT,          kVexMotor393S, kVexMotorNormal,  	kVexSensorQuadEncoder,  kVexQuadEncoder_4 },
    { M_DRIVE_RIGHT_B,      kVexMotor393S, kVexMotorNormal,  	kVexSensorQuadEncoder,  kVexQuadEncoder_1 },
    { M_DRIVE_LEFT_B,       kVexMotor393S, kVexMotorNormal,    kVexSensorQuadEncoder,  kVexQuadEncoder_3 },
    { M_DRIVE_LEFT_F,       kVexMotor393S, kVexMotorNormal,    kVexSensorQuadEncoder,  kVexQuadEncoder_3 },
    { M_DRIVE_RIGHT_F,      kVexMotor393S, kVexMotorNormal,  kVexSensorQuadEncoder,  kVexQuadEncoder_1 }
};

static vexDigiCfg dConfig[] = { 
    { P_DRIVE_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1, kVexQuadEncoder_1 },
    { P_DRIVE_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2, kVexQuadEncoder_1 },
    
    { P_DRIVE_ENC_LEFT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_3 },
    { P_DRIVE_ENC_LEFT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },
    
    { P_LIFT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_4 },
    { P_LIFT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_4 },
    
    { P_CLAW_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_2 },
    { P_CLAW_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_2 }
    
 };

//-------------------Setup----------------------------------------------------//
void vexUserSetup()
{
    vexMotorConfigure  ( mConfig,   MOT_CONFIG_SIZE( mConfig ) )  ;
    vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
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
	
	SetMotor(M_DRIVE_LEFT_B,  ld);
	SetMotor(M_DRIVE_RIGHT_B, rd);
	SetMotor(M_DRIVE_LEFT_F,  ld);
	SetMotor(M_DRIVE_RIGHT_F, rd);


	
	
	return (ld != 0 || rd != 0);
}



void Set_Claw_Motors(int val){
	SetMotor(M_CLAW, val);
}


void Set_Lift_Motors(int val){
	SetMotor(M_LIFT_LEFT, val);
	SetMotor(M_LIFT_RIGHT, val);
}


//---------------------Autonomous Functions ----------------------------------//

/**
 drive_forward(double ratio);
 param:
 @ratio is the ditance you want to move. One block take a second to move
 */

void drive_at(int to){
	SetMotor(M_DRIVE_LEFT_B,  to);
	SetMotor(M_DRIVE_RIGHT_B, to);
	SetMotor(M_DRIVE_LEFT_F,  to);
	SetMotor(M_DRIVE_RIGHT_F, to);
}


void drive_forward(double ratio){
	systime_t init_time = chTimeNow();
		vexSensorValueSet(S_DRIVE_ENC_RIGHT, 0);
		// int backupLevel = BackupBatteryLevel; 
	vex_printf("%d    %d   ", vexSpiGetMainBattery(), vexSpiGetBackupBattery());
	
	systime_t duration = abs(ratio*700);
	int32_t target = 1500*ratio;
	EPidEnable(rightDrivePid,duration, target);
	EPidEnable(leftDrivePid,duration, target);
	// systime_t end_time = chTimeNow() + duration;
	
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			drive_at(0);
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
			drive_at(0);
			break;
		}
	}
	vex_printf("%d\n", vexSensorValueGet(S_DRIVE_ENC_RIGHT));
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
			vexMotorSet(M_DRIVE_LEFT_B,  0);
			vexMotorSet(M_DRIVE_RIGHT_B, 0);
			vexMotorSet(M_DRIVE_LEFT_F,  0);
			vexMotorSet(M_DRIVE_RIGHT_F, 0);
			break;
		}
		
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			int16_t ld = EPidUpdate(leftDrivePid);
			int16_t rd = EPidUpdate(rightDrivePid);
			vexMotorSet(M_DRIVE_LEFT_B,  ld);
			vexMotorSet(M_DRIVE_RIGHT_B, rd);
			vexMotorSet(M_DRIVE_LEFT_F,  ld);
			vexMotorSet(M_DRIVE_RIGHT_F, rd);
			vexSleep(10);
		}
		else{
			
			break;
		}
	}
	vexMotorSet(M_DRIVE_LEFT_B,   0);
	vexMotorSet(M_DRIVE_RIGHT_B, 0);
	vexMotorSet(M_DRIVE_LEFT_F,  0);
	vexMotorSet(M_DRIVE_RIGHT_F, 0);
}


void wait(double ratio){
	systime_t init_time = chTimeNow();
	systime_t duration = abs(ratio*1000);
	vexMotorSet(M_DRIVE_LEFT_B,   0);
	vexMotorSet(M_DRIVE_RIGHT_B, 0);
	vexMotorSet(M_DRIVE_LEFT_F,  0);
	vexMotorSet(M_DRIVE_RIGHT_F, 0);
	
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

void claw_close(void){
	// Do Some shit
	systime_t init_time = chTimeNow();
	systime_t duration = abs(700);
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			break;
		}
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration && vexSensorValueGet(S_CLAW) > 0){
			Set_Claw_Motors(-120);
		}
		else{
			Set_Claw_Motors(0);
			break;
		}
	}
	Set_Claw_Motors(0);

}



void claw_open(void){
	systime_t init_time = chTimeNow();
	systime_t duration = abs(700);
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			break;
		}
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration && vexSensorValueGet(S_CLAW) < 3500){
			Set_Claw_Motors(120);
		}
		else{
			Set_Claw_Motors(0);
			break;
		}
	}
	Set_Claw_Motors(-10);

}





void lift_Auton(void)
{
	systime_t t = chTimeNow();
	vexSensorValueSet(S_LIFT, 0);
	while ((chTimeNow() - t) < 700){
		if(vexControllerGet(Btn7R)){
			return;
		}
		drive_at(-125);
		Set_Lift_Motors(-125);
		if ((vexSensorValueGet(S_CLAW))>500){
			Set_Claw_Motors(-80);
		}
		else{
			Set_Claw_Motors(0);
		}
		
	}
	drive_at(0);
	
	while ((vexSensorValueGet(S_LIFT) > -25902) && (chTimeNow ()- t)< 7000) 
	{
		if(vexControllerGet(Btn7R)){
			return;
		}
		Set_Lift_Motors(-125);
	} 

	while(((vexSensorValueGet(S_CLAW))>20) && (chTimeNow ()- t)< 8000)
	{
		if(vexControllerGet(Btn7R)){
			return;
		}
		Set_Lift_Motors(-100);
		Set_Claw_Motors(-125);
	}



}

void slight_dumper_lift(void){
 	systime_t init_time = chTimeNow();
 	systime_t duration = abs(500);

	while (!chThdShouldTerminate()){
     	if(vexControllerGet(Btn7R)){
         	break;
     	}
     	systime_t autonTime = chTimeNow() - init_time;
     	if (autonTime < 300){
     		Set_Claw_Motors(125);
     	}
     	else{
     		Set_Claw_Motors(15);
         	// Set_Lift_Motors(20);
         	break;
     	}
 	}

 	while (!chThdShouldTerminate()){
     	if(vexControllerGet(Btn7R)){
         	break;
     	}
     	systime_t autonTime = chTimeNow() - init_time;
     	if (autonTime < 500){
     		Set_Lift_Motors(95);
     	}
     	else{
     		// Set_Claw_Motors(15);
         	Set_Lift_Motors(20);
         	break;
     	}
 	}
 }

 void dump_to_zero(void){
 	systime_t init_time = chTimeNow();
 	systime_t duration = abs(500);

	while (!chThdShouldTerminate()){
     	if(vexControllerGet(Btn7R)){
         	break;
     	}
     	systime_t autonTime = chTimeNow() - init_time;
     	if (vexSensorValueGet(S_LIFT)>10 && autonTime < duration){
     		Set_Lift_Motors(-125);
     	}
     	else{
     		Set_Lift_Motors(0);
         	// Set_Lift_Motors(20);
         	break;
     	}
 	}

 	
 }

task fight_against_bands(void){

	if (vexSensorValueGet(S_CLAW)>3000){
        	Set_Claw_Motors(80);
	}
	else if (vexSensorValueGet(S_CLAW)>2000){
        	Set_Claw_Motors(-10);
	}


}

void Dump_Open_Claw(void){
	// systime_t t = chTimeNow();
	vexSensorValueSet(S_LIFT, 0);
	
	while ((vexSensorValueGet(S_LIFT) < 15000)) 
	{
		if(vexControllerGet(Btn7R)){
			return;
		}
		Set_Lift_Motors(125);
	} 

	while(((vexSensorValueGet(S_CLAW))>2000))
	{
		if(vexControllerGet(Btn7R)){
			return;
		}
		if (((vexSensorValueGet(S_LIFT) < 18000))){
			Set_Lift_Motors(100);
			Set_Claw_Motors(-120);
		}
		else{

			Set_Lift_Motors(00);
			Set_Claw_Motors(0);
			break;
		}
		
	}

}



void Dump_Down(void){
	// systime_t t = chTimeNow()
	
	while ((vexSensorValueGet(S_LIFT) > 100)) 
	{
		if(vexControllerGet(Btn7R)){
			return;
		}
		Set_Lift_Motors(-125);
	} 
	Set_Lift_Motors(0);		
}

//---------------------Autonomous routine-------------------------------------//

msg_t vexAutonomous( void *arg )
{
	(void)arg;
	vexTaskRegister("auton");
	StartTask(fight_against_bands);
	claw_open();
    
   	turn_deg(0.22);
	drive_forward(-0.7);
    claw_open();
   	drive_forward(-0.7);
   	slight_dumper_lift();
   	wait(1);
   	turn_deg(0.35);

   	wait(0.5);
   	drive_forward(0.6);

   	dump_to_zero();
   	wait(1);
    
	Dump_Open_Claw();
	wait(1);
	Dump_Down();
   wait(1);
//    Dump_Down();
//    wait(1.2);
    claw_close();
   	drive_forward(-1);
    claw_open();
    claw_open();
    wait(1);
    drive_forward(1);
    wait(0.5);
    Dump_Open_Claw();
    wait(0.5);
	//turn_deg(0.3);
	//drive_forward(0.9);
	//Dump_Open_Claw();
	//wait(1);
	Dump_Down();


	
	return (msg_t)0;
}


//---------------------User control Functions----------------------------------//




//---------------------User control settings----------------------------------//


msg_t vexOperator( void *arg )
{
	(void)arg;
	vexTaskRegister("operator");
	// int16_t left_lift_val = -580, right_lift_val = 580;
	// 
	// bool keepLiftinPlace = false, keepDumpInPlace = false;
	
	//Clear encoders
	// clearDriveEncoders();
	// bool
	while(!chThdShouldTerminate())
	{
		//Remember to Uncomment.
		driveMotors();
		
		// /= false;
		if(vexControllerGet(Btn7L)){
			// Dump_Down();
			vexAutonomous(NULL);
		}

		// if(vexControllerGet(Btn7U)){
		// 	Dump_Down();
		// }

		
		if(vexControllerGet(J_LIFT_UP)){
			Set_Lift_Motors(100);
		}
		else if(vexControllerGet(J_LIFT_DOWN)){
			Set_Lift_Motors(-100);
		} else{
			Set_Lift_Motors(0);
		}
		
		
		if(vexControllerGet(J_CLAW_CLOSE)){
			Set_Claw_Motors(100);
		}
		else if(vexControllerGet(J_CLAW_OPEN)){
			Set_Claw_Motors(-100);
		} else if (vexSensorValueGet(S_CLAW)>2000){
			Set_Claw_Motors(-10);
		}
		else{
			Set_Claw_Motors(0);
		}
		
		
	
		 
		//Don't hog cpu
		vexSleep(10);
	}
	return (msg_t)0;
}
