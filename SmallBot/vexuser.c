#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"
#include "../Common/takebackhalf.h"
#include "../Common/easing.h"

// Motor configs

#define M_FLY_BOT_WHEEL      kVexMotor_5
#define M_FLY_TOP_WHEEL      kVexMotor_6

#define M_FEED_FRONT      kVexMotor_2

#define M_DRIVE_RIGHT1    kVexMotor_4
#define M_DRIVE_RIGHT2    kVexMotor_8

#define M_DRIVE_LEFT1     kVexMotor_3
#define M_DRIVE_LEFT2     kVexMotor_7

#define M_FEED_SHOOT      kVexMotor_9

// Sensor channels
#define P_ENC_BOT_FLY_A       kVexDigital_1
#define P_ENC_BOT_FLY_B       kVexDigital_2

#define P_ENC_TOP_FLY_A       kVexDigital_3
#define P_ENC_TOP_FLY_B       kVexDigital_4

#define S_BALL_IN              0
#define S_BALL_IN2             1
#define S_BALL_OUT             2

#define S_ENC_BOT_FLY        kVexSensorDigital_2
#define S_ENC_TOP_FLY        kVexSensorDigital_4

#define S_IME_DRIVE_RIGHT kVexSensorIme_3
#define S_IME_DRIVE_LEFT  kVexSensorIme_2
#define S_IME_FEED_SHOOT  kVexSensorIme_1

// Joystick settings
#define J_DRIVE      Ch3
#define J_TURN       Ch1
//#define J_SHOOT      Btn6U

#define J_SHOOT_MAX     Btn7R
#define J_SHOOT_75      Btn7U
#define J_SHOOT_STOP    Btn7D
#define J_SHOOT_PB      Btn7L
#define J_SHOOT_SIDE    Btn8U

#define J_FEED_SHOOT Btn6U
#define J_FEED_REV   Btn6D
#define J_FEED_FRONT_U Btn5D
#define J_FEED_FRONT_D Btn5U

// Constants
#define DEFAULT_FEED_SPEED 90
#define FEED_SPOOL_TIME 100

#define FLY_MAX_SPEED 10500
#define FLY_SIDE_SPEED 7700
#define FLY_PB_SPEED  5000
#define FLY_75_SPEED  6100

// Digi IO configuration
static  vexDigiCfg  dConfig[] = {
        { P_ENC_BOT_FLY_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_1},
        { P_ENC_BOT_FLY_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_1},

        { P_ENC_TOP_FLY_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_2},
        { P_ENC_TOP_FLY_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_2}
};

// Motor Config
static  vexMotorCfg mConfig[] = {
    { M_DRIVE_LEFT1,    kVexMotor393S,           kVexMotorReversed,     kVexSensorIME,         kImeChannel_2 },
    { M_DRIVE_RIGHT1,   kVexMotor393S,           kVexMotorNormal,     kVexSensorIME,         kImeChannel_1 },
    { M_FEED_SHOOT,     kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },

    { M_FLY_TOP_WHEEL,      kVexMotor393T,      kVexMotorNormal,   kVexSensorQuadEncoder, kVexQuadEncoder_1 },
    { M_FLY_BOT_WHEEL,     kVexMotor393T,      kVexMotorNormal,     kVexSensorQuadEncoder, kVexQuadEncoder_2 },

    { M_FEED_FRONT,     kVexMotor393S,           kVexMotorReversed,     kVexSensorNone,        0 },
    { M_DRIVE_LEFT2,    kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
    { M_DRIVE_RIGHT2,   kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 }
};


// TBH Controllers
TBHController *topWheelCtrl;
TBHController *botWheelCtrl;

bool driveMotors(void) {
    short ld, rd ;
    //Calculate Motor Power
    int forward = VALLEY(vexControllerGet(J_DRIVE), 45, 127);
    int turn    = VALLEY(vexControllerGet(J_TURN), 45, 127);
    ld = VALLEY(forward + turn, 45, 127);
    rd = VALLEY(forward - turn, 45, 127);

    vexMotorSet(M_DRIVE_LEFT1,  ld);
    vexMotorSet(M_DRIVE_LEFT2,  ld);
    vexMotorSet(M_DRIVE_RIGHT1, rd);
    vexMotorSet(M_DRIVE_RIGHT2, rd);
	
    return (ld != 0 || rd != 0);
}

bool isBallOuter(void) {
	return (vexAdcGet(S_BALL_OUT) < 500);
}

bool isBallInner(void) {
	return (vexAdcGet(S_BALL_IN) < 500);
}

bool isBallInner2(void) {
	return (vexAdcGet(S_BALL_IN2) < 500);
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
    topWheelCtrl = TBHControllerInit(S_ENC_TOP_FLY, 0.05, 10500, false);
    topWheelCtrl->powerZeroClamp = true;
    topWheelCtrl->log = false;

    botWheelCtrl = TBHControllerInit(S_ENC_BOT_FLY, 0.05, 10500, false);
    botWheelCtrl->log = true;
    botWheelCtrl->powerZeroClamp = true;
}

msg_t
vexAutonomous( void *arg )
{
   (void)arg;
	vexTaskRegister("auton");
	vex_printf("starting autonomous\n");
    int shootCount = 0;
    int step = 0;
    systime_t lastTime = chTimeNow();
    while(!chThdShouldTerminate())
    {
    	systime_t time = chTimeNow();
    	int32_t timeGap = time-lastTime;
    	if(step == 0 && timeGap > 1000) {
			vex_printf("step 0\n");
			vexMotorSet(M_DRIVE_LEFT1,  80);
			vexMotorSet(M_DRIVE_LEFT2,  80);
			vexMotorSet(M_DRIVE_RIGHT1, 80);
			vexMotorSet(M_DRIVE_RIGHT2, 80);
			step++;
			lastTime = time;
    	}
    	else if(step == 1 && timeGap > 2000) {
			vex_printf("step 1\n");
			vexMotorSet(M_DRIVE_LEFT1,  0);
			vexMotorSet(M_DRIVE_LEFT2,  0);
			vexMotorSet(M_DRIVE_RIGHT1, 0);
			vexMotorSet(M_DRIVE_RIGHT2, 0);
    		step++;
    	} else if(step == 2) {
    		break;
    	}
        vexSleep( 10 );
    }
	vex_printf("End\n");
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


	// Run until asked to terminate
    int  motorRunningTime = 0;
    bool ballFeed = false;
	while(!chThdShouldTerminate())
	{
		if(isBallInner2()) {
			ballFeed = true;
		}
		if(isBallOuter()) {
			ballFeed = false;
		}

        bool motorRunning = driveMotors();
        if(motorRunning) {
            motorRunningTime = 25;
        } else {
            motorRunningTime = MAX(motorRunningTime-1, 0);
        }
        if(motorRunningTime > 0) {
            motorRunning = true;
        }

        if(vexControllerGet(J_SHOOT_PB)) {
        	tbhEnable(topWheelCtrl, FLY_PB_SPEED);
        	tbhEnable(botWheelCtrl, FLY_PB_SPEED);
        }
        if(vexControllerGet(J_SHOOT_MAX)) {
        	tbhEnable(topWheelCtrl, FLY_MAX_SPEED);
        	tbhEnable(botWheelCtrl, FLY_MAX_SPEED);
        }
        if(vexControllerGet(J_SHOOT_75)) {
        	tbhEnable(topWheelCtrl, FLY_75_SPEED);
        	tbhEnable(botWheelCtrl, FLY_75_SPEED);
        }
        if(vexControllerGet(J_SHOOT_SIDE)) {
        	tbhEnable(topWheelCtrl, FLY_SIDE_SPEED);
        	tbhEnable(botWheelCtrl, FLY_SIDE_SPEED);
        }
        if(vexControllerGet(J_SHOOT_STOP)) {
        	tbhDisable(topWheelCtrl);
        	tbhDisable(botWheelCtrl);
        }
        vexMotorSet(M_FLY_TOP_WHEEL, tbhUpdate(topWheelCtrl));
        vexMotorSet(M_FLY_BOT_WHEEL, tbhUpdate(botWheelCtrl));

        if(vexControllerGet(J_FEED_FRONT_U)) {
            vexMotorSet(M_FEED_FRONT, 100);
        } else if(vexControllerGet(J_FEED_FRONT_D)) {
            vexMotorSet(M_FEED_FRONT, -100);
        }  else if(motorRunning && !(isBallInner() && isBallOuter())) {
			vexMotorSet(M_FEED_FRONT, -DEFAULT_FEED_SPEED);
		} else {
            vexMotorSet(M_FEED_FRONT, 0);
        }

        if(vexControllerGet(J_FEED_SHOOT) || ballFeed) { // || (isBallInner() && !isBallOuter())) { //8D
            vexMotorSet(M_FEED_SHOOT, -80);
            vexMotorSet(M_FEED_FRONT, -80);
        }  else if(motorRunning && !(isBallInner() && isBallOuter())) {
            vexMotorSet(M_FEED_FRONT, -DEFAULT_FEED_SPEED);
		} else {
            vexMotorSet(M_FEED_SHOOT, 0);
            if(!(vexControllerGet(J_FEED_FRONT_D) || vexControllerGet(J_FEED_FRONT_U))) {
                vexMotorSet(M_FEED_FRONT, 0);
            }
        }

		if(vexControllerGet(J_FEED_REV)) {
            vexMotorSet(M_FEED_SHOOT, 127);
            vexMotorSet(M_FEED_FRONT, 127);
        }
        vexSleep( 10 );
	}

	return (msg_t)0;
}
