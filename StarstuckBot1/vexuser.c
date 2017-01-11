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
#define M_LIFT_RIGHT       kVexMotor_3
#define M_LIFT_LEFT        kVexMotor_9

// Drive
#define M_DRIVE_RIGHT_B    kVexMotor_2
#define M_DRIVE_LEFT_B     kVexMotor_5
#define M_DRIVE_LEFT_F     kVexMotor_8
#define M_DRIVE_RIGHT_F    kVexMotor_6

//Followers
#define M_FOLLOWERS_RIGHT       kVexMotor_7
#define M_FOLLOWERS_LEFT        kVexMotor_4


// Sensor mappings
// #define P_LIFT_ENC_RIGHT_A   kVexDigital_1
// #define P_LIFT_ENC_RIGHT_B   kVexDigital_2
// #define P_LIFT_ENC_LEFT_A  kVexDigital_3
// #define P_LIFT_ENC_LEFT_B  kVexDigital_4

// #define P_DRIVE_ENC_RIGHT_A kVexDigital_5
// #define P_DRIVE_ENC_RIGHT_B kVexDigital_6
// #define P_DRIVE_ENC_LEFT_A  kVexDigital_7
// #define P_DRIVE_ENC_LEFT_B  kVexDigital_8

// #define P_CLAW_A  kVexDigital_9
// #define P_CLAW_B  kVexDigital_10

// #define S_LIFT_ENC_RIGHT_A  kVexSensorDigital_1
// #define S_LIFT_ENC_LEFT_A   kVexSensorDigital_3
// #define S_DRIVE_ENC_LEFT_B  kVexSensorDigital_5
// #define S_DRIVE_ENC_RIGHT_B kVexSensorDigital_7

// #define S_ClAW          kVexSensorDigital_9

// Controller mappings
#define J_LIFT_UP     Btn5U
#define J_LIFT_DOWN   Btn5D
#define J_DRIVE       Ch3
#define J_TURN        Ch1

#define J_FOLLOWERS_UP   Btn8U
#define J_FOLLOWERS_DOWN   Btn8D

// #define J_CLAW_OPEN   Btn6U
// #define J_CLAW_CLOSE  Btn6D


// PID Controls
// EPidController *leftDrivePid;
// EPidController *rightDrivePid;
// EPidController *rightLiftPid;
// EPidController *leftLiftPid;


//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {
  { M_FOLLOWERS_RIGHT,    kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_FOLLOWERS_LEFT,     kVexMotor393S, kVexMotorReversed,    kVexSensorNone,  0 },
  { M_LIFT_RIGHT,         kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_LIFT_LEFT,          kVexMotor393S, kVexMotorReversed,    kVexSensorNone,  0 },
  { M_DRIVE_RIGHT_B,      kVexMotor393S, kVexMotorReversed,    kVexSensorNone,  0 },
  { M_DRIVE_LEFT_B,       kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_DRIVE_LEFT_F,       kVexMotor393S, kVexMotorReversed,    kVexSensorNone,  0 },
  { M_DRIVE_RIGHT_F,      kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 }
};

static vexDigiCfg dConfig[] = {

  // { P_DRIVE_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1, kVexQuadEncoder_2 },
  // { P_DRIVE_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2, kVexQuadEncoder_2 },

  // { P_DRIVE_ENC_LEFT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_3 },
  // { P_DRIVE_ENC_LEFT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },

  // { P_LIFT_ENC_LEFT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_4 },
  // { P_LIFT_ENC_LEFT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_4 },

  // { P_LIFT_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_5 },
  // { P_LIFT_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_5 }

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
    // // Initialize PID
    // leftDrivePid = EPidInit(kMinJerk, 0.01, 0, 0.01, S_DRIVE_ENC_LEFT, false);
    // rightDrivePid = EPidInit(kMinJerk  , 0.01, 0, 0.01, S_DRIVE_ENC_RIGHT, true);
    // rightLiftPid = EPidInit(kMinJerk, 0.001, 0, 0.01, S_LIFT_ENC_RIGHT, false);
    // leftLiftPid = EPidInit (kMinJerk, 0.001, 0, 0.01, S_LIFT_ENC_LEFT, false);
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

  vexMotorSet(M_DRIVE_RIGHT_B,  ld);
  vexMotorSet(M_DRIVE_RIGHT_F, rd);
  vexMotorSet(M_DRIVE_LEFT_F,  ld);
  vexMotorSet(M_DRIVE_LEFT_B, rd);

  return (ld != 0 || rd != 0);
}

// Raises lift, adjusting motor output to rise smoothly
// void raiseLift(void){

//   //Start with a base value of 100
//   short ld =100;
//   short rd = 100;

//   //Get lift encoder values
//   int32_t right_enc_value = abs(vexSensorValueGet(S_LIFT_ENC_RIGHT_A));
//   int32_t left_enc_value = abs(vexSensorValueGet(S_LIFT_ENC_LEFT_A));

//   int32_t diff = right_enc_value - left_enc_value;
//   if (diff > 2){
//     rd = rd - 3*diff;
//   }
//   if (diff < -2) {
//     ld = ld - 3*diff;
//   }
//   if (right_enc_value > 330){
//     rd = 0;
//   }
//   if (left_enc_value > 330){
//     ld = 0;
//   }

//   vexMotorSet(M_LIFT_RIGHT_A, rd);
//   vexMotorSet(M_LIFT_RIGHT_B, rd);
//   vexMotorSet(M_LIFT_LEFT_A, ld);
//   vexMotorSet(M_LIFT_LEFT_B, ld);
// }

// void lowerLift(void){

//   //Start with a base value of -52 as we are going down.
//   short ld = -52;
//   short rd = -52;

//   //Get lift encoder values
//   int32_t right_enc_value = abs(vexSensorValueGet(S_LIFT_ENC_RIGHT_A));
//   int32_t left_enc_value = abs(vexSensorValueGet(S_LIFT_ENC_LEFT_A));

//   int32_t diff = right_enc_value - left_enc_value;
//   if (diff > 5){
//     ld = 0;
//   }
//   if (diff < -5) {
//     rd = 0;
//   }
//   vexMotorSet(M_LIFT_RIGHT_A, rd);
//   vexMotorSet(M_LIFT_RIGHT_B, rd);
//   vexMotorSet(M_LIFT_LEFT_A, ld);
//   vexMotorSet(M_LIFT_LEFT_B, ld);
// }

// void clearDriveEncoders(void) {
//     vexSensorValueSet(S_DRIVE_ENC_LEFT, 0);
//     vexSensorValueSet(S_DRIVE_ENC_RIGHT, 0);
// }

void drive_forward(){
  
}
/*--------------------------Auton Routine Functions---------------------------*/

//---------------------Autonomous routine-------------------------------------//

msg_t vexAutonomous( void *arg )
{
    (void)arg;
    vexTaskRegister("auton");

    //Reset encoders
    // clearDriveEncoders();
    // vexSensorValueSet(S_ENC_DRIVE_LEFT,0);
    // vexSensorValueSet(S_ENC_DRIVE_RIGHT,0);
    systime_t init_time = chTimeNow();
    int step = 0;
    while(!chThdShouldTerminate())
    {
      systime_t autonTime = chTimeNow() - init_time;

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
    // clearDriveEncoders();

    while(!chThdShouldTerminate())
    {
      //Remember to Uncomment.
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

      if(vexControllerGet(J_LIFT_UP)){
        vexMotorSet(M_LIFT_RIGHT, 30);
        vexMotorSet(M_LIFT_LEFT, 30);
      }
      else if(vexControllerGet(J_LIFT_DOWN)){
        vexMotorSet(M_LIFT_RIGHT, -30);
        vexMotorSet(M_LIFT_LEFT, -30);
      } else{
        vexMotorSet(M_LIFT_RIGHT, 0);
        vexMotorSet(M_LIFT_LEFT, 0);
      }


      if(vexControllerGet(J_FOLLOWERS_UP)){
        vexMotorSet(M_FOLLOWERS_RIGHT, 100);
        vexMotorSet(M_FOLLOWERS_LEFT, 100);
      }
      else if(vexControllerGet(J_FOLLOWERS_DOWN)){
        vexMotorSet(M_FOLLOWERS_RIGHT, -100);
        vexMotorSet(M_FOLLOWERS_LEFT, -100);
      } else{
        vexMotorSet(M_FOLLOWERS_RIGHT, 0);
        vexMotorSet(M_FOLLOWERS_LEFT, 0);
      }
      // else {
      //   vexMotorSet(M_LIFT_RIGHT_A, 0);
      //   vexMotorSet(M_LIFT_RIGHT_B, 0);
      //   vexMotorSet(M_LIFT_LEFT_A, 0);
      //   vexMotorSet(M_LIFT_LEFT_B, 0);
      // }

      // //Controls for claw
      // if(vexControllerGet(J_CLAW_OPEN)) {
      //   vexMotorSet(M_CLAW_A, -60);
      //   vexMotorSet(M_CLAW_B, -60);
      // }

      // else if(vexControllerGet(J_CLAW_CLOSE)) {
      //   vexMotorSet(M_CLAW_A, 60);
      //   vexMotorSet(M_CLAW_B, 60);
      // }
      // else {
      //   vexMotorSet(M_CLAW_A, 0);
      //   vexMotorSet(M_CLAW_B, 0);
      // }

      //Don't hog cpu
      vexSleep(10);
    }
    return (msg_t)0;
}
