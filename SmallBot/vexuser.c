#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"
#include "../Common/takebackhalf.h"

// Motor configs
#define M_DRIVE_RIGHT1    kVexMotor_3

#define M_FLY_LEFT_WHEEL       kVexMotor_6
#define M_FLY_RIGHT_WHEEL      kVexMotor_5

#define M_FEED_FRONT      kVexMotor_2
#define M_DRIVE_RIGHT2    kVexMotor_7
#define M_DRIVE_LEFT1     kVexMotor_4
#define M_DRIVE_LEFT2     kVexMotor_8
#define M_FEED_SHOOT      kVexMotor_9

// Sensor channels
#define P_ENC_LEFT_FLY_A       kVexDigital_1
#define P_ENC_LEFT_FLY_B       kVexDigital_2

#define P_ENC_RIGHT_FLY_A       kVexDigital_3
#define P_ENC_RIGHT_FLY_B       kVexDigital_4

#define S_BALL_IN              1
#define S_BALL_OUT             2

#define S_ENC_LEFT_FLY         kVexSensorDigital_2
#define S_ENC_RIGHT_FLY        kVexSensorDigital_4

#define S_IME_DRIVE_RIGHT kVexSensorIme_3
#define S_IME_DRIVE_LEFT  kVexSensorIme_2
#define S_IME_FEED_SHOOT  kVexSensorIme_1

// Joystick settings
#define J_DRIVE      Ch3
#define J_TURN       Ch1
//#define J_SHOOT      Btn6U

#define J_SHOOT_MAX     Btn7R
#define J_SHOOT_75      Btn7U
#define J_SHOOT_25      Btn7D
#define J_SHOOT_50      Btn7L

#define J_FEED_SHOOT Btn6U
#define J_FEED_REV   Btn6D
#define J_FEED_FRONT_U Btn5D
#define J_FEED_FRONT_D Btn5U

// Constants
#define DEFAULT_FEED_SPEED 90
#define FEED_SPOOL_TIME 100

// Digi IO configuration
static  vexDigiCfg  dConfig[] = {
        { P_ENC_LEFT_FLY_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_1},
        { P_ENC_LEFT_FLY_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_1},

        { P_ENC_RIGHT_FLY_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_2},
        { P_ENC_RIGHT_FLY_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_2}
};

// Motor Config
static  vexMotorCfg mConfig[] = {
    { M_DRIVE_LEFT1,    kVexMotor393S,           kVexMotorNormal,     kVexSensorIME,         kImeChannel_2 },
    { M_DRIVE_RIGHT1,   kVexMotor393S,           kVexMotorNormal,     kVexSensorIME,         kImeChannel_1 },
    { M_FEED_SHOOT,     kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },

    { M_FLY_LEFT_WHEEL,      kVexMotor393T,      kVexMotorReversed,   kVexSensorQuadEncoder, kVexQuadEncoder_1 },
    { M_FLY_RIGHT_WHEEL,     kVexMotor393T,      kVexMotorNormal,     kVexSensorQuadEncoder, kVexQuadEncoder_2 },

    { M_FEED_FRONT,     kVexMotor393S,           kVexMotorReversed,     kVexSensorNone,        0 },
    { M_DRIVE_LEFT2,    kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
    { M_DRIVE_RIGHT2,   kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 }
};


// TBH Controllers
TBHController *leftWheelCtrl;
TBHController *rightWheelCtrl;

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


void
vexUserSetup()
{
	vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
	vexMotorConfigure( mConfig, MOT_CONFIG_SIZE( mConfig ) );
}

void
vexUserInit()
{
    rightWheelCtrl = TBHControllerInit(S_ENC_RIGHT_FLY, 0.05, 10080, false);
    rightWheelCtrl->powerZeroClamp = true;
    rightWheelCtrl->log = true;
    leftWheelCtrl = TBHControllerInit(S_ENC_LEFT_FLY, 0.05, 10080, false);
    leftWheelCtrl->log = true;
    leftWheelCtrl->powerZeroClamp = true;
}

msg_t
vexAutonomous( void *arg )
{
    (void)arg;

	tbhEnable(leftWheelCtrl, 7250);
	tbhEnable(rightWheelCtrl, 7250);

	vex_printf("starting autonomous\n");
    int shootCount = 0;
    int step = 0;
    systime_t lastTime = chTimeNow();
    bool firstTime = true;
    while(1)
    {
    	systime_t time = chTimeNow();
    	int32_t timeGap = time-lastTime;

    	if((firstTime && step == 0 && timeGap > 4600) ||
    	   (!firstTime && step == 0 && timeGap > 3300)) {
    		vex_printf("Step 0\n");
			vexMotorSet(M_FEED_SHOOT, -127);
			vexMotorSet(M_FEED_FRONT, -127);
			step++;
			lastTime = time;
    	} else if(step == 1 && timeGap > 1200) {
    		vex_printf("Step 1\n");
			vexMotorSet(M_FEED_SHOOT, 127);
			vexMotorSet(M_FEED_FRONT, 127);
			step++;
			lastTime = time;
    	} else if(step == 2 && timeGap > 150) {
    		vex_printf("Step 2\n");
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
    		vex_printf("Step 3\n");
			tbhDisable(leftWheelCtrl);
			tbhDisable(rightWheelCtrl);
			step++;
    	} else if(step == 4) {
    		vex_printf("Step 4\n");
			vexMotorSet(M_DRIVE_LEFT1,  100);
			vexMotorSet(M_DRIVE_LEFT2,  100);
			vexMotorSet(M_DRIVE_RIGHT1, 100);
			vexMotorSet(M_DRIVE_RIGHT2, 100);
			step++;
			lastTime = time;
    	} else if(step == 5 && timeGap > 1000) {
    		vex_printf("Step 5\n");
			vexMotorSet(M_DRIVE_LEFT1,  0);
			vexMotorSet(M_DRIVE_LEFT2,  0);
			vexMotorSet(M_DRIVE_RIGHT1, 0);
			vexMotorSet(M_DRIVE_RIGHT2, 0);
			break;
    	}

    	firstTime = false;
        vexMotorSet(M_FLY_RIGHT_WHEEL, tbhUpdate(rightWheelCtrl));
        vexMotorSet(M_FLY_LEFT_WHEEL, tbhUpdate(leftWheelCtrl));
        // Don't hog cpu
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

//	while(!chThdShouldTerminate()) {
//		vex_printf("encoder left = %d ", vexSensorValueGet(S_ENC_LEFT_FLY));
//		vex_printf("encoder right = %d\n", vexSensorValueGet(S_ENC_RIGHT_FLY));
//	}

	// Run until asked to terminate
    int  motorRunningTime = 0;
	while(!chThdShouldTerminate())
	{
        bool motorRunning = driveMotors();
        if(motorRunning) {
            motorRunningTime = 25;
        } else {
            motorRunningTime = MAX(motorRunningTime-1, 0);
        }
        if(motorRunningTime > 0) {
            motorRunning = true;
        }

        if(vexControllerGet(J_SHOOT_50)) {
        	tbhEnable(rightWheelCtrl, 6000);
        	tbhEnable(leftWheelCtrl, 6000);
        }
        if(vexControllerGet(J_SHOOT_MAX)) {
        	tbhEnable(rightWheelCtrl, 7150);
        	tbhEnable(leftWheelCtrl, 7150);
        }
        if(vexControllerGet(J_SHOOT_75)) {
        	tbhEnable(rightWheelCtrl, 6600);
        	tbhEnable(leftWheelCtrl, 6600);
        }
        if(vexControllerGet(J_SHOOT_25)) {
        	tbhDisable(rightWheelCtrl);
        	tbhDisable(leftWheelCtrl);
        }
        vexMotorSet(M_FLY_RIGHT_WHEEL, tbhUpdate(rightWheelCtrl));
        vexMotorSet(M_FLY_LEFT_WHEEL, tbhUpdate(leftWheelCtrl));

        if(vexControllerGet(J_FEED_FRONT_U)) {
            vexMotorSet(M_FEED_FRONT, 100);
        } else if(vexControllerGet(J_FEED_FRONT_D)) {
            vexMotorSet(M_FEED_FRONT, -100);
        }  else if(motorRunning) {
			vexMotorSet(M_FEED_FRONT, -DEFAULT_FEED_SPEED);
		} else {
            vexMotorSet(M_FEED_FRONT, 0);
        }

        if(vexControllerGet(J_FEED_SHOOT)) { //8D
            vexMotorSet(M_FEED_SHOOT, -127);
            vexMotorSet(M_FEED_FRONT, -127);
        } else if(motorRunning) {
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
