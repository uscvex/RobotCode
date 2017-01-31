#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"
#include "../Common/easing.h"
#include "../Common/linefollower.h"

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


#define P_DRIVE_ENC_RIGHT_A kVexDigital_7
#define P_DRIVE_ENC_RIGHT_B kVexDigital_8
#define P_DRIVE_ENC_LEFT_A  kVexDigital_9
#define P_DRIVE_ENC_LEFT_B  kVexDigital_10


#define P_DUMP_A kVexDigital_5
#define P_DUMP_B kVexDigital_6


#define P_LIFT_LEFT_A   kVexDigital_1
#define P_LIFT_LEFT_B  kVexDigital_2

#define P_LIFT_RIGHT_A   kVexDigital_3
#define P_LIFT_RIGHT_B  kVexDigital_4


#define S_DRIVE_ENC_LEFT  kVexSensorDigital_9
#define S_DRIVE_ENC_RIGHT kVexSensorDigital_7
#define S_LIFT_LEFT       kVexSensorDigital_1
#define S_LIFT_RIGHT      kVexSensorDigital_3
#define S_DUMP           kVexSensorDigital_5


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


#define J_DUMPER_DOWN   Btn6D
#define J_DUMPER_UP   Btn6U


#define AUTON_SPEED 350


// PID Controls
EPidController *leftDrivePid;
EPidController *rightDrivePid;



//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {

    { M_CLAW,               kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
    { M_LIFT_RIGHT,         kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
    { M_LIFT_LEFT,          kVexMotor393S, kVexMotorReversed,  kVexSensorNone,  0 },
    { M_DRIVE_RIGHT_B,      kVexMotor393S, kVexMotorReversed,  kVexSensorNone,  0 },
    { M_DRIVE_LEFT_B,       kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
    { M_DRIVE_LEFT_F,       kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
    { M_DRIVE_RIGHT_F,      kVexMotor393S, kVexMotorReversed,  kVexSensorNone,  0 }
};

static vexDigiCfg dConfig[] = {
    
    // { P_DRIVE_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1, kVexQuadEncoder_1 },
    // { P_DRIVE_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2, kVexQuadEncoder_1 },
    
    // { P_DRIVE_ENC_LEFT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_3 },
    // { P_DRIVE_ENC_LEFT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },
    
    // { P_LIFT_LEFT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_4 },
    // { P_LIFT_LEFT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_4 },
    
    // { P_LIFT_RIGHT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_2 },
    // { P_LIFT_RIGHT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_2 },
    
    // { P_DUMP_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_5 },
    // { P_DUMP_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_5 }
 };

//-------------------Setup----------------------------------------------------//
void vexUserSetup()
{
    //vexDigitalPinSet(kVexDigital_5, 0);
    vexMotorConfigure  ( mConfig,   MOT_CONFIG_SIZE( mConfig ) )  ;
    vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );

}

void vexUserInit()
{
	// Initialize PID
	leftDrivePid  =  EPidInit(kMinJerk, 0.01, 0, 0.01, S_DRIVE_ENC_LEFT,  false);
	rightDrivePid =  EPidInit(kMinJerk, 0.01, 0, 0.01, S_DRIVE_ENC_RIGHT,  true);
	
	
	//  lfol = LineFollowerInit(
	// 5,                   // five sensors
	// S_LINE_FOLLOWER_L2,  // starting from the leftmost
	// 30,                 // max speed
	// lfolThresholds,
	// lfolDrives
	// );
	// lfol->log = false;
}

//-------------Miscellaneous functions----------------------------------------//

bool driveMotors(void) {
	short ld, rd ;
	//Calculate Motor Power
	int forward = VALLEY(vexControllerGet(J_DRIVE), 20, 127) * 0.8;
	int turn;
	
	
	int turnChannel = vexControllerGet(J_TURN) * 0.6;
	
	turn = VALLEY(turnChannel, 20, 127);
	ld = VALLEY(forward + turn, 20, 127);
	rd = VALLEY(forward - turn, 20, 127);
	//
	vexMotorSet(M_DRIVE_LEFT_B,  ld);
	vexMotorSet(M_DRIVE_RIGHT_B, rd);
	vexMotorSet(M_DRIVE_LEFT_F,  ld);
	vexMotorSet(M_DRIVE_RIGHT_F, rd);
	
	
	return (ld != 0 || rd != 0);
}



void Set_Follower_Motors(int val){
	vexMotorSet(M_DUMPER_TOP, val);
	vexMotorSet(M_DUMPER_BOTTOM, val);
}


void Set_Lift_Motors(int val){
	vexMotorSet(M_LIFT_LEFT, val);
	vexMotorSet(M_LIFT_RIGHT, val);
}


//---------------------Autonomous Functions ----------------------------------//

/**
 drive_forward(double ratio);
 param:
 @ratio is the ditance you want to move. One block take a second to move
 */
void drive_forward(double ratio){
	systime_t init_time = chTimeNow();
	
	
	systime_t duration = abs(ratio*800);
	int32_t target = 850*ratio;
	EPidEnable(rightDrivePid,duration, target);
	EPidEnable(leftDrivePid,duration, target);
	systime_t end_time = chTimeNow() + duration;
	
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
	
}

void turn_deg(double ratio){
	systime_t init_time = chTimeNow();
	
	
	systime_t duration = abs(ratio*1900);
	int32_t target = 2300*ratio;
	EPidEnable(rightDrivePid,duration, -target);
	EPidEnable(leftDrivePid,duration, target);
	systime_t end_time = chTimeNow() + duration;
	
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

// task lift_up(){
//   systime_t init_time = chTimeNow();
//   systime_t duration = 1200;
//   EPidEnable(rightLiftPid,duration, 2000);
//   EPidEnable(leftLiftPid,duration, 2000);
//   while (true){
//       if(vexControllerGet(Btn7R)){
//         break;
//       }

//       systime_t autonTime = chTimeNow() - init_time;
//       if (autonTime < duration){
//          vexMotorSet(M_LIFT_LEFT, EPidUpdate(leftLiftPid));
//          vexMotorSet(M_LIFT_RIGHT, EPidUpdate(rightLiftPid));
//       }
//       else{
//         vexMotorSet(M_LIFT_LEFT, 0);
//         vexMotorSet(M_LIFT_RIGHT, 0);
//         break;
//       }
//   }

// }

task lift_up(){
	systime_t init_time = chTimeNow();
	
	// StartTask(lift_up_1);
	systime_t duration = abs(800);
	
	
	
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			break;
		}
		
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			// if (vexSensorValueGet(S_LIFT_LEFT) < 550) {
			Set_Lift_Motors(100);
			// }
		}
		else{
			Set_Lift_Motors(0);
			break;
		}
	}
	
}

task lift_up_STOP(){
	systime_t init_time = chTimeNow();
	
	// StartTask(lift_up_1);
	systime_t duration = abs(1100);
	
	
	
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			break;
		}
		
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			// if (vexSensorValueGet(S_LIFT_LEFT) < 550) {
			Set_Lift_Motors(100);
			// }
		}
		else{
			Set_Lift_Motors(10);
			break;
		}
	}
	
	init_time = chTimeNow();
	duration = abs(2900);
	
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			break;
		}
		
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			// if (vexSensorValueGet(S_LIFT_LEFT) < 550) {
			Set_Lift_Motors(10);
			// }
		}
		else{
			Set_Lift_Motors(-10);
			break;
		}
	}
	
	
	
}

task lift_down(){
	systime_t init_time = chTimeNow();
	
	// StartTask(lift_up_1);
	systime_t duration = abs(1500);
	
	
	
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			break;
		}
		
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			if (vexSensorValueGet(S_DUMP) < 630)
				Set_Lift_Motors(-100);
		}
		else{
			Set_Lift_Motors(0);
			break;
		}
	}
	
}

void lift_down_1(){
	systime_t init_time = chTimeNow();
	
	
	systime_t duration = abs(700);
	
	
	
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			break;
		}
		
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			Set_Lift_Motors(-100);
		}
		else{
			break;
		}
	}
	
}

void lift_up_2(){
	systime_t init_time = chTimeNow();
	systime_t duration = abs(1000);
	
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			break;
		}
		
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			Set_Lift_Motors(100);
		}
		else{
			break;
		}
	}
	
}


void lift_down_2(){
	systime_t init_time = chTimeNow();
	systime_t duration = abs(1000);
	
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			break;
		}
		
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			Set_Lift_Motors(-100);
		}
		else{
			break;
		}
	}
	
}

task dumper_retract (){
	systime_t init_time = chTimeNow();
	systime_t duration = abs(1200);
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			break;
		}
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration && vexSensorValueGet(S_DUMP) < 1000){
			Set_Follower_Motors(-90);
		}
		else{
			Set_Follower_Motors(0);
			break;
		}
	}
	Set_Follower_Motors(0);
}

task dumper_down (){
	systime_t init_time = chTimeNow();
	systime_t duration = abs(1000);
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			Set_Follower_Motors(0);
			break;
		}
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			if (vexSensorValueGet(S_DUMP) >0)
				Set_Follower_Motors(60);
			else{
				Set_Follower_Motors(0);
			}
		}
		else{
			Set_Follower_Motors(0);
			break;
		}
	}
}



void dumper_deploy (){
	systime_t init_time = chTimeNow();
	systime_t duration = abs(700);
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			break;
		}
		
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration && vexSensorValueGet(S_DUMP)>0) {
			Set_Follower_Motors(100);
		}
		else{
			break;
		}
	}
}

task dumper_reset_zero (){
	systime_t init_time = chTimeNow();
	systime_t duration = abs(1000);
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			break;
		}
		
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration){
			Set_Follower_Motors(40);
			vexSensorValueSet(S_DUMP, 0);
		}
		else{
			Set_Follower_Motors(0);
			break;
		}
	}
}
void stop_slight_lift(){
	StopTask(dumper_retract);
	Set_Follower_Motors(-10);
}

void slight_dumper_lift(){
	systime_t init_time = chTimeNow();
	systime_t duration = abs(800);
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			break;
		}
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration && vexSensorValueGet(S_DUMP) < 640){
			Set_Follower_Motors(-90);
		}
		else{
			Set_Follower_Motors(-10);
			break;
		}
	}
}

bool isAllLineOn(void) {
	int i = 0;
	for(i = 0;i < 5;i++) {
		if(vexAdcGet(i) >= 400) {
			return false;
		}
	}
	return true;
}



void dumper_to_zero(){
	systime_t init_time = chTimeNow();
	systime_t duration = abs(800);
	while (!chThdShouldTerminate()){
		if(vexControllerGet(Btn7R)){
			break;
		}
		systime_t autonTime = chTimeNow() - init_time;
		if (autonTime < duration && vexSensorValueGet(S_DUMP) > 0){
			Set_Follower_Motors(90);
		}
		else{
			Set_Follower_Motors(0);
			break;
		}
	}
	Set_Follower_Motors(0);
}
void turn_on_line(){
	
	// drive_forward(-0.3);
	
	while (!isAllLineOn()){
		if(vexControllerGet(Btn7R)){
			break;
		}
		vexMotorSet(M_DRIVE_LEFT_B,  -30);
		vexMotorSet(M_DRIVE_RIGHT_B, -30);
		vexMotorSet(M_DRIVE_LEFT_F,  -30);
		vexMotorSet(M_DRIVE_RIGHT_F, -30);
		vexSleep(10);
	}
	vexMotorSet(M_DRIVE_LEFT_B,  0);
	vexMotorSet(M_DRIVE_RIGHT_B, 0);
	vexMotorSet(M_DRIVE_LEFT_F,  0);
	vexMotorSet(M_DRIVE_RIGHT_F, 0);
	
}


void stop_dumper(){
	StopTask(dumper_retract);
	Set_Follower_Motors(0);
	vexMotorSet(M_DRIVE_LEFT_B,  -50);
	vexMotorSet(M_DRIVE_RIGHT_B, -50);
	vexMotorSet(M_DRIVE_LEFT_F,  -50);
	vexMotorSet(M_DRIVE_RIGHT_F, -50);
	
}

void drive_forward_into_cube(){
	systime_t init_time = chTimeNow();
	
	
	systime_t duration = abs(1300);
	int32_t target = 1300;
	systime_t end_time = chTimeNow() + duration;
	
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
			int16_t ld = 127;
			int16_t rd = 127;
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
	
}
//---------------------Autonomous routine-------------------------------------//

msg_t vexAutonomous( void *arg )
{
	(void)arg;
	vexTaskRegister("auton");
	/*
	 // First Function.
	 
	 StartTask(dumper_reset_zero);
	 drive_forward(-0.7);
	 turn_on_line();
	 drive_forward(0.1);
	 wait(0.75);
	 turn_deg(0.277);
	 wait(1);
	 drive_forward_into_cube() ;
	 wait(1);
	 drive_forward(2.2);
	 
	 // Throwing the Dumper
	 slight_dumper_lift();
	 wait(0.25);
	 
	 wait(0.25);
	 turn_deg(-0.25);
	 turn_deg(0.02);
	 wait(1);
	 StartTask(dumper_retract);
	 drive_forward(-0.9);
	 wait(1);
	 
	 // Going for the Middle Stars on the Fence.
	 dumper_to_zero();
	 drive_forward(0.2);
	 StartTask(lift_up);
	 wait(1);
	 
	 // Going towards the Middle Stars.
	 drive_forward(1.2);
	 wait(1);
	 slight_dumper_lift();
	 wait(1);
	 
	 // Picking up with Middle Stars.
	 drive_forward(-1);
	 drive_forward(-0.3);
	 dumper_retract();
	 wait(1);
	 dumper_to_zero();
	 
	 // Any remaining Middle Stars.
	 drive_forward(0.5);
	 // wait(1);
	 // wait(0.5);
	 // slight_dumper_lift();
	 // wait(0.5);
	 // turn_deg(0.23);
	 // wait(1);
	 
	 // // Going for the Left Stars on the Top.
	 // drive_forward(1.65);
	 // wait(0.5);
	 // turn_deg(-0.35);
	 // drive_forward(-0.4);
	 // lift_up();
	 
	 // // Going for the Corner Star
	 // wait(0.5);
	 // turn_deg(0.1);
	 // dumper_to_zero();
	 // drive_forward(1.25);
	 // wait(0.5);
	 // slight_dumper_lift();
	 
	 // Going for the last row Stars.
	 drive_forward(0.3);
	 turn_deg(-0.25);
	 dumper_to_zero();
	 drive_forward(4.3);
	 slight_dumper_lift();
	 turn_deg(0.25);
	 drive_forward(-2);
	 
	 wait(1);
	 turn_deg(-0.1);
	 drive_forward(-0.15);
	 lift_up();
	 
	 
	 */
	
	// SECOND FUNTION BEGINS.
	
	// For the Cube Allignment
	StartTask(dumper_reset_zero);
	drive_forward(-0.7);
	turn_on_line();
	drive_forward(0.1);
	wait(0.75);
	turn_deg(0.277);
	wait(1);
	drive_forward_into_cube() ;
	wait(1);
	
	// Throwing the Dumper
	slight_dumper_lift();
	wait(0.25);
	turn_deg(-0.27);
	turn_deg(0.02);
	wait(1);
	drive_forward(-0.65);
	// lift_up_STOP();
	// lift_down();
	wait(0.5);
	StartTask(dumper_retract);
	drive_forward(-0.55);
	wait(1);
	
	// Going for the Middle Stars on the Fence.
	dumper_to_zero();
	drive_forward(0.15);
	StartTask(lift_up);
	wait(1);
	
	// Going towards the Middle Stars.
	drive_forward(1.05);
	wait(1);
	slight_dumper_lift();
	wait(1);
	
	// Picking up with Middle Stars.
	drive_forward(-1.25);
	wait(0.5);
	dumper_retract();
	wait(1);
	dumper_to_zero();
	
	// Any remaining Middle Stars.
	drive_forward(0.5);
	wait(1);
	wait(0.5);
	slight_dumper_lift();
	wait(0.5);
	turn_deg(0.23);
	wait(1);
	
	// Going for the Left Stars on the Top.
	drive_forward(1.65);
	wait(0.5);
	turn_deg(-0.25);
	drive_forward(-0.2);
	lift_up();
	
	// // Going for the Corner Star
	wait(0.5);
	// drive_forward(1.65);
	// wait(0.5);
	// turn_deg(-0.35);
	// wait(1);
	// drive_forward(-0.6);
	// lift_up();
	// // turn_deg(0.);
	// dumper_to_zero();
	// drive_forward(1.18);
	// wait(0.5);
	// slight_dumper_lift();
	
	// // Going for the last row Stars.
	// drive_forward(0.3);
	// turn_deg(-0.35);
	// dumper_to_zero();
	// drive_forward(3);
	// slight_dumper_lift();
	// turn_deg(0.25);
	// drive_forward(-2);
	
	// wait(1);
	// turn_deg(-0.1);
	// drive_forward(-0.15);
	// lift_up();
	
	
	vexSleep(10);
	// }
	
	return (msg_t)0;
}

//---------------------User control settings----------------------------------//

msg_t vexOperator( void *arg )
{
	(void)arg;
	vexTaskRegister("operator");
	int16_t left_lift_val = -580, right_lift_val = 580;
	
	bool keepLiftinPlace = false, keepDumpInPlace = false;
	
	//Clear encoders
	// clearDriveEncoders();
	
	while(!chThdShouldTerminate())
	{
		//Remember to Uncomment.
		driveMotors();
		
		if(vexControllerGet(Btn7L)){
			vexAutonomous(NULL);
		}
		
		
		//640 ticks
		// Lift Functions.
		
		if(vexControllerGet(J_LIFT_UP)){
			Set_Lift_Motors(100);
		}
		else if(vexControllerGet(J_LIFT_DOWN)){
			Set_Lift_Motors(-100);
		} else if (vexControllerGet(Btn8D)){
			Set_Lift_Motors(10);
		} else{
			Set_Lift_Motors(0);
		}
		
		// if ((!vexControllerGet(J_LIFT_UP) && !vexControllerGet(J_LIFT_DOWN)) && vexControllerGet(Btn7D)){
		//   Set_Lift_Motors(10);
		// }
		
		
		if(vexControllerGet(J_DUMPER_DOWN) && vexSensorValueGet(S_DUMP) > 0){
			keepDumpInPlace = false;
			Set_Follower_Motors(50);
		}
		else if(vexControllerGet(J_DUMPER_UP)){
			if (vexSensorValueGet(S_DUMP) < 630)
				keepDumpInPlace = true;
			Set_Follower_Motors(-80);
		} else if  (keepDumpInPlace && !vexControllerGet(J_DUMPER_UP)){
			Set_Follower_Motors(-10);
		}
		else{
			Set_Follower_Motors(0);
		}
		
		
		
		
		
		if(vexControllerGet(J_DUMPER_DOWN) && vexControllerGet(Btn8L) ){
			Set_Follower_Motors(65);
			vexSensorValueSet(S_DUMP,0);
		}
		
		// if(vexControllerGet(J_SOLENOID_RELEASE)) {
		//   vexDigitalPinSet(P_SOLENOID_1, 0);
		//   vexDigitalPinSet(P_SOLENOID_2, 0);
		// }
		// else  if(vexControllerGet(J_SOLENOID_RETRACT)) {
		//   vexDigitalPinSet(P_SOLENOID_1, 1);
		//   vexDigitalPinSet(P_SOLENOID_2, 1);
		// }
		
		
		//Don't hog cpu
		vexSleep(10);
	}
	return (msg_t)0;
}
