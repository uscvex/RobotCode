#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"

// Motor configs
#define M_DRIVE_LEFT1     kVexMotor_2
#define M_DRIVE_RIGHT1    kVexMotor_3
#define M_FEED_SHOOT      kVexMotor_4
#define M_FLY_WHEEL       kVexMotor_5
#define M_FEED_FRONT      kVexMotor_7
#define M_DRIVE_LEFT2     kVexMotor_8
#define M_DRIVE_RIGHT2    kVexMotor_9

// Sensor channels
#define S_ENC_FLY         kVexQuadEncoder_1

// Joystick settings
#define J_DRIVE      Ch3
#define J_TURN       Ch1
#define J_SHOOT      Btn6U
#define J_FEED_SHOOT Btn6D
#define J_FEED_U     Btn5U
#define J_FEED_D     Btn5D

// Constants
#define FEED_SPOOL_TIME 100

// Digi IO configuration
static  vexDigiCfg  dConfig[kVexDigital_Num] = {
        { kVexDigital_1,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    S_ENC_FLY},
        { kVexDigital_2,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    S_ENC_FLY},
        { kVexDigital_3,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_4,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_5,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_6,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_7,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_8,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_9,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_10,   kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_11,   kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_12,   kVexSensorDigitalInput,  kVexConfigInput,       0 }
};

// Motor Config
static  vexMotorCfg mConfig[kVexMotorNum] = {
        { kVexMotor_1,      kVexMotorUndefined,      kVexMotorNormal,     kVexSensorNone,        0 },
        { M_DRIVE_LEFT1,    kVexMotor393S,           kVexMotorNormal,     kVexSensorIME,         kImeChannel_1 },
        { M_DRIVE_RIGHT1,   kVexMotor393S,           kVexMotorNormal,     kVexSensorIME,         kImeChannel_2 },
        { M_FEED_SHOOT,     kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
        { M_FLY_WHEEL,      kVexMotor393T,           kVexMotorReversed,   kVexSensorQuadEncoder, kVexQuadEncoder_1 },
        { kVexMotor_6,      kVexMotorUndefined,      kVexMotorNormal,     kVexSensorNone,        0 },
        { M_FEED_FRONT,     kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
        { M_DRIVE_LEFT2,    kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
        { M_DRIVE_RIGHT2,   kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
        { kVexMotor_10,     kVexMotorUndefined,      kVexMotorNormal,     kVexSensorNone,        0 }
};


// PID Controllers
pidController *pidcFly;

void driveMotors(void) {
    short ld, rd ;
    //Calculate Motor Power
    int forward = VALLEY(vexControllerGet(J_DRIVE), 45, 127);
    int turn    = VALLEY(vexControllerGet(J_TURN), 45, 127);
    ld = forward + turn;
    rd = forward - turn;

    vexMotorSet(M_DRIVE_LEFT1,  VALLEY(ld, 45, 127));
    vexMotorSet(M_DRIVE_LEFT2,  VALLEY(ld, 45, 127));
    vexMotorSet(M_DRIVE_RIGHT1, VALLEY(rd, 45, 127));
    vexMotorSet(M_DRIVE_RIGHT2, VALLEY(rd, 45, 127));
}


msg_t flyWheelTask(void *arg) {
    (void)arg;

    vexTaskRegister("flyWheelTask");

    PidControllerMakeLut();

    // initialize PID COntroller
    pidcFly = PidControllerInit(0.5, 0, 0, S_ENC_FLY, true);

    vexSensorValueSet(S_ENC_FLY, 0);

    int step = 0;
    while(!chThdShouldTerminate()) {
        if(vexControllerGet(J_SHOOT)) {
            if(!pidcFly->enabled) {
                pidcFly->enabled = true;
                pidcFly->target_value = 0;
                vexSensorValueSet(S_ENC_FLY, 0);
            } else if(step <= 0) {
                pidcFly->target_value = 700;
                step = 0;
            }
        } else {
            step = 0;
            pidcFly->enabled = false;
        }
        PidControllerUpdate(pidcFly);
        vexMotorSet(M_FLY_WHEEL, pidcFly->drive_cmd);

        step--;
        vexSleep(25);
    }

    return (msg_t)0;
}

/*-----------------------------------------------------------------------------*/
/** @brief      User setup                                                     */
/*-----------------------------------------------------------------------------*/
/** @details
 *  The digital and motor ports can (should) be configured here.
 */
void
vexUserSetup()
{
	vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
	vexMotorConfigure( mConfig, MOT_CONFIG_SIZE( mConfig ) );
}

/*-----------------------------------------------------------------------------*/
/** @brief      User initialize                                                */
/*-----------------------------------------------------------------------------*/
/** @details
 *  This function is called after all setup is complete and communication has
 *  been established with the master processor.
 *  Start other tasks and initialize user variables here
 */
void
vexUserInit()
{
}

/*-----------------------------------------------------------------------------*/
/** @brief      Autonomous                                                     */
/*-----------------------------------------------------------------------------*/
/** @details
 *  This thread is started when the autonomous period is started
 */
msg_t
vexAutonomous( void *arg )
{
    (void)arg;

    // Must call this
    vexTaskRegister("auton");

    while(1)
        {
        // Don't hog cpu
        vexSleep( 25 );
        }

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

	// Must call this
	vexTaskRegister("operator");

    StartTask(flyWheelTask);

	// Run until asked to terminate
    //int feedSpoolCounter = 0;
	while(!chThdShouldTerminate())
	{
        driveMotors();
        vexMotorSet(M_FEED_FRONT, vexControllerGet(J_FEED_U)?100:0);
        vexMotorSet(M_FEED_FRONT, vexControllerGet(J_FEED_D)?-100:0);
        vexMotorSet(M_FEED_SHOOT, vexControllerGet(J_FEED_SHOOT)?127:0);

/*        if(vexControllerGet(J_SHOOT)) {
            vexMotorSet(M_FLY_WHEEL, 100);
            if(feedSpoolCounter > FEED_SPOOL_TIME) {
                vexMotorSet(M_FEED_SHOOT, 100);
            } else {
                feedSpoolCounter++;
            }
        } else {
            vexMotorSet(M_FLY_WHEEL, 0);
            feedSpoolCounter = 0;
            vexMotorSet(M_FEED_SHOOT, 0);
        }
*/
        vexSleep( 25 );
	}

	return (msg_t)0;
}
