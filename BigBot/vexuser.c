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
#define M_DRIVE_FRONT_RIGHT  kVexMotor_3
#define M_DRIVE_FRONT_LEFT   kVexMotor_4
#define M_DRIVE_BACK_LEFT    kVexMotor_7
#define M_DRIVE_BACK_RIGHT   kVexMotor_8

#define M_FLY_A              kVexMotor_2
#define M_FLY_B              kVexMotor_5
#define M_FLY_C              kVexMotor_6

#define M_FEED            kVexMotor_9

// Sensor channels
#define P_FEED_RELEASE        kVexDigital_4
#define P_RAMP_RELEASE        kVexDigital_3

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


//#define J_SHOOT      Btn6U
#define J_SHOOT_START   Btn8R
#define J_SHOOT_MID     Btn8U
#define J_SHOOT_CORNER  Btn7U
#define J_SHOOT_STOP    Btn8D

#define J_START_AUTON   Btn7R

#define J_SHOOT_LESS    Btn8L
#define J_FEED_RELEASE      Btn7D

#define J_RAMP_RELEASE  Btn6U
#define J_FEED_SHOOT_D  Btn6D
#define J_FEED_FRONT_D  Btn5D
#define J_FEED_FRONT_U  Btn5U

// Constants
#define DEFAULT_FEED_SPEED 127
#define FEED_SPOOL_TIME    100

#define FLY_CORNER_SPEED 7200
#define FLY_SHORT_SPEED  4500
#define FLY_START_SPEED  7000
#define FLY_MID_SPEED    6000
#define FLY_LESS_SPEED   6800

// Digi IO configuration
static  vexDigiCfg  dConfig[] = {
    { P_ENC_TOP_FLY_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_2},
    { P_ENC_TOP_FLY_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_2},

    { P_ENC_DRIVE_RIGHT_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_3},
    { P_ENC_DRIVE_RIGHT_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_3},

    { P_ENC_DRIVE_LEFT_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_4},
    { P_ENC_DRIVE_LEFT_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_4}
};

// Motor Config
static  vexMotorCfg mConfig[] = {
    { M_DRIVE_FRONT_RIGHT,    kVexMotor393S,           kVexMotorNormal,     kVexSensorIME,         0 },
    { M_DRIVE_BACK_LEFT,        kVexMotor393S,           kVexMotorReversed,     kVexSensorIME,         0 },
    { M_DRIVE_BACK_RIGHT,       kVexMotor393S,           kVexMotorNormal,       kVexSensorNone,        0 },
    { M_DRIVE_FRONT_LEFT,       kVexMotor393S,           kVexMotorNormal,       kVexSensorNone,        0 },
    { M_FEED,           kVexMotor393S,           kVexMotorReversed,     kVexSensorNone,        0 },
    { M_FLY_A,     kVexMotor393T,      kVexMotorReversed,     kVexSensorNone, 0 },
    { M_FLY_B,     kVexMotor393T,      kVexMotorReversed,     kVexSensorNone, 0 },
    { M_FLY_C,     kVexMotor393T,      kVexMotorReversed,     kVexSensorNone, 0 }
};

// TBH Controllers
TBHController *flyWheelCtrl;

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
    flyWheelCtrl->log = false;
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

    systime_t currentTime = 0;
    systime_t pneumaticPressed = 0;
    int32_t pneumaticTimeGap;
    bool ramp = true;

    //Run until asked to terminate
    while(!chThdShouldTerminate())
    {
        currentTime = chTimeNow();
        pneumaticTimeGap = currentTime - pneumaticPressed;
        //Stop timer for piston if the button is pressed
        if(!vexControllerGet(J_FEED_RELEASE))
            {
                pneumaticPressed = currentTime;
            }

        //Test autonomous
        if(vexControllerGet(J_START_AUTON))
            {
                vexAutonomous(NULL);
            }

        //Calculate Motor Power
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

        if(pneumaticTimeGap >= 250) {
            vexDigitalPinSet(P_FEED_RELEASE, 1);
        } else {
            vexDigitalPinSet(P_FEED_RELEASE, 0);
        }
        //Start position shot
        if(vexControllerGet(J_SHOOT_START)) {
            tbhEnableWithGain(flyWheelCtrl, FLY_START_SPEED, 0.04);
        }
        //3/4 court shot
        if(vexControllerGet(J_SHOOT_MID)) {
            tbhEnableWithGain(flyWheelCtrl, FLY_MID_SPEED,0.035);
        }
        //Full court shot
        if(vexControllerGet(J_SHOOT_CORNER)) {
            tbhEnableWithGain(flyWheelCtrl, FLY_CORNER_SPEED,0.0125);
        }
        if(vexControllerGet(J_SHOOT_LESS)) {
            tbhEnableWithGain(flyWheelCtrl, FLY_LESS_SPEED,0.04);
        }
        //Turn off flywheels
        if(vexControllerGet(J_SHOOT_STOP)) {
            tbhDisable(flyWheelCtrl);
        }
        //Activate/deactivate flywheel motors
        vexMotorSet(M_FLY_A, tbhUpdate(flyWheelCtrl));
        vexMotorSet(M_FLY_B, tbhUpdate(flyWheelCtrl));
        vexMotorSet(M_FLY_C, tbhUpdate(flyWheelCtrl));

        // Shoot Feed

        //if 6U then retract solenoid else deploy
        if(vexControllerGet(J_RAMP_RELEASE)) {
            vexMotorSet(M_FEED, 77);
            vexDigitalPinSet(P_RAMP_RELEASE, 0);
            ramp = false;
        }
        //5U Feed In, 5D Feed Out
        else if (vexControllerGet(J_FEED_FRONT_D)){
            vexMotorSet(M_FEED, -77);
        }
        else if (vexControllerGet(J_FEED_FRONT_U)){
            vexMotorSet(M_FEED, 77);
        }
        else{
            vexDigitalPinSet(P_RAMP_RELEASE, 1);
            ramp = true;
            vexMotorSet(M_FEED, 0);
        }

        if (vexControllerGet(J_FEED_SHOOT_D)){
            if (ramp){
                vexDigitalPinSet(P_RAMP_RELEASE, 0);
                ramp= false;
            }
            else{
                vexDigitalPinSet(P_RAMP_RELEASE, 1);
                ramp = true;
            }
        }

        //Don't hog CPU
        vexSleep( 10 );
    }

    return (msg_t)0;
}
