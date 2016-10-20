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
#define J_LIFT_UP    Btn6U
#define J_LIFT_DOWN  Btn6D
#define J_DRIVE      Ch2
#define J_TURN       Ch4
#define J_CLAW_OPEN  Btn5D
#define J_CLAW_CLOSE Btn5U


// PID Controls
EPidController *leftDrivePid;
EPidController *rightDrivePid;


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
    // //Initialize TBHControllers
    // flyWheelCtrl = TBHControllerInit(S_ENC_FLY, 0.01, 11000, false);
    // flyWheelCtrl->powerqZeroClamp = true;
    // flyWheelCtrl->log = false;

    // Initialize PID
    leftDrivePid = EPidInit(kFlat, 0.001, 0, 0.01, S_ENC_DRIVE_LEFT, true);
    rightDrivePid = EPidInit(kFlat, 0.001, 0, 0.01, S_ENC_DRIVE_RIGHT, true);
    
}

//-------------Miscellaneous functions----------------------------------------//

bool driveMotors(void) {
  short ld, rd ;
  //Calculate Motor Power
  int forward = VALLEY(vexControllerGet(J_DRIVE), 25, 127);
  int turn;

  turn = VALLEY(vexControllerGet(J_TURN), 25, 127);
  ld = VALLEY(forward + turn, 25, 127);
  rd = VALLEY(forward - turn, 25, 127);

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
  if (diff > 5 || right_enc_value > 1100){
    rd = 0;
  }
  if (diff < -5 || left_enc_value > 1100) {
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

  vexMotorSet(M_LIFT_RIGHT, rd);
  vexMotorSet(M_LIFT_LEFT, ld);
}

void clearDriveEncoders(void) {
    vexSensorValueSet(S_DRIVE_ENC_LEFT, 0);
    vexSensorValueSet(S_DRIVE_ENC_RIGHT, 0);
}
/*--------------------------Auton Routine Functions---------------------------*/

void Move_Forward (int taget, int time) {
  const int SPEED = 350;
  int32_t duration = (ABS(target)/SPEED)*1000;
  EPidEnable(rightDrivePid, duration, target);
  EPidEnable(leftDrivePid, duration, target);
}

void Move_in_Dir (int taget, int time, int dir) {
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

    while(!chThdShouldTerminate())
    {
      systime_t autonTime = chTimeNow();

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

    //Clear encoders
    clearDriveEncoders();

    while(!chThdShouldTerminate())
    {
      isMoving = driveMotors();

      // Controls for lift

      if(vexControllerGet(J_LIFT_UP)) {
        raiseLift();
      }

      else if(vexControllerGet(J_LIFT_DOWN)) {
        lowerLift();
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
