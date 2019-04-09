#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"

#define M_CLAW        kVexMotor_3
#define M_RIGHT_DRIVE kVexMotor_5
#define M_LEFT_DRIVE  kVexMotor_4
#define M_LIFT        kVexMotor_2

#define J_DRIVE      Ch3
#define J_TURN       Ch1

#define J_LIFT_U     Btn6U
#define J_LIFT_D     Btn6D

#define J_CLAW_U     Btn5U
#define J_CLAW_D     Btn5D


// Digi IO configuration
static  vexDigiCfg  dConfig[] = {
};

// Motor Config
static  vexMotorCfg mConfig[] = {
    { M_CLAW,    kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
    { M_RIGHT_DRIVE,    kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
    { M_LEFT_DRIVE,    kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
    { M_LIFT,    kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
};

bool driveMotors(void) {
    short ld, rd ;
    //Calculate Motor Power
    int forward = VALLEY(vexControllerGet(J_DRIVE), 45, 127);
    int turn    = VALLEY(vexControllerGet(J_TURN), 45, 127);
    ld = VALLEY(forward + turn, 45, 127);
    rd = VALLEY(forward - turn, 45, 127);

    vexMotorSet(M_LEFT_DRIVE,  ld);
    vexMotorSet(M_RIGHT_DRIVE, rd);

    return (ld != 0 || rd != 0);
}

void
vexUserSetup()
{
	vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
	vexMotorConfigure( mConfig, MOT_CONFIG_SIZE( mConfig ) );
}

void
vexUserInit()
{
}

msg_t
vexAutonomous( void *arg )
{
    (void)arg;
    return (msg_t)0;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Driver control                                                 */
/*-----------------------------------------------------------------------------*/
/** @details
 *  This thread is started when the driver control period is started
 */
msg_t
vexOperator( void *arg )
{
	(void)arg;

	bool hold = false;
	while(!chThdShouldTerminate())
	{
		driveMotors();

		if(vexControllerGet(J_LIFT_U)) {
			hold = true;
			vexMotorSet(M_LIFT, 80);
		} else if(vexControllerGet(J_LIFT_D)) {
			hold = false;
			vexMotorSet(M_LIFT, -80);
		} else if(hold){
			vexMotorSet(M_LIFT, 20);
		} else {
			vexMotorSet(M_LIFT, 0);
		}

		if(vexControllerGet(J_CLAW_U)) {
			vexMotorSet(M_CLAW, 80);
		} else if(vexControllerGet(J_CLAW_D)) {
			vexMotorSet(M_CLAW, -80);
		} else {
			vexMotorSet(M_CLAW, 0);
		}
        vexSleep( 10 );

	}

	return (msg_t)0;
}
