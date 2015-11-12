#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"

// Motor config
#define M_FEED_FRONT         kVexMotor_2
#define M_DRIVE_FRONT_LEFT   kVexMotor_3
#define M_DRIVE_BACK_LEFT    kVexMotor_4
#define M_FLY_TOP            kVexMotor_5
#define M_FLY_BOT            kVexMotor_6
#define M_DRIVE_BACK_RIGHT   kVexMotor_7
#define M_FEED_SHOOT         kVexMotor_8
#define M_DRIVE_FRONT_RIGHT  kVexMotor_9

// Sensor channels
#define S_ENC_FLY_TOP kVexQuadEncoder_2
#define S_ENC_FLY_BOT kVexQuadEncoder_1

// Joystick settings
#define J_SHOOT     Btn8R
#define J_FEED      Btn8L
#define J_XDRIVE_X  Ch4
#define J_XDRIVE_Y  Ch3
#define J_XDRIVE_R  Ch1

// Constants
#define FEED_SPOOL_TIME 100

// Digi IO configuration
static  vexDigiCfg  dConfig[kVexDigital_Num] = {
        { kVexDigital_1,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    S_ENC_FLY_BOT }, // bottom
        { kVexDigital_2,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    S_ENC_FLY_BOT },
        { kVexDigital_3,    kVexSensorQuadEncoder,   kVexConfigQuadEnc1,    S_ENC_FLY_TOP }, // top
        { kVexDigital_4,    kVexSensorQuadEncoder,   kVexConfigQuadEnc2,    S_ENC_FLY_TOP },
        { kVexDigital_5,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_6,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_7,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_8,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_9,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_10,   kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_11,   kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_12,   kVexSensorDigitalInput,  kVexConfigInput,       0 }
};

static  vexMotorCfg mConfig[kVexMotorNum] = {
        { kVexMotor_1,         kVexMotorUndefined,      kVexMotorNormal,       kVexSensorNone,        0 },
        { M_FEED_FRONT,        kVexMotor393T,           kVexMotorNormal,       kVexSensorNone,        0 },
        { M_DRIVE_FRONT_LEFT,  kVexMotor393T,           kVexMotorNormal,       kVexSensorNone,        0 },
        { M_DRIVE_BACK_LEFT,   kVexMotor393T,           kVexMotorNormal,       kVexSensorNone,        0 },
        { M_FLY_TOP,           kVexMotor393S,           kVexMotorReversed,     kVexSensorQuadEncoder, kVexQuadEncoder_2 },
        { M_FLY_BOT,           kVexMotor393S,           kVexMotorReversed,     kVexSensorQuadEncoder, kVexQuadEncoder_1 },
        { M_DRIVE_BACK_RIGHT,  kVexMotor393T,           kVexMotorReversed,     kVexSensorNone,        0 },
        { M_FEED_SHOOT,        kVexMotor393T,           kVexMotorReversed,     kVexSensorNone,        0 },
        { M_DRIVE_FRONT_RIGHT, kVexMotor393T,           kVexMotorReversed,     kVexSensorNone,        0 },
        { kVexMotor_10,        kVexMotorUndefined,      kVexMotorNormal,       kVexSensorNone,        0 }
};

// PID Controlers
pidController *pidcFlyTop;
pidController *pidcFlyBot;

void doMotorDrive(void) {
    int C1LX = VALLEY(vexControllerGet(J_XDRIVE_X), 45, 127);
    int C1LY = VALLEY(vexControllerGet(J_XDRIVE_Y), 45, 127);
    int C1RX = VALLEY(vexControllerGet(J_XDRIVE_R), 45, 127);

    // Y component, X component, Rotation
    vexMotorSet(M_DRIVE_FRONT_LEFT,   VALLEY(-C1LY - C1LX + C1RX, 45, 127));
    vexMotorSet(M_DRIVE_FRONT_RIGHT,  VALLEY(C1LY  - C1LX + C1RX, 45, 127));
    vexMotorSet(M_DRIVE_BACK_RIGHT,   VALLEY(C1LY  - C1LX - C1RX, 45, 127));
    vexMotorSet(M_DRIVE_BACK_LEFT,    VALLEY(-C1LY - C1LX - C1RX, 45, 127));
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

msg_t flyWheelTask(void *arg) {
    (void)arg;

    vexTaskRegister("flyWheelTask");

    PidControllerMakeLut();

    // initialize PID COntroller
    pidcFlyTop = PidControllerInit(0.5, 0, 0, S_ENC_FLY_TOP, true);
    pidcFlyBot = PidControllerInit(0.5, 0, 0, S_ENC_FLY_BOT, true);

    vexSensorValueSet(S_ENC_FLY_BOT, 0);
    vexSensorValueSet(S_ENC_FLY_TOP, 0);

    int step = 0;
    while(!chThdShouldTerminate()) {
        if(vexControllerGet(J_SHOOT)) {
            if(!pidcFlyTop->enabled) {
                pidcFlyTop->enabled = true;
                pidcFlyBot->enabled = true;
                pidcFlyBot->target_value = 0;
                pidcFlyTop->target_value = 0;
                vexSensorValueSet(S_ENC_FLY_BOT, 0);
                vexSensorValueSet(S_ENC_FLY_TOP, 0);
            } else if(step <= 0) {
                //vexSensorValueSet(S_ENC_FLY_BOT, 0);
                //vexSensorValueSet(S_ENC_FLY_TOP, 0);
                pidcFlyBot->target_value = 600;
                pidcFlyTop->target_value = 600;
                step = 0;
            }
        } else {
            step = 0;
            pidcFlyTop->enabled = false;
            pidcFlyBot->enabled = false;
        }
        //vex_printf("V=%d E=%f C=%d R=%d, ", pidcFlyTop->sensor_value, pidcFlyTop->error, pidcFlyTop->drive_cmd, pidcFlyTop->drive_raw);
        //vex_printf("V=%d E=%f C=%d R=%d\n", pidcFlyBot->sensor_value, pidcFlyBot->error, pidcFlyBot->drive_cmd, pidcFlyBot->drive_raw);
        PidControllerUpdate(pidcFlyTop);
        PidControllerUpdate(pidcFlyBot);
        vexMotorSet(M_FLY_TOP, pidcFlyTop->drive_cmd);
        vexMotorSet(M_FLY_BOT, pidcFlyBot->drive_cmd);

        step--;
        vexSleep(25);
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
    int feedSpoolCounter = 0;
	while(!chThdShouldTerminate())
	{
        doMotorDrive();
        vexMotorSet(M_FEED_FRONT, vexControllerGet(J_FEED)?100:0);
        if(vexControllerGet(J_SHOOT)) {
            if(feedSpoolCounter > FEED_SPOOL_TIME) {
                vexMotorSet(M_FEED_SHOOT, 100);
                vexMotorSet(M_FEED_FRONT, 100);
            } else {
                feedSpoolCounter++;
            }
        } else {
            feedSpoolCounter = 0;
            vexMotorSet(M_FEED_SHOOT, 0);
        }
        vexSleep( 25 );
	}

	return (msg_t)0;
}
