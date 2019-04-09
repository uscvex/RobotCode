#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"
#include "../Common/easing.h"


#define M_PORT8 kVexMotor_8
#define M_PORT3 kVexMotor_3
#define M_PORT2 kVexMotor_2

#define P_CLAW  kVexDigital_9
#define J_CLAW  Btn7U

// Motor mappings

// Controller mappings




//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {
  { M_PORT3,   kVexMotor393S, kVexMotorNormal,  kVexSensorNone, 0 },
  { M_PORT8,   kVexMotor393S, kVexMotorNormal,  kVexSensorNone, 0 }
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

  vexMotorSet(M_DRIVE_LEFT,  ld);
  vexMotorSet(M_DRIVE_RIGHT, rd);

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
    vexDigitalPinSet(P_CLAW, 0);

    while(!chThdShouldTerminate())
    {

      //vexMotorSet(M_PORT, 50);

      if(vexControllerGet(J_CLAW)) {
          vexDigitalPinSet(P_CLAW, 1);
      } else {
          vexDigitalPinSet(P_CLAW, 0);
      }

      vexSleep(10);
    }
    return (msg_t)0;
}
