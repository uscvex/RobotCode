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
#define P_ENC_FLY_TOP_A kVexDigital_3
#define P_ENC_FLY_TOP_B kVexDigital_4
#define S_ENC_FLY_TOP   kVexSensorDigital_4
#define P_ENC_FLY_BOT_A kVexDigital_1
#define P_ENC_FLY_BOT_B kVexDigital_2
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
        { P_ENC_FLY_BOT_A,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_1},
        { P_ENC_FLY_BOT_B,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_1},
        { P_ENC_FLY_TOP_A,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_2},
        { P_ENC_FLY_TOP_B,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_2}
};

static  vexMotorCfg mConfig[kVexMotorNum] = {
        { M_FEED_FRONT,        kVexMotor393T,           kVexMotorNormal,       kVexSensorNone,        0 },
        { M_DRIVE_FRONT_LEFT,  kVexMotor393T,           kVexMotorNormal,       kVexSensorNone,        0 },
        { M_DRIVE_BACK_LEFT,   kVexMotor393T,           kVexMotorNormal,       kVexSensorNone,        0 },
        { M_FLY_TOP,           kVexMotor393S,           kVexMotorNormal,     kVexSensorQuadEncoder, kVexQuadEncoder_2 },
        { M_FLY_BOT,           kVexMotor393S,           kVexMotorNormal,     kVexSensorQuadEncoder, kVexQuadEncoder_1 },
        { M_DRIVE_BACK_RIGHT,  kVexMotor393T,           kVexMotorReversed,     kVexSensorNone,        0 },
        { M_FEED_SHOOT,        kVexMotor393T,           kVexMotorReversed,     kVexSensorNone,        0 },
        { M_DRIVE_FRONT_RIGHT, kVexMotor393T,           kVexMotorReversed,     kVexSensorNone,        0 },
};

// PID Controlers
pidController *pidcFlyTop;
pidController *pidcFlyBot;

int doMotorDrive(void) {
    int C1LX = VALLEY(vexControllerGet(J_XDRIVE_X), 45, 127);
    int C1LY = VALLEY(-vexControllerGet(J_XDRIVE_Y), 45, 127);
    int C1RX = VALLEY(vexControllerGet(J_XDRIVE_R), 45, 127);

    int targetFrontLeft = VALLEY(-C1LY - C1LX + C1RX, 45, 127);
	int targetFrontRight = VALLEY(C1LY  - C1LX + C1RX, 45, 127);
	int targetBackLeft = VALLEY(-C1LY - C1LX - C1RX, 45, 127);
	int targetBackRight = VALLEY(C1LY  - C1LX - C1RX, 45, 127);
	
	int currFrontLeft = vexMotorGet(M_DRIVE_FRONT_LEFT);
	int currFrontRight = vexMotorGet(M_DRIVE_FRONT_RIGHT);
	int currBackLeft = vexMotorGet(M_DRIVE_BACK_LEFT);
	int currBackRight = vexMotorGet(M_DRIVE_BACK_RIGHT);
	
	while(true) {
		if (currFrontLeft == targetFrontLeft && currFrontRight == targetFrontRight && currBackLeft == targetBackLeft && currBackRight == targetBackRight)
			break;
		if(currFrontLeft != targetFrontLeft)
			currFrontLeft = VALLEY(currFrontLeft+10*SIGN(targetFrontLeft), 0, targetFrontLeft);
		if(currFrontRight != targetFrontRight)
			currFrontRight = VALLEY(currFrontRight+10*SIGN(targetFrontRight), 0, targetFrontRight);
		if(currBackLeft != targetBackLeft)
			currBackLeft = VALLEY(currBackLeft+10*SIGN(targetBackLeft), 0, targetBackLeft);
		if(currBackRight != targetBackRight)
			currBackRight = VALLEY(currFrontLeft+10*SIGN(targetBackRight), 0, targetBackRight);
		
		vexMotorSet(M_DRIVE_FRONT_LEFT, currFrontLeft);
		vexMotorSet(M_DRIVE_FRONT_RIGHT, currFrontRight);
		vexMotorSet(M_DRIVE_BACK_RIGHT, currBackLeft);
		vexMotorSet(M_DRIVE_BACK_LEFT, currBackRight);
	}
    return( targetFrontLeft !=0 && targetFrontRight != 0 && targetBackLeft != 0 && targetBackRight !=0);
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
    pidcFlyTop = PidControllerInit(0.5, 0, 0, S_ENC_FLY_TOP, false);
    pidcFlyBot = PidControllerInit(0.5, 0, 0, S_ENC_FLY_BOT, true);

    while(!chThdShouldTerminate()) {
        if(vexControllerGet(J_SHOOT)) {
            if(!pidcFlyTop->enabled) {
                pidcFlyTop->enabled = true;
                pidcFlyBot->enabled = true;
                vexSensorValueSet(S_ENC_FLY_TOP, 0);
                vexSensorValueSet(S_ENC_FLY_BOT, 0);
                pidcFlyBot->target_value = 0;
                pidcFlyTop->target_value = 0;
            } else {
                pidcFlyBot->target_value += 600;
                pidcFlyTop->target_value += 600;
            }
        } else {
            pidcFlyTop->enabled = false;
            pidcFlyBot->enabled = false;
        }
        PidControllerUpdate(pidcFlyBot);
        vexMotorSet(M_FLY_TOP, pidcFlyTop->drive_cmd);
        vexMotorSet(M_FLY_BOT, pidcFlyBot->drive_cmd);
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
        bool isRunning = doMotorDrive();
        if(vexControllerGet(J_FEED_U)) {
            vexMotorSet(M_FEED_FRONT, 100);
        } else if(vexControllerGet(J_FEED_D)) {
            vexMotorSet(M_FEED_FRONT, -100);
        } else if(isRunning){
            vexMotorSet(M_FEED_FRONT, DEFAULT_SPEED);
        }
		else {
			vexMotorSet(M_FEED_FRONT, DEFAULT_SPEED);
			vexSleep(25);
			vexMotorSet(M_FEED_FRONT, 0);
		}
		
        if(vexControllerGet(J_SHOOT)) {
            if(feedSpoolCounter > FEED_SPOOL_TIME) {
                vexMotorSet(M_FEED_SHOOT, 100);
                vexMotorSet(M_FEED_FRONT, 100);
            } else {
                feedSpoolCounter++;
            }
        } else if (isRunning){
            feedSpoolCounter = 0;
            vexMotorSet(M_FEED_SHOOT, DEFAULT_SPEED);
        }
		else {
            feedSpoolCounter = 0;
            vexMotorSet(M_FEED_SHOOT, DEFAULT_SPEED);
			vexSleep(25);
			vexMotorSet(M_FEED_SHOOT, 0);
        }
        vexSleep( 25 );
	}

	return (msg_t)0;
}
