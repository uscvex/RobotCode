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
#define M_LIFT_LEFT   kVexMotor_8
#define M_LIFT_RIGHT  kVexMotor_2
#define M_CLAW        kVexMotor_9


// Drive
#define M_DRIVE_RIGHT1  kVexMotor_3
#define M_DRIVE_RIGHT2  kVexMotor_4
#define M_DRIVE_LEFT1   kVexMotor_7
#define M_DRIVE_LEFT2   kVexMotor_6

// Sensor mappings
#define P_LIFT_ENC_LEFT_A   kVexDigital_1
#define P_LIFT_ENC_LEFT_B   kVexDigital_2
#define P_LIFT_ENC_RIGHT_A  kVexDigital_3
#define P_LIFT_ENC_RIGHT_B  kVexDigital_4

#define P_DRIVE_ENC_RIGHT_A kVexDigital_5
#define P_DRIVE_ENC_RIGHT_B kVexDigital_6
#define P_DRIVE_ENC_LEFT_A  kVexDigital_7
#define P_DRIVE_ENC_LEFT_B  kVexDigital_8

#define S_LIFT_ENC_LEFT   kVexSensorDigital_1
#define S_LIFT_ENC_RIGHT  kVexSensorDigital_3
#define S_DRIVE_ENC_LEFT  kVexSensorDigital_5
#define S_DRIVE_ENC_RIGHT kVexSensorDigital_7

// Controller mappings
#define J_LIFT_UP     Btn6U
#define J_LIFT_DOWN   Btn6D
#define J_DRIVE       Ch3
#define J_TURN        Ch1
#define J_CLAW_OPEN   Btn5D
#define J_CLAW_CLOSE  Btn5U
#define J_RAISE_LEFT  Btn7U
#define J_LOWER_LEFT  Btn7D
#define J_RAISE_RIGHT Btn8U
#define J_LOWER_RIGHT Btn8D




//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {
  { M_LIFT_LEFT,     kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_LIFT_RIGHT,    kVexMotor393S, kVexMotorReversed,  kVexSensorNone,  0 },
  { M_DRIVE_RIGHT1,  kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_DRIVE_RIGHT2,  kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_DRIVE_LEFT1,   kVexMotor393S, kVexMotorReversed,  kVexSensorNone,  0 },
  { M_DRIVE_LEFT2,   kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_CLAW,          kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 }
};

static vexDigiCfg dConfig[] = {

  { P_DRIVE_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1, kVexQuadEncoder_2 },
  { P_DRIVE_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2, kVexQuadEncoder_2 },

  { P_DRIVE_ENC_LEFT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_3 },
  { P_DRIVE_ENC_LEFT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },

  { P_LIFT_ENC_LEFT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_4 },
  { P_LIFT_ENC_LEFT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_4 },

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
    /*//Initialize TBHControllers
    flyWheelCtrl = TBHControllerInit(S_ENC_FLY, 0.01, 11000, false);
    flyWheelCtrl->powerZeroClamp = true;
    flyWheelCtrl->log = false;

    // Initialize PID
    leftDrivePid = EPidInit(kFlat, 0.001, 0, 0.01, S_ENC_DRIVE_LEFT, true);
    rightDrivePid = EPidInit(kFlat, 0.001, 0, 0.01, S_ENC_DRIVE_RIGHT, true);
    */
}

//-------------Miscellaneous functions----------------------------------------//


bool driveMotors(void) {
  short ld, rd ;
  //Calculate Motor Power
  int forward = VALLEY(vexControllerGet(J_DRIVE), 20, 127);
  int turn;


  int turnChannel = vexControllerGet(J_TURN) * 0.7;

  turn = VALLEY(turnChannel, 20, 127);
  ld = VALLEY(forward + turn, 20, 127);
  rd = VALLEY(forward - turn, 20, 127);

  vexMotorSet(M_DRIVE_LEFT1,  ld);
  vexMotorSet(M_DRIVE_LEFT2,  ld);
  vexMotorSet(M_DRIVE_RIGHT1, rd);
  vexMotorSet(M_DRIVE_RIGHT2, rd);

  return (ld != 0 || rd != 0);
}

// Raises lift, adjusting motor output to rise smoothly
void raiseLift(void){

  //Start with a base value of 100
  short ld = 100;
  short rd = 100;

  //Get lift encoder values
  int32_t right_enc_value = abs(vexSensorValueGet(S_LIFT_ENC_RIGHT));
  int32_t left_enc_value = abs(vexSensorValueGet(S_LIFT_ENC_LEFT));

  int32_t diff = right_enc_value - left_enc_value;
  if (diff > 5){
    rd = rd - 3*diff;
  }
  if (diff < -5) {
    ld = ld - 3*diff;
  }
  if (right_enc_value > 1100){
    rd = 0;
  }
  if (left_enc_value > 1100){
    ld = 0;
  }
  // vex_printf("right motor power: %d \n", rd);
  // vex_printf("left motor power: %d \n", ld);
  // vex_printf("right encoder: %d \n", right_enc_value);
  // vex_printf("left encoder:  %d \n", left_enc_value);
  // vex_printf("\n");

  vexMotorSet(M_LIFT_RIGHT, rd);
  vexMotorSet(M_LIFT_LEFT, ld);
}

void lowerLift(void){

  //Start with a base value of 100
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

  vexMotorSet(M_LIFT_RIGHT, rd);
  vexMotorSet(M_LIFT_LEFT, ld);
}

void clearDriveEncoders(void) {
    vexSensorValueSet(S_DRIVE_ENC_LEFT, 0);
    vexSensorValueSet(S_DRIVE_ENC_RIGHT, 0);
}


//---------------------Autonomous routine-------------------------------------//

msg_t vexAutonomous( void *arg )
{
    (void)arg;
    vexTaskRegister("auton");

    //Reset encoders
    clearDriveEncoders();

    while(!chThdShouldTerminate())
    {
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
    systime_t encoderStopTime = chTimeNow();
    int32_t curr_drive_enc_val_right = 0;
    int32_t curr_drive_enc_val_left = 0;
    int32_t last_drive_enc_val_right = 0;
    int32_t last_drive_enc_val_left = 0;

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

      // Controls for lift

      if(vexControllerGet(J_LIFT_UP)) {
        raiseLift();
      }
      else if(vexControllerGet(J_LIFT_DOWN)) {
        lowerLift();
      }
      else if(vexControllerGet(J_RAISE_LEFT)){
        vexMotorSet(M_LIFT_LEFT, 70);
      }
      else if(vexControllerGet(J_LOWER_LEFT)){
        vexMotorSet(M_LIFT_LEFT, -25);
      }
      else if(vexControllerGet(J_RAISE_RIGHT)){
        vexMotorSet(M_LIFT_RIGHT, 70);
      }

      else if(vexControllerGet(J_LOWER_RIGHT)){
        vexMotorSet(M_LIFT_RIGHT, -25);
      }
      else {
        vexMotorSet(M_LIFT_RIGHT, 0);
        vexMotorSet(M_LIFT_LEFT, 0);
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


      // vexMotorSet(M_LIFT_RIGHT, -25);
      //Don't hog cpu
      vexSleep(10);
    }
    return (msg_t)0;
}
