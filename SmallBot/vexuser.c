#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"
#include "../Common/easing.h"

// Motor configs
#define M_DRIVE_RIGHT1    kVexMotor_2
#define M_FLY_WHEEL       kVexMotor_3
#define M_FEED_FRONT      kVexMotor_4
#define M_DRIVE_RIGHT2    kVexMotor_5
#define M_DRIVE_LEFT1     kVexMotor_6
#define M_DRIVE_LEFT2     kVexMotor_7
#define M_FEED_SHOOT      kVexMotor_8

// Sensor channels
#define P_ENC_FLY_A       kVexDigital_1
#define P_ENC_FLY_B       kVexDigital_2
#define S_ENC_FLY         kVexSensorDigital_2
#define S_IME_DRIVE_RIGHT kVexSensorIme_1
#define S_IME_DRIVE_LEFT  kVexSensorIme_2

// Joystick settings
#define J_DRIVE      Ch3
#define J_TURN       Ch1
#define J_SHOOT      Btn6U
#define J_FEED_SHOOT Btn8D
#define J_FEED_REV Btn6D
#define J_FEED_FRONT_U Btn5U
#define J_FEED_FRONT_D Btn5D

// Constants
#define FEED_SPOOL_TIME 100

// Digi IO configuration
static  vexDigiCfg  dConfig[kVexDigital_Num] = {
        { P_ENC_FLY_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_1},
        { P_ENC_FLY_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_1}
};

// Motor Config
static  vexMotorCfg mConfig[kVexMotorNum] = {
    { M_DRIVE_LEFT1,    kVexMotor393S,           kVexMotorNormal,     kVexSensorIME,         kImeChannel_2 },
    { M_DRIVE_RIGHT1,   kVexMotor393S,           kVexMotorNormal,     kVexSensorIME,         kImeChannel_1 },
    { M_FEED_SHOOT,     kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
    { M_FLY_WHEEL,      kVexMotor393T,           kVexMotorNormal,     kVexSensorQuadEncoder, kVexQuadEncoder_1 },
    { M_FEED_FRONT,     kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
    { M_DRIVE_LEFT2,    kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
    { M_DRIVE_RIGHT2,   kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 }
};


// PID Controllers
pidController *pidcFly;
pidController *pidcLD;
pidController *pidcRD;

EasingConfig *easingDriveLeft1;
EasingConfig *easingDriveLeft2;
EasingConfig *easingDriveRight1;
EasingConfig *easingDriveRight2;

bool driveMotors(void) {
    short ld, rd ;
    //Calculate Motor Power
    int forward = VALLEY(vexControllerGet(J_DRIVE), 45, 127);
    int turn    = VALLEY(vexControllerGet(J_TURN), 45, 127);
    ld = VALLEY(forward + turn, 45, 127);
    rd = VALLEY(forward - turn, 45, 127);

	/*pidcLD = PidControllerInit(1, 0, 0.5, S_IME_DRIVE_LEFT, false);
	pidcRD = PidControllerInit(1, 0, 0.5, S_IME_DRIVE_RIGHT, false);
	
	easingDriveLeft1 = configPidEasing(pidcLD, func?, 100, true);
	easingDriveLeft2 = configPidEasing(pidcLD, func?, 100, true);
	easingDriveRight1 = configPidEasing(pidcRD, func?, 100, true);
	easingDriveRight2 = configPidEasing(pidcRD, func?, 100, true);
    
	startEasing(easingDriveLeft1,  ld);
    startEasing(easingDriveLeft2,  ld);
    startEasing(easingDriveRight1, rd);
    startEasing(easingDriveRight2, rd);*/
	
    vexMotorSet(M_DRIVE_LEFT1,  ld);
    vexMotorSet(M_DRIVE_LEFT2,  ld);
    vexMotorSet(M_DRIVE_RIGHT1, rd);
    vexMotorSet(M_DRIVE_RIGHT2, rd);
	
    return (ld != 0 || rd != 0);
}


msg_t flyWheelTask(void *arg) {
    (void)arg;

    vexTaskRegister("flyWheelTask");


    // initialize PID COntroller
    pidcFly = PidControllerInit(1, 0, 0, S_ENC_FLY, true);

    while(!chThdShouldTerminate()) {
        if(vexControllerGet(J_SHOOT)) {
            if(!pidcFly->enabled) {
                pidcFly->enabled = true;
                vexSensorValueSet(S_ENC_FLY, 0);
                pidcFly->target_value = 0;
            } else {
                pidcFly->target_value += 1000;
            }
        } else {
            pidcFly->enabled = false;
        }
        PidControllerUpdate(pidcFly);
        vexMotorSet(M_FLY_WHEEL, pidcFly->drive_cmd);
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
    PidControllerMakeLut();
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
	
	StartTask(easingTask);
	
	pidcLD = PidControllerInit(1, 0, 0.5, S_IME_DRIVE_LEFT, false);
	pidcRD = PidControllerInit(1, 0, 0.5, S_IME_DRIVE_RIGHT, false);
	
	easingDriveLeft1 = configPidEasing(pidcLD, M_DRIVE_LEFT1, kMinJerk, 100, false);
	easingDriveLeft2 = configPidEasing(pidcLD, M_DRIVE_LEFT2, kMinJerk, 100, false);
	easingDriveRight1 = configPidEasing(pidcRD, M_DRIVE_RIGHT1, kMinJerk, 100, false);
	easingDriveRight2 = configPidEasing(pidcRD, M_DRIVE_RIGHT2, kMinJerk, 100, false);

	startEasing(easingDriveLeft1, 10);
	startEasing(easingDriveLeft2, 10);
	startEasing(easingDriveRight1, 10);
	startEasing(easingDriveRight2, 10);

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

    //easingDriveLeft1 = configMotorEasing(M_DRIVE_LEFT1, kLinear, 100, true);
    //setOutputValley(easingDriveLeft1, 45, 127);
    //easingDriveLeft2 = configMotorEasing(M_DRIVE_LEFT2, kLinear, 100, true);
    //setOutputValley(easingDriveLeft2, 45, 127);
    //easingDriveRight1 = configMotorEasing(M_DRIVE_RIGHT1, kLinear, 100, true);
    //setOutputValley(easingDriveRight1, 45, 127);
    //easingDriveRight2 = configMotorEasing(M_DRIVE_RIGHT2, kLinear, 100, true);
    //setOutputValley(easingDriveRight2, 45, 127);

	// Must call this
	vexTaskRegister("operator");

    StartTask(flyWheelTask);
    //StartTask(easingTask);

	// Run until asked to terminate
    //int feedSpoolCounter = 0;
	while(!chThdShouldTerminate())
	{
        bool motorRunning = driveMotors();

        if(vexControllerGet(J_FEED_FRONT_U)) {
            vexMotorSet(M_FEED_FRONT, 100);
        } else if(vexControllerGet(J_FEED_FRONT_D)) {
            vexMotorSet(M_FEED_FRONT, -100);
        }  else if(motorRunning) {
			vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
		} else {
            vexMotorSet(M_FEED_FRONT, 0);
        }

        if(vexControllerGet(J_FEED_SHOOT)) { //8D
            vexMotorSet(M_FEED_SHOOT, -127);
            vexMotorSet(M_FEED_FRONT, 127);
        } else if(motorRunning) {
			//vexMotorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
            vexMotorSet(M_FEED_FRONT, -1*DEFAULT_FEED_SPEED); 
		} else {
            vexMotorSet(M_FEED_SHOOT, 0);
            if(!(vexControllerGet(J_FEED_FRONT_D) || vexControllerGet(J_FEED_FRONT_U))) {
                vexMotorSet(M_FEED_FRONT, 0);
            }
        }
		
		if(vexControllerGet(J_FEED_REV)) {
            vexMotorSet(M_FEED_SHOOT, 127);
            vexMotorSet(M_FEED_FRONT, -127);
        } else {
			    vexMotorSet(M_FEED_SHOOT, 0);
            vexMotorSet(M_FEED_FRONT, 0);
		}
		
        vexSleep( 25 );
	}

	return (msg_t)0;
}
