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
#define M_DRIVE_RIGHT1    kVexMotor_3

#define M_FLY_BOT_WHEEL       kVexMotor_6
#define M_FLY_TOP_WHEEL      kVexMotor_5

#define M_FEED_FRONT      kVexMotor_2
#define M_DRIVE_RIGHT2    kVexMotor_7
#define M_DRIVE_LEFT1     kVexMotor_4
#define M_DRIVE_LEFT2     kVexMotor_8
#define M_FEED_SHOOT      kVexMotor_9

// Sensor channels
#define P_ENC_BOT_FLY_A       kVexDigital_1
#define P_ENC_BOT_FLY_B       kVexDigital_2

#define P_ENC_TOP_FLY_A       kVexDigital_3
#define P_ENC_TOP_FLY_B       kVexDigital_4

#define S_BALL_IN              0
#define S_BALL_OUT             1

#define S_ENC_BOT_FLY         kVexSensorDigital_2
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
#define FLY_PB_SPEED  5700
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
    { M_DRIVE_LEFT1,    kVexMotor393S,           kVexMotorNormal,     kVexSensorIME,         kImeChannel_2 },
    { M_DRIVE_RIGHT1,   kVexMotor393S,           kVexMotorReversed,     kVexSensorIME,         kImeChannel_1 },
    { M_FEED_SHOOT,     kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },

    { M_FLY_TOP_WHEEL,      kVexMotor393T,      kVexMotorReversed,   kVexSensorQuadEncoder, kVexQuadEncoder_1 },
    { M_FLY_BOT_WHEEL,     kVexMotor393T,      kVexMotorNormal,     kVexSensorQuadEncoder, kVexQuadEncoder_2 },

    { M_FEED_FRONT,     kVexMotor393S,           kVexMotorReversed,     kVexSensorNone,        0 },
    { M_DRIVE_LEFT2,    kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
    { M_DRIVE_RIGHT2,   kVexMotor393S,           kVexMotorReversed,     kVexSensorNone,        0 }
};


// TBH Controllers
TBHController *leftWheelCtrl;
TBHController *rightWheelCtrl;

EasingConfig *leftDriveEasing;
EasingConfig *rightDriveEasing;
pidController *leftDriveCtrl;
pidController *rightDriveCtrl;

bool driveMotors(void) {
    short ld, rd ;
    //Calculate Motor Power
    int forward = VALLEY(vexControllerGet(J_DRIVE), 45, 127);
    int turn    = VALLEY(vexControllerGet(J_TURN), 45, 127);
    ld = VALLEY(forward + turn, 45, 127);
    rd = VALLEY(forward - turn, 45, 127);

    rd = rd * 1.9;
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

void
vexUserSetup()
{
	vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
	vexMotorConfigure( mConfig, MOT_CONFIG_SIZE( mConfig ) );
}

void
vexUserInit()
{
//	leftDriveCtrl = PidControllerInit(1, 0, 0, S_IME_DRIVE_LEFT, false);
//	rightDriveCtrl = PidControllerInit(1, 0, 0, S_IME_DRIVE_RIGHT, false);
//	leftDriveEasing = easingInit(kMinJerk);
//	rightDriveEasing = easingInit(kMinJerk);

    rightWheelCtrl = TBHControllerInit(S_ENC_TOP_FLY, 0.05, 10500, false);
    rightWheelCtrl->powerZeroClamp = true;
    rightWheelCtrl->log = false;
    leftWheelCtrl = TBHControllerInit(S_ENC_BOT_FLY, 0.05, 10500, false);
    leftWheelCtrl->log = false;
    leftWheelCtrl->powerZeroClamp = true;
}

bool isLeftFlyWheelStable(void) {
	return (leftWheelCtrl->acceleration < 0.01);
}

bool isRightFlyWheelStable(void) {
	return (rightWheelCtrl->acceleration < 0.01);
}

msg_t
vexAutonomous( void *arg )
{
    (void)arg;

//	tbhEnable(leftWheelCtrl, FLY_MAX_SPEED);
//	tbhEnable(rightWheelCtrl, FLY_MAX_SPEED);

    //enableEasing(leftDriveEasing, 1000, vexSensorValueGet(S_IME_DRIVE_LEFT), vexSensorValueGet(S_IME_DRIVE_LEFT)+1000);
    //enableEasing(rightDriveEasing, 1000, vexSensorValueGet(S_IME_DRIVE_RIGHT), vexSensorValueGet(S_IME_DRIVE_RIGHT)+1000);

	vex_printf("starting autonomous\n");
	vexTaskRegister("auton");
    int shootCount = 0;
    int step = 0;
    systime_t lastTime = chTimeNow();
//    bool firstTime = true;
    while(!chThdShouldTerminate())
    {
    	systime_t time = chTimeNow();
    	int32_t timeGap = time-lastTime;
    	//vex_printf("right=%d, left=%d\n", vexSensorValueGet(S_IME_DRIVE_RIGHT), vexSensorValueGet(S_IME_DRIVE_LEFT));
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
//    	} else if(step == 2 && isRightFlyWheelStable() && isLeftFlyWheelStable()) {
//			vex_printf("step 2\n");
//			vexMotorSet(M_FEED_SHOOT, -DEFAULT_FEED_SPEED);
//			vexMotorSet(M_FEED_FRONT, -DEFAULT_FEED_SPEED);
//			step++;
//    	} else if (step == 3 && isBallOuter()) {
//			vex_printf("step 3\n");
//			step++;
//			lastTime = time;
//    	} else if (step == 4 && timeGap > 500) {
//			vex_printf("step 4\n");
//			step++;
//    	} else if(step == 5) {
//			vex_printf("step 5\n");
//			vexMotorSet(M_FEED_SHOOT, 0);
//			vexMotorSet(M_FEED_FRONT, 0);
//			step = 1;
//			shootCount++;
//			if(shootCount == 4) {
//				vexMotorSet(M_FLY_RIGHT_WHEEL, 0);
//				vexMotorSet(M_FLY_LEFT_WHEEL, 0);
//				break;
//			}
//    	}

//        vexMotorSet(M_FLY_RIGHT_WHEEL, tbhUpdate(rightWheelCtrl));
//        vexMotorSet(M_FLY_LEFT_WHEEL, tbhUpdate(leftWheelCtrl));

//        leftDriveCtrl->target_value = updateEasing(leftDriveEasing);
//        rightDriveCtrl->target_value = updateEasing(rightDriveEasing);
//        int16_t leftMotorCmd = PidControllerUpdate(rightDriveCtrl);
//        int16_t rightMotorCmd = PidControllerUpdate(leftDriveCtrl);
//        vexMotorSet(M_DRIVE_LEFT1, leftMotorCmd);
//        vexMotorSet(M_DRIVE_LEFT2, leftMotorCmd);
//        vexMotorSet(M_DRIVE_RIGHT1, rightMotorCmd);
//        vexMotorSet(M_DRIVE_RIGHT2, rightMotorCmd);

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
//	while(!chThdShouldTerminate()) {
//		if(vexControllerGet(Btn7D)) {
//			doVexAutonomous(NULL);
//		}
//	}
//	return (msg_t)0;

	// Must call this
	vexTaskRegister("operator");


	// Run until asked to terminate
    int  motorRunningTime = 0;
    bool ballFeed = false;
//    systime_t lastTime = chTimeNow();
//    int32_t lastValue = vexSensorValueGet(S_ENC_LEFT_FLY);
	while(!chThdShouldTerminate())
	{
//		int32_t value = vexSensorValueGet(S_ENC_LEFT_FLY);
//		systime_t currTime = chTimeNow();
//		double speed = (value - lastValue)/((double)(currTime - lastTime));
//		lastValue = value;
//		lastTime = currTime;
//		vex_printf("speed = %f\n", speed);

//		if(isBallInner()) {
//			ballFeed = true;
//		}
//		if(isBallOuter()) {
//			ballFeed = false;
//		}
		//vex_printf("bi=%d lbi=%d bo=%d lbo=%d bc=%d\n", ballInner, lastBallInner, ballOuter, lastBallOuter, ballCount);

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
        	tbhEnable(rightWheelCtrl, FLY_PB_SPEED);
        	tbhEnable(leftWheelCtrl, FLY_PB_SPEED);
        }
        if(vexControllerGet(J_SHOOT_MAX)) {
        	tbhEnable(rightWheelCtrl, FLY_MAX_SPEED);
        	tbhEnable(leftWheelCtrl, FLY_MAX_SPEED);
        }
        if(vexControllerGet(J_SHOOT_75)) {
        	tbhEnable(rightWheelCtrl, FLY_75_SPEED);
        	tbhEnable(leftWheelCtrl, FLY_75_SPEED);
        }
        if(vexControllerGet(J_SHOOT_SIDE)) {
        	tbhEnable(rightWheelCtrl, FLY_SIDE_SPEED);
        	tbhEnable(leftWheelCtrl, FLY_SIDE_SPEED);
        }
        if(vexControllerGet(J_SHOOT_STOP)) {
        	tbhDisable(rightWheelCtrl);
        	tbhDisable(leftWheelCtrl);
        }
        vexMotorSet(M_FLY_TOP_WHEEL, tbhUpdate(rightWheelCtrl));
        vexMotorSet(M_FLY_BOT_WHEEL, 0);
        //vexMotorSet(M_FLY_BOT_WHEEL, tbhUpdate(leftWheelCtrl));

        if(vexControllerGet(J_FEED_FRONT_U)) {
            vexMotorSet(M_FEED_FRONT, 100);
        } else if(vexControllerGet(J_FEED_FRONT_D)) {
            vexMotorSet(M_FEED_FRONT, -100);
        }  else if(motorRunning && !(isBallInner() && isBallOuter())) {
			vexMotorSet(M_FEED_FRONT, -DEFAULT_FEED_SPEED);
		} else {
            vexMotorSet(M_FEED_FRONT, 0);
        }

        if(vexControllerGet(J_FEED_SHOOT)) { // || (isBallInner() && !isBallOuter())) { //8D
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
