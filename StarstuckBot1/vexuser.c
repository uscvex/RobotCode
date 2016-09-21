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
#define M_LIFT_RIGHT  kVexMotor_3
#define M_LIFT_LEFT   kVexMotor_2

// Drive
/*
#define M_DRIVE_RIGHT1
#define M_DRIVE_RIGHT2
#define M_DRIVE_LEFT1
#define M_DRIVE_LEFT2
*/

// Controller mappings
#define J_LIFT_UP    Btn6U
#define J_LIFT_DOWN  Btn6D



//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {
  { M_LIFT_LEFT,    kVexMotor393S, kVexMotorReversed, kVexSensorNone,  0 },
  { M_LIFT_RIGHT,   kVexMotor393S, kVexMotorNormal,   kVexSensorNone,  0 }
};

//-------------------Setup----------------------------------------------------//
void vexUserSetup()
{
    //vexDigitalPinSet(kVexDigital_5, 0);
    //vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
    vexMotorConfigure( mConfig, MOT_CONFIG_SIZE( mConfig ) )  ;
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

/*
bool driveMotors(void) {
  short ld, rd ;
  //Calculate Motor Power
  int forward = VALLEY(vexControllerGet(J_DRIVE), 25, 127);
  int turn;

  turn = VALLEY(vexControllerGet(J_TURN), 25, 127);
  ld = VALLEY(forward + turn, 25, 127);
  rd = VALLEY(forward - turn, 25, 127);

  vexMotorSet(M_DRIVE_LEFT_A,  ld);
  vexMotorSet(M_DRIVE_LEFT_B,  ld);
  vexMotorSet(M_DRIVE_RIGHT_A, rd);
  vexMotorSet(M_DRIVE_RIGHT_B, rd);

  return (ld != 0 || rd != 0);
}
*/


//---------------------Autonomous routine-------------------------------------//

msg_t vexAutonomous( void *arg )
{
    (void)arg;
    vexTaskRegister("auton");

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

    while(!chThdShouldTerminate())
    {
      if(vexControllerGet(J_LIFT_UP)) {
        vexMotorSet(M_LIFT_RIGHT, 100);
        vexMotorSet(M_LIFT_LEFT, 100);
      }

      else if(vexControllerGet(J_LIFT_DOWN)) {
        vexMotorSet(M_LIFT_RIGHT, -52);
        vexMotorSet(M_LIFT_LEFT, -52);
      }
      else {
        vexMotorSet(M_LIFT_RIGHT, 0);
        vexMotorSet(M_LIFT_LEFT, 0);
      }

      vexSleep(10);
    }
    return (msg_t)0;
}
