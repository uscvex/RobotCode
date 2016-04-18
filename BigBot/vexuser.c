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
#define J_SHOOT_OFF       Btn8D
#define J_SHOOT_BAR_MID   Btn8L
#define J_SHOOT_BAR_EDGE  Btn8U
#define J_SHOOT_MID       Btn8R

#define J_RAMP             Btn7D
#define J_FEED_RELEASE     Btn6U
#define J_FEED_RELEASE_OFF Btn6D
#define J_FEED_D           Btn5D
#define J_FEED_U           Btn5U

// Constants
#define FEED_SPEED 63

// fly wheel speeds
typedef struct _FlyWheelCalib {
    tCtlIndex button;
    int32_t speed;
    float gain;
    bool useTbh;
    double tbh;
} FlyWheelCalib;

#define CALIB_SIZE 4
static FlyWheelCalib flyWheelCalibration[] = {
    {J_SHOOT_MAX       , 12000, 0.05,  true, 0.3},
    {J_SHOOT_BAR_MID   , 6650,  0.1,   false, 0},
    {J_SHOOT_BAR_EDGE  , 7200,  0.2,   false, 0},
    {J_SHOOT_MID       , 8400,  0.1,   false, 0}
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
    { M_DRIVE_FRONT_RIGHT, kVexMotor393S, kVexMotorNormal,   kVexSensorNone, 0 },
    { M_DRIVE_BACK_LEFT,   kVexMotor393S, kVexMotorReversed, kVexSensorNone, 0 },
    { M_DRIVE_BACK_RIGHT,  kVexMotor393S, kVexMotorNormal,   kVexSensorNone, 0 },
    { M_DRIVE_FRONT_LEFT,  kVexMotor393S, kVexMotorNormal,   kVexSensorNone, 0 },
    { M_FEED,              kVexMotor393S, kVexMotorReversed, kVexSensorNone, 0 },
    { M_FLY_A,             kVexMotor393T, kVexMotorReversed, kVexSensorNone, 0 },
    { M_FLY_B,             kVexMotor393T, kVexMotorReversed, kVexSensorNone, 0 },
    { M_FLY_C,             kVexMotor393T, kVexMotorReversed, kVexSensorNone, 0 }
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
    flyWheelCtrl = TBHControllerInit(S_ENC_FLY, 0.01, 11000, true);
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
    bool feedReleaseOff = false;
    systime_t feedReleaseTime = 0;

    Debouncer dbncJFeedReleaseOff;
    debounceInit(&dbncJFeedReleaseOff, J_FEED_RELEASE_OFF, 50);

    //Speedometer *spdm = SpeedometerInit(S_ENC_FLY);
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

        //Calculate Motor Power
        xDriveMotors(
            VALLEY(vexControllerGet(J_DRIVE), 25, 127),
            VALLEY(vexControllerGet(J_STRAFE), 25, 127),
            VALLEY((int16_t)(vexControllerGet(J_TURN) * 0.70), 15, 127),
            M_DRIVE_FRONT_RIGHT,
            M_DRIVE_BACK_RIGHT,
            M_DRIVE_FRONT_LEFT,
            M_DRIVE_BACK_LEFT,
            25, 127
        );

        // Enable fly wheel
        for(i = 0;i < CALIB_SIZE;i++) {
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

        /* vex_printf("speed = %f, jostick=%d\n", SpeedometerUpdate(spdm), vexControllerGet(J_DRIVE)); */
        /* vexMotorSet(M_FLY_A, vexControllerGet(J_DRIVE)); */
        /* vexMotorSet(M_FLY_B, vexControllerGet(J_DRIVE)); */
        /* vexMotorSet(M_FLY_C, vexControllerGet(J_DRIVE)); */

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
            else if((currentTime - feedReleaseTime) > 250) {
                vexDigitalPinSet(P_FEED_RELEASE, 1);
            }
        }

        if(debounceKeyDown(&dbncJFeedReleaseOff)) {
            feedReleaseOff = !feedReleaseOff;
        }
        if(feedReleaseOff) {
            vexDigitalPinSet(P_FEED_RELEASE, 1);
        }

        //5U Feed In, 5D Feed Out
        if (vexControllerGet(J_FEED_D)){
            vexMotorSet(M_FEED, -FEED_SPEED);
        }
        else if (vexControllerGet(J_FEED_U) || (feedRelease && (currentTime - feedReleaseTime) > 500)){
            vexMotorSet(M_FEED, FEED_SPEED);
        } else {
            vexMotorSet(M_FEED, 0);
        }

        //Don't hog CPU
        vexSleep( 10 );
    }

    return (msg_t)0;
}
