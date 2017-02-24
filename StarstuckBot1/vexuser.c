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
#define M_LIFT_RIGHT_2   kVexMotor_2
#define M_LIFT_LEFT_2    kVexMotor_6
#define M_LIFT_LEFT_1    kVexMotor_9
#define M_LIFT_RIGHT_1   kVexMotor_5


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


#define J_CLAW_OPEN  Btn6D
#define J_CLAW_CLOSE   Btn6U

#define J_Auton_Climb Btn7L

// PID Controls
EPidController *leftDrivePid;
EPidController *rightDrivePid;

// bool auton_mode;



//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {

    { M_LIFT_RIGHT_2,       kVexMotor393S, kVexMotorReversed,    	kVexSensorQuadEncoder,  kVexQuadEncoder_2 },
    { M_LIFT_RIGHT_1,       kVexMotor393S, kVexMotorReversed,    	kVexSensorQuadEncoder,  kVexQuadEncoder_2 },
    { M_LIFT_LEFT_1,        kVexMotor393S, kVexMotorReversed,  	kVexSensorQuadEncoder,  kVexQuadEncoder_2 },
    { M_LIFT_LEFT_2,        kVexMotor393S, kVexMotorReversed,  	kVexSensorQuadEncoder,  kVexQuadEncoder_2 },
    
    { M_DRIVE_RIGHT_B,      kVexMotor393S, kVexMotorNormal,  	kVexSensorQuadEncoder,  kVexQuadEncoder_1 },
    { M_DRIVE_LEFT_B,       kVexMotor393S, kVexMotorNormal,    	kVexSensorQuadEncoder,  kVexQuadEncoder_3 },
    { M_DRIVE_LEFT_F,       kVexMotor393S, kVexMotorNormal,    	kVexSensorQuadEncoder,  kVexQuadEncoder_3 },
    { M_DRIVE_RIGHT_F,      kVexMotor393S, kVexMotorNormal,  	kVexSensorQuadEncoder,  kVexQuadEncoder_1 }
};

static vexDigiCfg dConfig[] = { 
    { P_DRIVE_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1, kVexQuadEncoder_1 },
    { P_DRIVE_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2, kVexQuadEncoder_1 },

    { P_DRIVE_ENC_LEFT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_3 },
    { P_DRIVE_ENC_LEFT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },

    { P_LIFT_A,  		   kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_2 },
    { P_LIFT_B,  		   kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_2 },

    { P_CLAW,  			   kVexSensorDigitalInput,  kVexConfigInput,       0 },
   
 };

//-------------------Setup----------------------------------------------------//
void vexUserSetup()
{
    vexMotorConfigure   ( mConfig,   MOT_CONFIG_SIZE( mConfig ) )  ;
    vexDigitalConfigure ( dConfig, DIG_CONFIG_SIZE( dConfig ) );
}

void vexUserInit()
{

	// SmartMotorsInit();
 //    SmartMotorPtcMonitorEnable();
 //    SmartMotorRun();

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
	
	vexMotorSet(M_DRIVE_LEFT_B,  ld);
	vexMotorSet(M_DRIVE_RIGHT_B, rd);
	vexMotorSet(M_DRIVE_LEFT_F,  ld);
	vexMotorSet(M_DRIVE_RIGHT_F, rd);
	
	return (ld != 0 || rd != 0);
}

void Set_Claw_Motors(int val){
	vexDigitalPinSet(P_CLAW, val);
}


void Set_Lift_Motors(int val){
	vexMotorSet(M_LIFT_LEFT_1, val);
	vexMotorSet(M_LIFT_LEFT_2, val);
	vexMotorSet(M_LIFT_RIGHT_1, val);
	vexMotorSet(M_LIFT_RIGHT_2, val);
}


//---------------------Autonomous Functions ----------------------------------//

/**
 drive_forward(double ratio);
 param:
 @ratio is the ditance you want to move. One block take a second to move
 */

void drive_forward(double ratio){
	systime_t init_time = chTimeNow();
		vexSensorValueSet(S_DRIVE_ENC_RIGHT, 0);
		// int backupLevel = BackupBatteryLevel; 
	//vex_printf("%d    %d   ", vexSpiGetMainBattery(), vexSpiGetBackupBattery());
	
	systime_t duration = abs(ratio*700);
	int32_t target = 1500*ratio;
	EPidEnable(rightDrivePid,duration, target);
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
			vexMotorSet(M_DRIVE_LEFT_B,  0);
			vexMotorSet(M_DRIVE_RIGHT_B, 0);
			vexMotorSet(M_DRIVE_LEFT_F,  0);
			vexMotorSet(M_DRIVE_RIGHT_F, 0);
			break;
		}
	}
	vexMotorSet(M_DRIVE_LEFT_B,  0);
	vexMotorSet(M_DRIVE_RIGHT_B, 0);
	vexMotorSet(M_DRIVE_LEFT_F,  0);
	vexMotorSet(M_DRIVE_RIGHT_F, 0);
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


void slight_dumper_lift(void)
{
 	systime_t init_time = chTimeNow();
 	systime_t duration = abs(350);

 	while (!chThdShouldTerminate()){
     	if(vexControllerGet(Btn7R)){
         	break;
     	}
     	systime_t autonTime = chTimeNow() - init_time;
     	if (autonTime < duration){
     		Set_Lift_Motors(105);
     	}
     	else{
     		// Set_Claw_Motors(15);
         	Set_Lift_Motors(30);
         	break;
     	}
 	}
 }

void dump_to_zero(void)
{
	while (!chThdShouldTerminate()){
     	if(vexControllerGet(Btn7R)){
         	break;
     	}
     	if (vexSensorValueGet(S_LIFT)>-70){
     		Set_Lift_Motors(-90);
     	}
     	else{
     		Set_Lift_Motors(0);
         	// Set_Lift_Motors(20);
         	break;
     	}
 	}
}


void Dump_Open_Claw(void){
	// systime_t t = chTimeNow();

	systime_t init_time = chTimeNow();
	systime_t duration = abs(1000);


	// dump_to_zero();
	vexSensorValueSet(S_LIFT, 0);
	// drive_forward(0.6);
	while ((vexSensorValueGet(S_LIFT) < 11000) || (chTimeNow() - init_time) < duration) 
	{
		if(vexControllerGet(Btn7R) || (chTimeNow() - init_time) > duration ){
			Set_Claw_Motors(0);
			Set_Lift_Motors(0);
			return;
		}
		// if (!auton_mode){
		// 	driveMotors();
		// }
		if (((vexSensorValueGet(S_LIFT) > 7000))){
			Set_Claw_Motors(0);
			// Set_Claw_Motors(-120);
		} else{
			Set_Claw_Motors(1);
		}
		
		Set_Lift_Motors(125);
	} 
	Set_Lift_Motors(0);
	// Set_Claw_Motors(10);

	vex_printf("I am out of Dump Open Claw");

}



void Dump_Close_Claw(void){
	// systime_t t = chTimeNow();
	//vexSensorValueSet(S_LIFT, 0);
	
	Set_Claw_Motors(0);
	while ((vexSensorValueGet(S_LIFT) > 100)) 
	{
		if(vexControllerGet(Btn5D)){
			return;
		}
		
		Set_Lift_Motors(-125);
		
	} 

}


void open_claw_init(void)
{
	systime_t init_time = chTimeNow();
 	systime_t duration = abs(100);

	while ((chTimeNow() - init_time) < duration){
     	if(vexControllerGet(Btn7R)){
			return;
		}
     	Set_Lift_Motors(125);
   		
 	}
 	Set_Lift_Motors(0);
 	Set_Claw_Motors(0);

 	while (!chThdShouldTerminate()){
     	if(vexControllerGet(Btn7R)){
         	break;
     	}
     	if (vexSensorValueGet(S_LIFT)>-70){
     		Set_Lift_Motors(-70);
     	}
     	else{
     		Set_Lift_Motors(0);
         	// Set_Lift_Motors(20);
         	break;
     	}
 	}
}

//---------------------Autonomous routine-------------------------------------//

msg_t vexAutonomous( void *arg )
{
	(void)arg;
	vexTaskRegister("auton");
	// auton_mode = true;
	vexSensorValueSet(S_LIFT, 0);
	// drive_forward(-0.75);

   	open_claw_init();
   	wait(1);
 	drive_forward(-1.2);
 	Set_Claw_Motors(1);
   	slight_dumper_lift();
   	wait(.5);
   	turn_deg(0.2);
   	wait(0.5);
   	drive_forward(-2);
   	wait(0.55);
   	turn_deg(0.35);
   	wait(0.5);
   	drive_forward(0.6);

	Dump_Open_Claw();
	wait(.5);

	dump_to_zero();

	// // turn_deg(0.2);

   	drive_forward(-0.6);

   	// turn_deg(0.25);

   	// drive_forward(-3);

   	// turn_deg(0.2);
 //   	wait(1);
 //   	Set_Claw_Motors(1);

	// slight_dumper_lift();
	// drive_forward(1.4);
	// wait(1);
	// // turn_deg(-0.2);
	// wait(1);
	// Dump_Open_Claw();





	
	return (msg_t)0;
}





//---------------------User control Functions----------------------------------//
/**

	Dump_to45();
	// claw_open();
	// Dump_Open_Claw();
	drive_forward(-1.5);
	
	dump_to_zero();
	claw_open();
	Claw_to45();
*/

	//___________________
	// drive_forward(1);
	// drive_forward(-1);
	// claw_open();
	// drive_forward(-0.5);
	// wait(0.5);
	// claw_open();
	// slight_dumper_lift();
	// wait(0.5);
	// drive_forward(1);
	// wait(0.5);
	// Dump_Open_Claw();
	// Dump_Down();
	// drive_forward(-1.7);
	

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
	// bool x=false;
	// bool y = false;

	// bool climb_mode = 0;
	// bool


	while(!chThdShouldTerminate())
	{
		//Remember to Uncomment.
		driveMotors();
		// auton_mode = false;

		if (vexControllerGet(Btn7L))
		{
			vexAutonomous(NULL);
		}

		// turn_on_SmartMotors();
		
		if(vexControllerGet(J_LIFT_UP)){
			Set_Lift_Motors(120);			
		}
		else if(vexControllerGet(J_LIFT_DOWN)){
			Set_Lift_Motors(-60);			
		} else{
			Set_Lift_Motors(0);
		}


		
		if(vexControllerGet(J_CLAW_CLOSE)){
			Set_Claw_Motors(0);
		}
		else if(vexControllerGet(J_CLAW_OPEN)){
			Set_Claw_Motors(1);
		}
		else{
			// Set_Claw_Motors(0);
		}

		
		//Don't hog cpu
		vexSleep(10);
	}
	return (msg_t)0;
}
