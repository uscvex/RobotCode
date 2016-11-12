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
#define M_LIFT_LEFT_AB   kVexMotor_7
#define M_LIFT_RIGHT_AB  kVexMotor_4

// Drive
#define M_DRIVE_RIGHT_AB  kVexMotor_2
#define M_DRIVE_RIGHT_C   kVexMotor_3
#define M_DRIVE_LEFT_AB   kVexMotor_9
#define M_DRIVE_LEFT_C    kVexMotor_8

//Claw
#define M_CLAW_A   kVexMotor_5
#define M_CLAW_B   kVexMotor_6

// Sensor mappings
#define P_LIFT_ENC_LEFT_A   kVexDigital_1
#define P_LIFT_ENC_LEFT_B   kVexDigital_2
#define P_LIFT_ENC_RIGHT_A  kVexDigital_3
#define P_LIFT_ENC_RIGHT_B  kVexDigital_4

#define P_DRIVE_ENC_RIGHT_A kVexDigital_5
#define P_DRIVE_ENC_RIGHT_B kVexDigital_6
#define P_DRIVE_ENC_LEFT_A  kVexDigital_7
#define P_DRIVE_ENC_LEFT_B  kVexDigital_8

#define P_CLAW_ENC_A        kVexDigital_9
#define P_CLAW_ENC_B        kVexDigital_10

#define S_LIFT_ENC_LEFT   kVexSensorDigital_1
#define S_LIFT_ENC_RIGHT  kVexSensorDigital_3
#define S_DRIVE_ENC_LEFT  kVexSensorDigital_5
#define S_DRIVE_ENC_RIGHT kVexSensorDigital_7

#define S_CLAW_ENC      kVexSensorDigital_9

#define SIDE_CHOOSER      kVexAnalog_1

// Controller mappings
#define J_LIFT_UP     Btn5U
#define J_LIFT_DOWN   Btn5D
#define J_DRIVE       Ch3
#define J_TURN        Ch1
#define J_TOSS_UP     Btn5U
#define J_TOSS_DOWN   Btn5D
#define J_CLAW_UP     Btn6U
#define J_CLAW_DOWN   Btn6D
#define J_AUTON_CONT  Btn7L

// PID Controls
EPidController *leftDrivePid;
EPidController *rightDrivePid;
EPidController *rightLiftPid;
EPidController *leftLiftPid;


//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {
  { M_LIFT_LEFT_AB,   kVexMotor393T,  kVexMotorNormal,     kVexSensorNone,  0 },
  { M_LIFT_RIGHT_AB,  kVexMotor393T,  kVexMotorReversed,   kVexSensorNone,  0 },
  { M_DRIVE_RIGHT_AB, kVexMotor393R,  kVexMotorReversed,   kVexSensorNone,  0 },
  { M_DRIVE_RIGHT_C,  kVexMotor393R,  kVexMotorReversed,   kVexSensorNone,  0 },
  { M_DRIVE_LEFT_AB,  kVexMotor393R,  kVexMotorReversed,   kVexSensorNone,  0 },
  { M_DRIVE_LEFT_C,   kVexMotor393R,  kVexMotorNormal,     kVexSensorNone,  0 },
  { M_CLAW_A,         kVexMotor393T,  kVexMotorReversed,   kVexSensorNone,  0 },
  { M_CLAW_B,         kVexMotor393T,  kVexMotorNormal,     kVexSensorNone,  0 }
};

static vexDigiCfg dConfig[] = {

  { P_CLAW_ENC_A,       kVexSensorQuadEncoder,  kVexConfigQuadEnc1,  kVexQuadEncoder_1 },
  { P_CLAW_ENC_B,       kVexSensorQuadEncoder,  kVexConfigQuadEnc2,  kVexQuadEncoder_1 },

  { P_DRIVE_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_2 },
  { P_DRIVE_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_2 },

  { P_DRIVE_ENC_LEFT_A, kVexSensorQuadEncoder,  kVexConfigQuadEnc1,  kVexQuadEncoder_3 },
  { P_DRIVE_ENC_LEFT_B, kVexSensorQuadEncoder,  kVexConfigQuadEnc2,  kVexQuadEncoder_3 },

  { P_LIFT_ENC_LEFT_A,  kVexSensorQuadEncoder,  kVexConfigQuadEnc1,  kVexQuadEncoder_4 },
  { P_LIFT_ENC_LEFT_B,  kVexSensorQuadEncoder,  kVexConfigQuadEnc2,  kVexQuadEncoder_4 },

  { P_LIFT_ENC_RIGHT_A, kVexSensorQuadEncoder,  kVexConfigQuadEnc1,  kVexQuadEncoder_5 },
  { P_LIFT_ENC_RIGHT_B, kVexSensorQuadEncoder,  kVexConfigQuadEnc2,  kVexQuadEncoder_5 }

};

//-------------------Setup----------------------------------------------------//
void vexUserSetup()
{
    //vexDigitalPinSet(kVexDigital_5, 0);
    vexMotorConfigure( mConfig,   MOT_CONFIG_SIZE( mConfig ) )  ;
    vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
}

void vexUserInit()
{
    // Initialize PID
    leftDrivePid  =  EPidInit(kMinJerk, 0.01, 0, 0.01, S_DRIVE_ENC_LEFT,  false);
    rightDrivePid =  EPidInit(kMinJerk, 0.01, 0, 0.01, S_DRIVE_ENC_RIGHT,  true);
    rightLiftPid  =  EPidInit(kMinJerk, 0.001, 0, 0.01, S_LIFT_ENC_RIGHT, false);
    leftLiftPid   =  EPidInit (kMinJerk, 0.001, 0, 0.01, S_LIFT_ENC_LEFT, false);
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

  vexMotorSet(M_DRIVE_LEFT_AB,  ld);
  vexMotorSet(M_DRIVE_LEFT_C,   ld);
  vexMotorSet(M_DRIVE_RIGHT_AB, rd);
  vexMotorSet(M_DRIVE_RIGHT_C,  rd);

  return (ld != 0 || rd != 0);
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

void liftClaw(){
  while(S_CLAW_ENC > -45){
    SetMotor(M_CLAW_A, 100);
    SetMotor(M_CLAW_B, 100);
  }
  SetMotor(M_CLAW_A, 0);
  SetMotor(M_CLAW_B, 0);
}

void dropClaw(){
  while(S_CLAW_ENC ){}
}

void dump(){

}

void lowerLift(){

}

//---------------------Autonomous routine-------------------------------------//

msg_t vexAutonomous( void *arg )
{
    (void)arg;
    vexTaskRegister("auton");

    //Reset encoders
    //clearDriveEncoders();

    systime_t init_time = chTimeNow();
    int step = 0;

    //int startOnLeft = -1 * (vexAdcGet(SIDE_CHOOSER) < 2048) ? 1 : 0;

    while(!chThdShouldTerminate())
    {
      if(vexControllerGet(Btn7U)){
        break;
      }
      systime_t autonTime = chTimeNow() - init_time;

      if (step == 0 && autonTime > 0 && autonTime < 2400){
        EPidEnable(rightDrivePid, 1500, 1300);
        EPidEnable(leftDrivePid, 1500, 1300);
        step++;
      }

      //Set motor values
      int16_t motorValL = EPidUpdate(leftDrivePid);
		  int16_t motorValR = EPidUpdate(rightDrivePid);

      int16_t leftLiftVal = EPidUpdate(leftLiftPid);
      int16_t rightLiftVal = EPidUpdate(rightLiftPid);

		  vexMotorSet(M_DRIVE_RIGHT_AB, motorValR);
      vexMotorSet(M_DRIVE_RIGHT_C, motorValR);
		  vexMotorSet(M_DRIVE_LEFT_AB, motorValL);
      vexMotorSet(M_DRIVE_LEFT_C, motorValL);

      vexMotorSet(M_LIFT_LEFT_AB, leftLiftVal);
      vexMotorSet(M_LIFT_RIGHT_AB, rightLiftVal);

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

      if(vexControllerGet(Btn7L)){
        vexAutonomous(NULL);
      }
      // Controls for lift LIFT
      if(vexControllerGet(J_LIFT_UP)) {
        vexMotorSet(M_LIFT_RIGHT_AB, 90);
        vexMotorSet(M_LIFT_LEFT_AB, 90);
      }
      else if(vexControllerGet(J_LIFT_DOWN) && vexSensorValueGet(S_LIFT_ENC_LEFT) < 0
              && vexSensorValueGet(S_LIFT_ENC_RIGHT) < 0) {
        vexMotorSet(M_LIFT_RIGHT_AB, -60);
        vexMotorSet(M_LIFT_LEFT_AB, -60);
      }
      else {
        vexMotorSet(M_LIFT_RIGHT_AB, 0);
        vexMotorSet(M_LIFT_LEFT_AB, 0);
      }
      //Claw controls
      if(vexControllerGet(J_CLAW_UP)){
        vexMotorSet(M_CLAW_A, 100);
        vexMotorSet(M_CLAW_B, 100);
      } else if (vexControllerGet(J_CLAW_DOWN)) {
        vexMotorSet(M_CLAW_A, -60);
        vexMotorSet(M_CLAW_B, -60);
      } else {
        vexMotorSet(M_CLAW_A, 0);
        vexMotorSet(M_CLAW_B, 0);
      }

      if(vexControllerGet(Btn7R)){
        clearDriveEncoders();
      }
      //Don't hog cpu
      vexSleep(10);
    }
    return (msg_t)0;
}
