#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"
#include "../Common/takebackhalf.h"

// Motor config
#define M_FEED_FRONT         kVexMotor_2
#define M_DRIVE_FRONT_LEFT   kVexMotor_3
#define M_DRIVE_BACK_LEFT    kVexMotor_4

#define M_FLY_TOP            kVexMotor_6
#define M_FLY_BOT            kVexMotor_5

#define M_DRIVE_BACK_RIGHT   kVexMotor_7
#define M_DRIVE_FRONT_RIGHT  kVexMotor_9

#define M_DRIVE_SIDE_RIGHT   kVexMotor_1
#define M_DRIVE_SIDE_LEFT    kVexMotor_10

#define M_FEED_SHOOT         kVexMotor_8

#define PN_DEPLOY            kVexDigital_1

// Sensor channels
#define S_ENC_FLY_TOP   kVexSensorDigital_11
#define S_ENC_FLY_BOT   kVexSensorDigital_2

// Joystick settings
#define J_SHOOT_MAX     Btn8R
#define J_SHOOT_75      Btn8U
#define J_SHOOT_25      Btn8D
#define J_SHOOT_50      Btn8L

#define J_PN_DEPLOY     Btn7U

#define J_SHOOT_FEED_U    Btn5U
#define J_SHOOT_FEED_D    Btn5D

#define J_FEED_U    Btn6U
#define J_FEED_D    Btn6D
#define J_XDRIVE_X  Ch4
#define J_XDRIVE_Y  Ch3
#define J_XDRIVE_R  Ch1

// Constants
#define FEED_SPOOL_TIME 60
#define FEED_SHOOT_POWER 90
#define FEED_FRONT_POWER 127

// Digi IO configuration
static  vexDigiCfg  dConfig[kVexDigital_Num] = {
        { kVexDigital_1,    kVexSensorDigitalOutput, kVexConfigOutput,    0},
        { kVexDigital_2,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_2},
        { kVexDigital_3,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_2},
        { kVexDigital_11,   kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_1},
        { kVexDigital_12,   kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_1},
};

static  vexMotorCfg mConfig[kVexMotorNum] = {
        { kVexMotor_1,        kVexMotor393T,           kVexMotorReversed,       kVexSensorNone,        0 },
        { kVexMotor_2,        kVexMotor393T,           kVexMotorNormal,       kVexSensorNone,        0 },
        { kVexMotor_3,        kVexMotor393T,           kVexMotorNormal,       kVexSensorNone,        0 },
        { kVexMotor_4,        kVexMotor393T,           kVexMotorReversed,     kVexSensorNone,        0 },
        { kVexMotor_5,        kVexMotor393S,           kVexMotorNormal,       kVexSensorQuadEncoder, kVexQuadEncoder_2 },
        { kVexMotor_6,        kVexMotor393S,           kVexMotorNormal,       kVexSensorQuadEncoder, kVexQuadEncoder_1 },
        { kVexMotor_7,        kVexMotor393T,           kVexMotorReversed,     kVexSensorNone,        0 },
        { kVexMotor_8,        kVexMotor393T,           kVexMotorReversed,     kVexSensorNone,        0 },
        { kVexMotor_9,        kVexMotor393T,           kVexMotorReversed,     kVexSensorNone,        0 },
        { kVexMotor_10,       kVexMotor393T,      	   kVexMotorNormal,       kVexSensorNone,        0 },
};

// TBH Controllers
TBHController *topWheelCtrl;
TBHController *botWheelCtrl;

bool doMotorDrive(int16_t x, int16_t y, int16_t r) {
    // Y component, X component, Rotation
    int fl, fr, br, bl;
    fl = VALLEY(y + x + r, 45, 127);
    fr = VALLEY(y - x - r, 45, 127);
    br = VALLEY(y  + x - r, 45, 127);
    bl = VALLEY(-y + x - r, 45, 127);
    vexMotorSet(M_DRIVE_FRONT_LEFT,   fl);
    vexMotorSet(M_DRIVE_FRONT_RIGHT,  fr);
    vexMotorSet(M_DRIVE_BACK_RIGHT,   br);
    vexMotorSet(M_DRIVE_BACK_LEFT,    bl);
    if(r == 0) {
		vexMotorSet(M_DRIVE_SIDE_LEFT, y);
		vexMotorSet(M_DRIVE_SIDE_RIGHT, y);
    } else if (x == 0 && y == 0) {
		vexMotorSet(M_DRIVE_SIDE_LEFT, -r);
		vexMotorSet(M_DRIVE_SIDE_RIGHT, r);
    } else {
		vexMotorSet(M_DRIVE_SIDE_LEFT, 0);
		vexMotorSet(M_DRIVE_SIDE_RIGHT, 0);
    }
    return (fr || fl || br || bl);
}

void
vexUserSetup()
{
    vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
    vexMotorConfigure( mConfig, MOT_CONFIG_SIZE( mConfig ) );
}

msg_t
vexAutonomous( void *arg )
{
    (void)arg;

    // Must call this
    vexTaskRegister("auton");

	tbhEnable(botWheelCtrl, 6950);
	tbhEnable(topWheelCtrl, 6950);

	vex_printf("starting autonomous\n");
    int shootCount = 0;
    int step = 0;
    systime_t lastTime = chTimeNow();
    bool firstTime = true;
    while(1)
    {
    	systime_t time = chTimeNow();
    	int32_t timeGap = time-lastTime;

    	if((firstTime && step == 0 && timeGap > 3500) ||
    	   (!firstTime && step == 0 && timeGap > 2500)) {
			vexMotorSet(M_FEED_SHOOT, FEED_SHOOT_POWER);
			vexMotorSet(M_FEED_FRONT, FEED_FRONT_POWER);
			step++;
			lastTime = time;
    	} else if(step == 1 && timeGap > 1750) {
			vexMotorSet(M_FEED_SHOOT, -FEED_SHOOT_POWER);
			vexMotorSet(M_FEED_FRONT, -FEED_FRONT_POWER);
			step++;
			lastTime = time;
    	} else if(step == 2 && timeGap > 250) {
			vexMotorSet(M_FEED_SHOOT, 0);
			vexMotorSet(M_FEED_FRONT, 0);
			shootCount++;
			if(shootCount == 4) {
				step++;
			} else {
				step = 0;
			}
			lastTime = time;
    	} else if(step == 3 && timeGap > 3000) {
			tbhDisable(botWheelCtrl);
			tbhDisable(topWheelCtrl);
			step++;
    	} else if(step == 4) {
    		break;
    	}

    	firstTime = false;
        vexMotorSet(M_FLY_BOT, tbhUpdate(botWheelCtrl));
        vexMotorSet(M_FLY_TOP, tbhUpdate(topWheelCtrl));
        // Don't hog cpu
        vexSleep( 10 );
    }

    return (msg_t)0;
}


void
vexUserInit()
{
    topWheelCtrl = TBHControllerInit(S_ENC_FLY_TOP, 0.06, 9800, false);
    //topWheelCtrl->log = true;
    botWheelCtrl = TBHControllerInit(S_ENC_FLY_BOT, 0.045, 9800, false);
    //botWheelCtrl->log = true;
}

msg_t
vexOperator( void *arg )
{
	(void)arg;

	// Must call this
	vexTaskRegister("operator");


	// Run until asked to terminate
    int feedSpoolCounter = 0;
    int  motorRunningTime = 0;
	while(!chThdShouldTerminate())
	{
//		if(vexControllerGet(Btn5U)) {
//			vexControllerReleaseWait(Btn5U);
//			botWheelCtrl->gain += 0.005;
//			vex_printf("Gain = %f\n", botWheelCtrl->gain);
//		}
//		if(vexControllerGet(Btn5D)) {
//			vexControllerReleaseWait(Btn5D);
//			botWheelCtrl->gain -= 0.005;
//			vex_printf("Gain = %f\n", botWheelCtrl->gain);
//		}

        if(vexControllerGet(J_SHOOT_MAX)) {
        	tbhEnable(botWheelCtrl, 6950);
        	tbhEnable(topWheelCtrl, 6950);
        }
        if(vexControllerGet(J_SHOOT_75)) {
        	tbhEnable(botWheelCtrl, 6300);
        	tbhEnable(topWheelCtrl, 6300);
        }
        if(vexControllerGet(J_SHOOT_50)) {
        	tbhEnable(botWheelCtrl, 5500);
        	tbhEnable(topWheelCtrl, 5500);
        }
        if(vexControllerGet(J_SHOOT_25)) {
        	tbhDisable(botWheelCtrl);
        	tbhDisable(topWheelCtrl);
        }
        vexMotorSet(M_FLY_BOT, tbhUpdate(botWheelCtrl));
        vexMotorSet(M_FLY_TOP, tbhUpdate(topWheelCtrl));

        int x = VALLEY(vexControllerGet(J_XDRIVE_X), 45, 127);
        int y = VALLEY(vexControllerGet(J_XDRIVE_Y), 45, 127);
        int r = VALLEY(vexControllerGet(J_XDRIVE_R), 45, 127);

        if(vexControllerGet(J_PN_DEPLOY)) {
        	vexDigitalPinSet(PN_DEPLOY, kVexDigitalHigh);
        } else {
        	vexDigitalPinSet(PN_DEPLOY, kVexDigitalLow);
        }

        bool motorRunning = doMotorDrive(x, y, r);
        if(motorRunning) {
            motorRunningTime = 25;
        } else {
            motorRunningTime = MAX(motorRunningTime-1, 0);
        }
        if(motorRunningTime > 0) {
            motorRunning = true;
        }

        if(motorRunning || vexControllerGet(J_FEED_U)) {
            vexMotorSet(M_FEED_FRONT, FEED_FRONT_POWER);
        } else if(vexControllerGet(J_FEED_D)) {
            vexMotorSet(M_FEED_FRONT, -FEED_FRONT_POWER);
        } else {
            vexMotorSet(M_FEED_FRONT, 0);
        }
        if(vexControllerGet(J_SHOOT_FEED_U)) {
            vexMotorSet(M_FEED_SHOOT, FEED_SHOOT_POWER);
            vexMotorSet(M_FEED_FRONT, FEED_FRONT_POWER);
        } else if(vexControllerGet(J_SHOOT_FEED_D)) {
            vexMotorSet(M_FEED_SHOOT, -FEED_SHOOT_POWER);
            vexMotorSet(M_FEED_FRONT, -FEED_FRONT_POWER);
        } else {
            vexMotorSet(M_FEED_SHOOT, 0);
            if(!motorRunning && !vexControllerGet(J_FEED_D) && !vexControllerGet(J_FEED_U)) {
                vexMotorSet(M_FEED_FRONT, 0);
            }
        }
        vexSleep( 10 );
	}

	return (msg_t)0;
}
