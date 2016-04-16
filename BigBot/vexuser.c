#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"
#include "../Common/takebackhalf.h"
#include "../Common/easing.h"
#include "../Common/mousedeadreck.h"

// Motor configs
#define M_DRIVE_FRONT_RIGHT  kVexMotor_3
#define M_DRIVE_FRONT_LEFT   kVexMotor_4
#define M_DRIVE_BACK_LEFT    kVexMotor_7
#define M_DRIVE_BACK_RIGHT   kVexMotor_8

#define M_FLY_A              kVexMotor_2
#define M_FLY_B              kVexMotor_5
#define M_FLY_C              kVexMotor_6

#define M_FEED            kVexMotor_9

// Sensor channels
#define P_RAMP        kVexDigital_4
#define P_FEED_RELEASE        kVexDigital_3

#define P_ENC_TOP_FLY_A       kVexDigital_1
#define P_ENC_TOP_FLY_B       kVexDigital_2

#define P_ENC_DRIVE_RIGHT_A   kVexDigital_7
#define P_ENC_DRIVE_RIGHT_B   kVexDigital_8

#define P_ENC_DRIVE_LEFT_A    kVexDigital_5
#define P_ENC_DRIVE_LEFT_B    kVexDigital_6

#define S_COLOR_SELECTOR        2

#define S_ENC_FLY             kVexSensorDigital_1
#define S_ENC_DRIVE_RIGHT     kVexSensorDigital_8
#define S_ENC_DRIVE_LEFT      kVexSensorDigital_6

// Joystick settings
#define J_TURN       Ch1
#define J_DRIVE      Ch3
#define J_STRAFE     Ch4

#define J_SHOOT_MAX       Btn7L
#define J_SHOOT_SIDE      Btn7U
#define J_SHOOT_OFF       Btn7R
#define J_SHOOT_BAR_MID   Btn8L
#define J_SHOOT_BAR_EDGE  Btn8U
#define J_SHOOT_MID       Btn8R
#define J_SHOOT_START     Btn8D

#define J_RAMP            Btn7D
#define J_FEED_RELEASE    Btn6U
#define J_FEED_D          Btn5D
#define J_FEED_U          Btn5U

// Constants
#define DEFAULT_FEED_SPEED 127
#define FEED_SPOOL_TIME    100

// fly wheel speeds
typedef struct _FlyWheelCalib {
    tCtlIndex button;
    int32_t speed;
    float gain;
    bool useTbh;
    double tbh;
} FlyWheelCalib;

static FlyWheelCalib flyWheelCalibration[] = {
    {J_SHOOT_MAX       , 5000, 0.01,   false, 0},
    {J_SHOOT_SIDE      , 5000, 0.01,   false, 0},
    {J_SHOOT_BAR_MID   , 6250, 0.00005,  false, 0},
    {J_SHOOT_BAR_EDGE  , 6300, 0.00005,  false, 0},
    {J_SHOOT_MID       , 5000, 0.01,   false, 0},
    {J_SHOOT_START     , 10000, 0.00005, true,  0.4}
};

// Digi IO configuration
static  vexDigiCfg  dConfig[] = {
    { P_ENC_TOP_FLY_A, kVexSensorQuadEncoder,  kVexConfigQuadEnc1, kVexQuadEncoder_2 },
    { P_ENC_TOP_FLY_B, kVexSensorQuadEncoder,  kVexConfigQuadEnc2, kVexQuadEncoder_2 },

    { P_ENC_DRIVE_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1, kVexQuadEncoder_3 },
    { P_ENC_DRIVE_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2, kVexQuadEncoder_3 },

    { P_ENC_DRIVE_LEFT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1, kVexQuadEncoder_4 },
    { P_ENC_DRIVE_LEFT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2, kVexQuadEncoder_4 }
};

// Motor Config
static  vexMotorCfg mConfig[] = {
<<<<<<< HEAD
  { M_DRIVE_FRONT_RIGHT,    kVexMotor393S,           kVexMotorNormal,    	  kVexSensorIME,         0 },
  { M_DRIVE_BACK_LEFT, 	    kVexMotor393S,           kVexMotorReversed,     kVexSensorIME,         0 },
  { M_DRIVE_BACK_RIGHT,    	kVexMotor393S,           kVexMotorNormal,     	kVexSensorNone,        0 },
  { M_DRIVE_FRONT_LEFT,    	kVexMotor393S,           kVexMotorNormal,     	kVexSensorNone,        0 },

  { M_FEED_SHOOT,           kVexMotor393S,           kVexMotorReversed,     kVexSensorNone,        0 },

  { M_FLY_A,     kVexMotor393T,      kVexMotorReversed,     kVexSensorNone, 0 },
  { M_FLY_B,     kVexMotor393T,      kVexMotorReversed,     kVexSensorNone, 0 },
  { M_FLY_C,     kVexMotor393T,      kVexMotorReversed,     kVexSensorNone, 0 }

  //Motors

=======
    { M_DRIVE_FRONT_RIGHT, kVexMotor393S, kVexMotorNormal,   kVexSensorNone, 0 },
    { M_DRIVE_BACK_LEFT,   kVexMotor393S, kVexMotorReversed, kVexSensorNone, 0 },
    { M_DRIVE_BACK_RIGHT,  kVexMotor393S, kVexMotorNormal,   kVexSensorNone, 0 },
    { M_DRIVE_FRONT_LEFT,  kVexMotor393S, kVexMotorNormal,   kVexSensorNone, 0 },
    { M_FEED,              kVexMotor393S, kVexMotorReversed, kVexSensorNone, 0 },
    { M_FLY_A,             kVexMotor393T, kVexMotorReversed, kVexSensorNone, 0 },
    { M_FLY_B,             kVexMotor393T, kVexMotorReversed, kVexSensorNone, 0 },
    { M_FLY_C,             kVexMotor393T, kVexMotorReversed, kVexSensorNone, 0 }
>>>>>>> 8230ebbd7c17f0aa156b4017edba7866859b83a5
};

// TBH Controllers
TBHController *flyWheelCtrl;

// Dead Reckoning Controller
DeadReck *dreck;

void
vexUserSetup()
{
    vexDigitalPinSet(kVexDigital_5, 0);
    vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
    vexMotorConfigure( mConfig, MOT_CONFIG_SIZE( mConfig ) );
}

void
vexUserInit()
{
    //Initialize TBHControllers
    flyWheelCtrl = TBHControllerInit(S_ENC_FLY, 0.01, 10500, true);
    flyWheelCtrl->powerZeroClamp = true;
    flyWheelCtrl->log = true;

    // Initialize Dead Reckoning Controller
    dreck = deadReckInit(&SD3, 115200);
}

bool isRed(void) {
    return (vexAdcGet(S_COLOR_SELECTOR) > 2000);
}

msg_t
vexAutonomous( void *arg )
{
    (void)arg;
    vexTaskRegister("auton");
    while(!chThdShouldTerminate());
    return (msg_t)0;
}

msg_t
//User control settings
vexOperator( void *arg )
{
    (void)arg;
    vexTaskRegister("operator");

    int i;
    systime_t currentTime = 0;
    systime_t rampReleaseTime = 0;
    bool feedRelease = false;
    systime_t feedReleaseTime = 0;

    deadReckStart(dreck);

    //Run until asked to terminate
    while(!chThdShouldTerminate()) {
        deadReckUpdate(dreck);
        //vex_printf("botX = %f, botY = %f, botTheta = %f\n", dreck->botX, dreck->botY, dreck->botTheta);

        currentTime = chTimeNow();
        //Stop timer for piston if the button is pressed
        if(!vexControllerGet(J_RAMP)) {
            rampReleaseTime = currentTime;
        }
        if((currentTime - rampReleaseTime) >= 250) {
            vexDigitalPinSet(P_RAMP, 1);
        } else {
            vexDigitalPinSet(P_RAMP, 0);
        }

        /* //Calculate Motor Power */
        xDriveMotors(
            vexControllerGet(J_DRIVE),
            vexControllerGet(J_STRAFE),
            vexControllerGet(J_TURN),
            M_DRIVE_FRONT_RIGHT,
            M_DRIVE_BACK_RIGHT,
            M_DRIVE_FRONT_LEFT,
            M_DRIVE_BACK_LEFT,
            25, 127, 25, 127
        );

        // Enable fly wheel
        for(i = 0;i < 6;i++) {
            FlyWheelCalib *calib = &(flyWheelCalibration[i]);
            if(vexControllerGet(calib->button)) {
                if(calib->useTbh) {
                    tbhEnableWithGainTBH(flyWheelCtrl, calib->speed, calib->gain, calib->tbh);
                }
                else {
                    tbhEnableWithGain(flyWheelCtrl, calib->speed, calib->gain);
                }
                break;
            }
        }
        //Turn off flywheels
        if(vexControllerGet(J_SHOOT_OFF)) {
            tbhDisable(flyWheelCtrl);
        }
        //Activate/deactivate flywheel motors
        int16_t flyMotor = CLAMP(tbhUpdate(flyWheelCtrl), 0, 90);
        vexMotorSet(M_FLY_A, flyMotor);
        vexMotorSet(M_FLY_B, flyMotor);
        vexMotorSet(M_FLY_C, flyMotor);

        /* if(!flyWheelCtrl->enabled) { */
        /*     vex_printf("%d\n", vexControllerGet(J_DRIVE)); */
        /*     vexMotorSet(M_FLY_A, vexControllerGet(J_DRIVE)); */
        /*     vexMotorSet(M_FLY_B, vexControllerGet(J_DRIVE)); */
        /*     vexMotorSet(M_FLY_C, vexControllerGet(J_DRIVE)); */
        /* } */


        // Shoot Feed

        //if 6U then retract solenoid else deploy
        if(vexControllerGet(J_FEED_RELEASE)) {
            vexDigitalPinSet(P_FEED_RELEASE, 0);
            if(!feedRelease) {
                feedReleaseTime = currentTime;
                feedRelease = true;
            }
        }
        else {
            if(feedRelease) {
                feedRelease = false;
                feedReleaseTime = currentTime;
            }
            else if((currentTime - feedReleaseTime) > 500) {
                vexDigitalPinSet(P_FEED_RELEASE, 1);
            }
        }

        //5U Feed In, 5D Feed Out
        if (vexControllerGet(J_FEED_D)){
            vexMotorSet(M_FEED, -77);
        }
        else if (vexControllerGet(J_FEED_U) || (feedRelease && (currentTime - feedReleaseTime) > 500)){
            vexMotorSet(M_FEED, 77);
        } else {
            vexMotorSet(M_FEED, 0);
        }

        //Don't hog CPU
        vexSleep( 10 );
    }
<<<<<<< HEAD
    if(vexControllerGet(J_SHOOT_LESS)) {
       	tbhEnableWithGain(topWheelCtrl, FLY_LESS_SPEED,0.04);
//       	tbhEnableWithGain(botWheelCtrl, FLY_LESS_SPEED,0.04);
       }

    //Turn off flywheels
    if(vexControllerGet(J_SHOOT_STOP)) {
      tbhDisable(topWheelCtrl);7n
//      tbhDisable(botWheelCtrl);
    }
    //Activate/deactivate flywheel motors
    vexMotorSet(M_FLY_A, tbhUpdate(topWheelCtrl));
    vexMotorSet(M_FLY_B, tbhUpdate(topWheelCtrl));
    vexMotorSet(M_FLY_C, tbhUpdate(topWheelCtrl));

    if(isBallBot())
    {
    	botSensorTime = chTimeNow();
    }
    // Front Feed Controls
//    if( vexControllerGet(J_FEED_FRONT_U) || vexControllerGet(J_FEED_SHOOT_U) || motorRunning) {
//       vexMotorSet(M_FEED_FRONT, 63);
//    } else if(vexControllerGet(J_FEED_FRONT_D) || vexControllerGet(J_FEED_SHOOT_D)) {
//       vexMotorSet(M_FEED_FRONT, -63);
//    } else {
//       vexMotorSet(M_FEED_FRONT, 0);
//    }


    // Shoot Feed

    //if 6U then retract solenoid else deploy
    if(vexControllerGet(J_FEED_SOLE_U)) {
    	   vexMotorSet(M_FEED_SHOOT, 77);
    	   vexDigitalPinSet(P_SOLENOID, 0);
    	   sol = false;
    }
    //5U Feed In, 5D Feed Out
    else if (vexControllerGet(J_FEED_FRONT_D)){
    	vexMotorSet(M_FEED_SHOOT, -77);
    }
    else if (vexControllerGet(J_FEED_FRONT_U)){
    	vexMotorSet(M_FEED_SHOOT, 77);
    }
    else{
    	vexDigitalPinSet(P_SOLENOID, 1);
    	sol = true;
    	vexMotorSet(M_FEED_SHOOT, 0);
    }

    if (vexControllerGet(J_FEED_SHOOT_D)){
    	if (sol){
    		vexDigitalPinSet(P_SOLENOID, 0);
    		sol= false;
    	}
    	else{
    		vexDigitalPinSet(P_SOLENOID, 1);
    		sol = true;
    	}

    }


    //Don't hog CPU
    vexSleep( 10 );
  }
=======
>>>>>>> 8230ebbd7c17f0aa156b4017edba7866859b83a5

    return (msg_t)0;
}
