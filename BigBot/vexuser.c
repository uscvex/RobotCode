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
#define M_DRIVE_FRONT_RIGHT  kVexMotor_9
#define M_FEED_SHOOT         kVexMotor_8

// Sensor channels
#define P_ENC_FLY_TOP_A kVexDigital_3
#define P_ENC_FLY_TOP_B kVexDigital_4
#define S_ENC_FLY_TOP   kVexSensorDigital_4
#define P_ENC_FLY_BOT_A kVexDigital_1
#define P_ENC_FLY_BOT_B kVexDigital_2
#define S_ENC_FLY_BOT   kVexSensorDigital_2

#define S_ENC_FREE_LEFT   kVexSensorDigital_10
#define S_ENC_FREE_RIGHT  kVexSensorDigital_12

#define S_ENC_FLY_BOT   kVexSensorDigital_2
#define S_ENC_FLY_BOT   kVexSensorDigital_2

// Joystick settings
#define J_SHOOT     Btn8R
#define J_FEED_U    Btn6U
#define J_FEED_D    Btn6D
#define J_XDRIVE_X  Ch4
#define J_XDRIVE_Y  Ch3
#define J_XDRIVE_R  Ch1

// Constants
#define FEED_SPOOL_TIME 100

// Digi IO configuration
static  vexDigiCfg  dConfig[kVexDigital_Num] = {
        { kVexDigital_1,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_1},
        { kVexDigital_2,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_1},
        { kVexDigital_3,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_2},
        { kVexDigital_4,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_2},
        { kVexDigital_5,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_6,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_7,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_8,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_9,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_1},
        { kVexDigital_10,   kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_1},
        { kVexDigital_11,   kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_2},
        { kVexDigital_12,   kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_2},
};

static  vexMotorCfg mConfig[kVexMotorNum] = {
        { kVexMotor_1,        kVexMotorUndefined,      kVexMotorNormal,       kVexSensorNone,        0 },
        { kVexMotor_2,        kVexMotor393T,           kVexMotorNormal,       kVexSensorNone,        0 },
        { kVexMotor_3,        kVexMotor393T,           kVexMotorNormal,       kVexSensorNone,        0 },
        { kVexMotor_4,        kVexMotor393T,           kVexMotorNormal,       kVexSensorNone,        0 },
        { kVexMotor_5,        kVexMotor393S,           kVexMotorNormal,       kVexSensorQuadEncoder, kVexQuadEncoder_2 },
        { kVexMotor_6,        kVexMotor393S,           kVexMotorNormal,       kVexSensorQuadEncoder, kVexQuadEncoder_1 },
        { kVexMotor_7,        kVexMotor393T,           kVexMotorReversed,     kVexSensorNone,        0 },
        { kVexMotor_8,        kVexMotor393T,           kVexMotorReversed,     kVexSensorNone,        0 },
        { kVexMotor_9,        kVexMotor393T,           kVexMotorReversed,     kVexSensorNone,        0 },
        { kVexMotor_10,       kVexMotorUndefined,      kVexMotorNormal,       kVexSensorNone,        0 },
};

// PID Controlers
//pidController *pidcFlyTop;
//pidController *pidcFlyBot;
pidController *pidcFreeLeft;

void doMotorDrive(int16_t x, int16_t y, int16_t r) {
    // Y component, X component, Rotation
    vexMotorSet(M_DRIVE_FRONT_LEFT,   VALLEY(-y - x + r, 45, 127));
    vexMotorSet(M_DRIVE_FRONT_RIGHT,  VALLEY(y  - x + r, 45, 127));
    vexMotorSet(M_DRIVE_BACK_RIGHT,   VALLEY(y  - x - r, 45, 127));
    vexMotorSet(M_DRIVE_BACK_LEFT,    VALLEY(-y - x - r, 45, 127));
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
    //pidcFreeLeft = PidControllerInit(0.5, 0, 0.5, S_ENC_FREE_LEFT, true);

    // initialize PID COntroller
    float kPTop = 10;
    float kPBot = 50;
    float topTargetVelocity = 6.5;
    int32_t topLastValue;
    float botTargetVelocity = 6.5;
    int32_t botLastValue;

    int32_t lastTime;
    int32_t currentTime;

    bool isRunning = false;
    while(!chThdShouldTerminate()) {
        currentTime = chTimeNow();
        if(vexControllerGet(J_SHOOT)) {
            if(!isRunning) {
                vexSensorValueSet(S_ENC_FLY_BOT, 0);
                vexSensorValueSet(S_ENC_FLY_TOP, 0);
                topLastValue = 0;
                botLastValue = 0;
                isRunning = true;
                //pidcFreeLeft->target_value = vexSensorValueGet(S_ENC_FREE_LEFT);
            } else {
                int32_t topCurrentValue = vexSensorValueGet(S_ENC_FLY_TOP);
                float topVelocity = (topCurrentValue - topLastValue)/((float)(lastTime-currentTime));
                float deltaV = topTargetVelocity - topVelocity;
                vexMotorSet(M_FLY_TOP, deltaV * kPTop);
                topLastValue = topCurrentValue;

                int32_t botCurrentValue = vexSensorValueGet(S_ENC_FLY_BOT);
                float botVelocity = (botCurrentValue - botLastValue)/((float)(lastTime-currentTime));
                deltaV = botTargetVelocity - botVelocity;
                vexMotorSet(M_FLY_BOT, deltaV * kPBot);
                botLastValue = botCurrentValue;
            }
            //PidControllerUpdate(pidcFreeLeft);
            //doMotorDrive(0, 0, pidcFreeLeft->drive_raw);
        } else {
            vexMotorSet(M_FLY_TOP, 0);
            vexMotorSet(M_FLY_BOT, 0);
            isRunning = false;
        }
        lastTime = currentTime;
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
        //vex_printf("%d %d\n", vexSensorValueGet(S_ENC_FREE_RIGHT), vexSensorValueGet(S_ENC_FREE_LEFT));
        int x = VALLEY(vexControllerGet(J_XDRIVE_X), 45, 127);
        int y = VALLEY(vexControllerGet(J_XDRIVE_Y), 45, 127);
        int r = VALLEY(vexControllerGet(J_XDRIVE_R), 45, 127);
        doMotorDrive(x, y, r);
        if(vexControllerGet(J_FEED_U)) {
            vexMotorSet(M_FEED_FRONT, 100);
        } else if(vexControllerGet(J_FEED_D)) {
            vexMotorSet(M_FEED_FRONT, -100);
        } else {
            vexMotorSet(M_FEED_FRONT, 0);
        }
        if(vexControllerGet(J_SHOOT)) {
            if(feedSpoolCounter > FEED_SPOOL_TIME) {
                vexMotorSet(M_FEED_SHOOT, 127);
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
