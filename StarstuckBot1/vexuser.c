#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"
#include "../Common/easing.h"

// Motor mappings

// Lift
#define M_LIFT_LEFT_A   kVexMotor_6
#define M_LIFT_LEFT_B   kVexMotor_7

#define M_LIFT_RIGHT_A  kVexMotor_4
#define M_LIFT_RIGHT_B  kVexMotor_5


// Drive
#define M_DRIVE_RIGHT_A  kVexMotor_2
#define M_DRIVE_RIGHT_B  kVexMotor_3
#define M_DRIVE_LEFT_A   kVexMotor_8
#define M_DRIVE_LEFT_B   kVexMotor_9


// Sensor mappings
#define P_LIFT_ENC_LEFT_A   kVexDigital_1
#define P_LIFT_ENC_LEFT_B   kVexDigital_2
#define P_LIFT_ENC_RIGHT_A  kVexDigital_3
#define P_LIFT_ENC_RIGHT_B  kVexDigital_4

#define P_DRIVE_ENC_RIGHT_A kVexDigital_5
#define P_DRIVE_ENC_RIGHT_B kVexDigital_6
#define P_DRIVE_ENC_LEFT_A  kVexDigital_7
#define P_DRIVE_ENC_LEFT_B  kVexDigital_8

#define P_TOSS_PNU          kVexDigital_9

#define S_LIFT_ENC_LEFT   kVexSensorDigital_1
#define S_LIFT_ENC_RIGHT  kVexSensorDigital_3
#define S_DRIVE_ENC_LEFT  kVexSensorDigital_5
#define S_DRIVE_ENC_RIGHT kVexSensorDigital_7

// Controller mappings
#define J_LIFT_UP     Btn6U
#define J_LIFT_DOWN   Btn6D
#define J_DRIVE       Ch3
#define J_TURN        Ch1
#define J_TOSS_UP     Btn5U
#define J_TOSS_DOWN   Btn5D
#define J_AUTON_CONT  Btn7L

// PID Controls
EPidController *leftDrivePid;
EPidController *rightDrivePid;
EPidController *rightLiftPid;
EPidController *leftLiftPid;


//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {
  { M_LIFT_LEFT_A,    kVexMotor393S, kVexMotorNormal,      kVexSensorNone,  0 },
  { M_LIFT_LEFT_B,    kVexMotor393S, kVexMotorNormal,      kVexSensorNone,  0 },
  { M_LIFT_RIGHT_A,   kVexMotor393S, kVexMotorReversed,    kVexSensorNone,  0 },
  { M_LIFT_RIGHT_B,   kVexMotor393S, kVexMotorReversed,    kVexSensorNone,  0 },
  { M_CLAW,           kVexMotor393S, kVexMotorReversed,    kVexSensorNone,  0 },
  { M_DRIVE_RIGHT,    kVexMotor393S, kVexMotorReversed,    kVexSensorNone,  0 },
  { M_DRIVE_LEFT,     kVexMotor393S, kVexMotorReversed,    kVexSensorNone,  0 }
};

static vexDigiCfg dConfig[] = {

  { P_DRIVE_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1, kVexQuadEncoder_2 },
  { P_DRIVE_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2, kVexQuadEncoder_2 },

  { P_DRIVE_ENC_LEFT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_3 },
  { P_DRIVE_ENC_LEFT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },

  { P_LIFT_ENC_LEFT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_4 },
  { P_LIFT_ENC_LEFT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_4 },

  { P_LIFT_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_5 },
  { P_LIFT_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_5 }

};

//-------------------Setup----------------------------------------------------//
void vexUserSetup()
{
    //vexDigitalPinSet(kVexDigital_5, 0);
    vexMotorConfigure( mConfig, MOT_CONFIG_SIZE( mConfig ) )  ;
    vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
}

void vexUserInit()
{
    // Initialize PID
    leftDrivePid = EPidInit(kMinJerk, 0.01, 0, 0.01, S_DRIVE_ENC_LEFT, false);
    rightDrivePid = EPidInit(kMinJerk  , 0.01, 0, 0.01, S_DRIVE_ENC_RIGHT, true);
    rightLiftPid = EPidInit(kMinJerk, 0.001, 0, 0.01, S_LIFT_ENC_RIGHT, false);
    leftLiftPid = EPidInit (kMinJerk, 0.001, 0, 0.01, S_LIFT_ENC_LEFT, false);
}

//-------------Miscellaneous functions----------------------------------------//

bool driveMotors(void) {
  short ld, rd ;
  //Calculate Motor Power
  int forward = VALLEY(vexControllerGet(J_DRIVE), 20, 127);
  int turn;


  int turnChannel = vexControllerGet(J_TURN) * 0.6;

  turn = VALLEY(turnChannel, 20, 127);
  ld = VALLEY(forward + turn, 20, 127);
  rd = VALLEY(forward - turn, 20, 127);

  vexMotorSet(M_DRIVE_LEFT,  ld);
  vexMotorSet(M_DRIVE_RIGHT, rd);

  return (ld != 0 || rd != 0);
}

// Raises lift, adjusting motor output to rise smoothly
void raiseLift(void){

  //Start with a base value of 100
  short ld =100;
  short rd = 100;

  //Get lift encoder values
  int32_t right_enc_value = abs(vexSensorValueGet(S_LIFT_ENC_RIGHT));
  int32_t left_enc_value = abs(vexSensorValueGet(S_LIFT_ENC_LEFT));

  int32_t diff = right_enc_value - left_enc_value;
  if (diff > 2){
    rd = rd - 3*diff;
  }
  if (diff < -2) {
    ld = ld - 3*diff;
  }
  if (right_enc_value > 330){
    rd = 0;
  }
  if (left_enc_value > 330){
    ld = 0;
  }

  vexMotorSet(M_LIFT_RIGHT_A, rd);
  vexMotorSet(M_LIFT_RIGHT_B, rd);
  vexMotorSet(M_LIFT_LEFT_A, ld);
  vexMotorSet(M_LIFT_LEFT_B, ld);
}

void lowerLift(void){

  //Start with a base value of -52 as we are going down.
  short ld = -52;
  short rd = -52;

  //Get lift encoder values
  int32_t right_enc_value = abs(vexSensorValueGet(S_LIFT_ENC_RIGHT));
  int32_t left_enc_value = abs(vexSensorValueGet(S_LIFT_ENC_LEFT));

  int32_t diff = right_enc_value - left_enc_value;
  if (diff > 5){
    ld = 0;
  }
  if (diff < -5) {
    rd = 0;
  }
  vexMotorSet(M_LIFT_RIGHT_A, rd);
  vexMotorSet(M_LIFT_RIGHT_B, rd);
  vexMotorSet(M_LIFT_LEFT_A, ld);
  vexMotorSet(M_LIFT_LEFT_B, ld);
}

void clearDriveEncoders(void) {
    vexSensorValueSet(S_DRIVE_ENC_LEFT, 0);
    vexSensorValueSet(S_DRIVE_ENC_RIGHT, 0);
}
/*--------------------------Auton Routine Functions---------------------------*/

void Move_Forward (int target) {
  const int SPEED = 350;
  int32_t duration = (ABS(target)/SPEED)*1000;
  EPidEnable(rightDrivePid, duration, target);
  EPidEnable(leftDrivePid, duration, target);
}

void Move_in_Dir (int target, int dir) {
  const int SPEED = 250;
  int32_t duration = (ABS(target)/SPEED)*1000;
  EPidEnable(rightDrivePid, duration, -dir*target);
  EPidEnable(leftDrivePid, duration, dir*target);
}

//---------------------Autonomous routine-------------------------------------//

msg_t vexAutonomous( void *arg )
{
    (void)arg;
    vexTaskRegister("auton");

    //Reset encoders
    clearDriveEncoders();
    // vexSensorValueSet(S_ENC_DRIVE_LEFT,0);
    // vexSensorValueSet(S_ENC_DRIVE_RIGHT,0);
    systime_t init_time = chTimeNow();
    int step = 0;
    while(!chThdShouldTerminate())
    {
      systime_t autonTime = chTimeNow() - init_time;

      //Deploy claw

      //ROUTINE 1
      /*
      if(autonTime < 200 && step == 0){
        vexMotorSet(M_CLAW, -60);
      }
      //Open claw
      else if(autonTime > 200 && autonTime < 450 && step == 0){
        vexMotorSet(M_CLAW, 60);
        step++;
      }
      //Turn counterclockwise
      else if (autonTime > 450 && autonTime < 1450 && step==1){
        vexMotorSet(M_CLAW, 0);
        EPidEnable(leftDrivePid, 900, -460);
        EPidEnable(rightDrivePid, 900, 460);
        step++;
      }
      //Drive forward
      else if (autonTime > 1450  && autonTime < 6500 && step ==2){
        EPidEnable(leftDrivePid, 4800, 2700);
        EPidEnable(rightDrivePid,4800, 2700);
        step++;
      }
      //Close claw
      else if (autonTime > 6500 && autonTime<6750 && step ==  3){
        vexMotorSet(M_CLAW, -60);
      }
      //Turn Clockwise
      else if (autonTime > 6750 && autonTime< 8200 && step == 4){
        vexMotorSet(M_CLAW, 0);
        EPidEnable(leftDrivePid, 1350, 450);
        EPidEnable(rightDrivePid, 1350, -450);
        step++;
      //Drive forward
    } else if (autonTime > 8200 && autonTime < 12000 && step == 5){

        EPidEnable(leftDrivePid, 2500, 700);
        EPidEnable(rightDrivePid, 2500, 700);
      }


      int16_t motorValL = EPidUpdate(leftDrivePid);
		  int16_t motorValR = EPidUpdate(rightDrivePid);
      int16_t leftLiftVal = EPidUpdate(leftLiftPid);
      int16_t rightLiftVal = EPidUpdate(rightLiftPid);

		  // vex_printf ("%d       %d", motorValL, motorValR);

		  vexMotorSet(M_DRIVE_RIGHT, motorValR);
		  vexMotorSet(M_DRIVE_LEFT, motorValL);

      vexMotorSet(M_LIFT_LEFT_A, leftLiftVal);
      vexMotorSet(M_LIFT_LEFT_B, leftLiftVal);

      vexMotorSet(M_LIFT_RIGHT_A, rightLiftVal);
      vexMotorSet(M_LIFT_RIGHT_B, rightLiftVal);
      */

      //ROUTINE 2

      if(autonTime > 350 && autonTime < 1650){
        vexMotorSet(M_DRIVE_LEFT, 80);
        vexMotorSet(M_DRIVE_RIGHT, 80);
      }
      if(autonTime > 300 && autonTime < 2000){
        raiseLift();
      }
      if(autonTime < 600){
        vexMotorSet(M_CLAW, -90);
      }
      if(autonTime > 600 && autonTime < 750){
        vexMotorSet(M_CLAW, 60);
      }
      if (autonTime > 750){
        vexMotorSet(M_CLAW, 0);
      }
      if (autonTime > 1650 && autonTime < 1850){
        vexMotorSet(M_DRIVE_LEFT, 0);
        vexMotorSet(M_DRIVE_RIGHT, 0);
      }
      //Turn counterclockwise
      if (autonTime > 1850 && autonTime < 2250){
        vexMotorSet(M_DRIVE_LEFT, -90);
        vexMotorSet(M_DRIVE_RIGHT, 90);
      }
      if (autonTime > 2250 && autonTime < 5150){
        vexMotorSet(M_DRIVE_RIGHT, -80);
        vexMotorSet(M_DRIVE_LEFT, -80);
      }
      if (autonTime > 5150){
        vexMotorSet(M_DRIVE_LEFT, 0);
        vexMotorSet(M_DRIVE_RIGHT, 0);
      }

      //Turn calibration
      /*
      8if(autonTime < 2000){
        raiseLift();
      }
      if(autonTime > 2000 && autonTime < 2400){
        vexMotorSet(M_DRIVE_LEFT, -100);
        vexMotorSet(M_DRIVE_RIGHT, 100);
      }
      if(autonTime > 2400){
        vexMotorSet(M_DRIVE_RIGHT, 0);
        vexMotorSet(M_DRIVE_LEFT, 0);
      }
      */
      vexSleep(10);
    }

    return (msg_t)0;
}



//---------------------User control settings----------------------------------//

msg_t vexOperator( void *arg )
{
    (void)arg;
    vexTaskRegister("operator");

    bool isMoving;
    // systime_t encoderStopTime = chTimeNow();
    // int32_t curr_drive_enc_val_right = 0;
    // int32_t curr_drive_enc_val_left = 0;
    // int32_t last_drive_enc_val_right = 0;
    // int32_t last_drive_enc_val_left = 0;

    //Clear encoders
    clearDriveEncoders();

    while(!chThdShouldTerminate())
    {
      isMoving = driveMotors();

      //Prevent motors from burning out if stuck
      /*
      curr_drive_enc_val_right = vexSensorValueGet(S_DRIVE_ENC_RIGHT);
      curr_drive_enc_val_left = vexSensorValueGet(S_DRIVE_ENC_LEFT);
      if (isMoving && (curr_drive_enc_val_left == last_drive_enc_val_left || curr_drive_enc_val_right == last_drive_enc_val_right)){
        encoderStopTime = chTimeNow();
      }
      if(!isMoving){
        encoderStopTime = chTimeNow();
      }
      if (chTimeNow() - encoderStopTime > 250) {
        vexMotorSet(M_DRIVE_LEFT1,  0);
        vexMotorSet(M_DRIVE_LEFT2,  0);
        vexMotorSet(M_DRIVE_RIGHT1, 0);
        vexMotorSet(M_DRIVE_RIGHT2, 0);
        vexSleep(3000);
        encoderStopTime = chTimeNow();
      }
      last_drive_enc_val_left = curr_drive_enc_val_left;
      last_drive_enc_val_right = curr_drive_enc_val_right;
      */

      // Controls for lift LIFT
      if(vexControllerGet(J_LIFT_UP)) {
        raiseLift();
      }
      else if(vexControllerGet(J_LIFT_DOWN)) {
        lowerLift();
      }
      else if(vexControllerGet(J_RAISE_LEFT)){
        vexMotorSet(M_LIFT_LEFT_A, 70);
        vexMotorSet(M_LIFT_LEFT_B, 70);
      }
      else if(vexControllerGet(J_LOWER_LEFT)){
        vexMotorSet(M_LIFT_LEFT_A, -25);
        vexMotorSet(M_LIFT_LEFT_B, -25);
      }
      else if(vexControllerGet(J_RAISE_RIGHT)){
        vexMotorSet(M_LIFT_RIGHT_A, 70);
        vexMotorSet(M_LIFT_RIGHT_B, 70);
      }

      else if(vexControllerGet(J_LOWER_RIGHT)){
        vexMotorSet(M_LIFT_RIGHT_A, -25);
        vexMotorSet(M_LIFT_RIGHT_B, -25);
      }
      else {
        vexMotorSet(M_LIFT_RIGHT_A, 0);
        vexMotorSet(M_LIFT_RIGHT_B, 0);
        vexMotorSet(M_LIFT_LEFT_A, 0);
        vexMotorSet(M_LIFT_LEFT_B, 0);
      }

      //Controls for claw
      if(vexControllerGet(J_CLAW_OPEN)) {
        vexMotorSet(M_CLAW, 60);
      }

      else if(vexControllerGet(J_CLAW_CLOSE)) {
        vexMotorSet(M_CLAW, -60);
      }
      else {
        vexMotorSet(M_CLAW, 0);
      }

      //Don't hog cpu
      vexSleep(10);
    }
    return (msg_t)0;
}
