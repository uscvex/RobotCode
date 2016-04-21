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
#define P_RAMP        kVexDigital_4
#define P_FEED_RELEASE        kVexDigital_3

#define P_ENC_TOP_FLY_A       kVexDigital_1
#define P_ENC_TOP_FLY_B       kVexDigital_2

#define P_ENC_DRIVE_RIGHT_A   kVexDigital_7
#define P_ENC_DRIVE_RIGHT_B   kVexDigital_8

#define P_ENC_DRIVE_LEFT_A    kVexDigital_5
#define P_ENC_DRIVE_LEFT_B    kVexDigital_6

#define P_ENC_DRIVE_BACK_A    kVexDigital_9
#define P_ENC_DRIVE_BACK_B    kVexDigital_10

#define S_COLOR_SELECTOR        6

#define S_ENC_FLY             kVexSensorDigital_1
#define S_ENC_DRIVE_RIGHT     kVexSensorDigital_8
#define S_ENC_DRIVE_LEFT      kVexSensorDigital_6
#define S_ENC_DRIVE_BACK      kVexSensorDigital_10

// Joystick settings
#define J_TURN       Ch1
#define J_DRIVE      Ch3
#define J_STRAFE     Ch4

#define J_SHOOT_MAX       Btn7L
#define J_SHOOT_OFF       Btn8D
#define J_SHOOT_BAR_MID   Btn8L
#define J_SHOOT_BAR_EDGE  Btn8U
#define J_SHOOT_MID       Btn8R
#define J_AUTON           Btn7R
#define J_AUTON_STOP       Btn7L

#define J_RAMP             Btn7D
#define J_FEED_RELEASE     Btn6U
#define J_FEED_RELEASE_OFF Btn6D
#define J_FEED_D           Btn5D
#define J_FEED_U           Btn5U

// Constants
#define FEED_SPEED 63
#define FEED_MAX_SPEED 127

// fly wheel speeds
typedef struct _FlyWheelCalib {
    tCtlIndex button;
    int32_t speed;
    float gain;
    bool useTbh;
    double tbh;
} FlyWheelCalib;

#define CALIB_SIZE 4
#define AUTON_DRIVE_SPEED 275
#define AUTON_TURN_SPEED 550

#define MAX_FEED_RELEASE_COUNT 10

#define FLY_MAX      0
#define FLY_BAR_MID  1
#define FLY_BAR_EDGE 2
#define FLY_MID      3
static FlyWheelCalib flyWheelCalibration[] = {
    {J_SHOOT_MAX       , 12000, 0.05,  true,  0.3},
    {J_SHOOT_BAR_MID   , 6600,  0.1,   false, 0},
    {J_SHOOT_BAR_EDGE  , 7200,  0.2,   false, 0},
    {J_SHOOT_MID       , 8700,  0.1,   false, 0}
};

// Digi IO configuration
static  vexDigiCfg  dConfig[] = {
    { P_ENC_TOP_FLY_A, kVexSensorQuadEncoder,  kVexConfigQuadEnc1, kVexQuadEncoder_2 },
    { P_ENC_TOP_FLY_B, kVexSensorQuadEncoder,  kVexConfigQuadEnc2, kVexQuadEncoder_2 },

    { P_ENC_DRIVE_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1, kVexQuadEncoder_3 },
    { P_ENC_DRIVE_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2, kVexQuadEncoder_3 },

    { P_ENC_DRIVE_LEFT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1, kVexQuadEncoder_4 },
    { P_ENC_DRIVE_LEFT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2, kVexQuadEncoder_4 },

    { P_ENC_DRIVE_BACK_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1, kVexQuadEncoder_5 },
    { P_ENC_DRIVE_BACK_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2, kVexQuadEncoder_5 }
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

// PID Controllers
EPidController *forwardPid;
EPidController *turnPid;
EPidController *strafePid;

void enableFlyWheel(int calibIndex) {
    FlyWheelCalib *calib = &(flyWheelCalibration[calibIndex]);
    if(calib->useTbh) {
        tbhEnableWithGainTBH(flyWheelCtrl, calib->speed, calib->gain, calib->tbh);
    }
    else {
        tbhEnableWithGain(flyWheelCtrl, calib->speed, calib->gain);
    }
}

void clearDriveEncoders(void) {
    vexSensorValueSet(S_ENC_DRIVE_LEFT, 0);
    vexSensorValueSet(S_ENC_DRIVE_RIGHT, 0);
    vexSensorValueSet(S_ENC_DRIVE_BACK, 0);
}

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
    flyWheelCtrl->log = false;

    // Initialize PID
    forwardPid = EPidInit(kFlat, 0.01, 0, 0.001, -1, false);
    turnPid    = EPidInit(kFlat, 0.01, 0, 0.05, -1, false);
    strafePid  = EPidInit(kFlat, 0.01, 0, 0, -1, false);
    turnPid->log = true;
}

bool isRed(void) {
    return (vexAdcGet(S_COLOR_SELECTOR) < 2000);
}

int32_t getForwardValue(void) {
    return -(vexSensorValueGet(S_ENC_DRIVE_LEFT) + vexSensorValueGet(S_ENC_DRIVE_RIGHT))/2;
}

int32_t getStrafeValue(void) {
    return vexSensorValueGet(S_ENC_DRIVE_BACK);
}

int32_t leftDriveLastValue = 0;
int32_t rightDriveLastValue = 0;
int32_t getTurnValue(void) {
    int32_t leftDrive = -vexSensorValueGet(S_ENC_DRIVE_LEFT) - leftDriveLastValue;
    int32_t rightDrive = -vexSensorValueGet(S_ENC_DRIVE_RIGHT) - rightDriveLastValue;
    return leftDrive-rightDrive;
}

void xDrivePidEnable(int forward, int strafe, int turn, int32_t duration, int enableForward, int enableStrafe, int enableTurn) {
    vex_printf("xdrive pid enable\n");
    leftDriveLastValue = -vexSensorValueGet(S_ENC_DRIVE_LEFT);
    rightDriveLastValue = -vexSensorValueGet(S_ENC_DRIVE_RIGHT);
    if(enableForward) {
        EPidEnableWithValue(forwardPid, duration, forward, getForwardValue());
    }
    else {
        EPidDisable(forwardPid);
    }

    if(enableStrafe) {
        EPidEnableWithValue(strafePid, duration, strafe, getStrafeValue());
    }
    else {
        EPidDisable(strafePid);
    }

    if(enableTurn) {
        vex_printf("Enabling turn");
        EPidEnableWithValue(turnPid, duration, turn, getTurnValue());
    }
    else {
        EPidDisable(turnPid);
    }
}

void disableDrivePids(void) {
    EPidDisable(forwardPid);
    EPidDisable(strafePid);
    EPidDisable(turnPid);
}

void xDrivePidUpdate(void) {
    EPidUpdateWithValue(forwardPid, getForwardValue());
    EPidUpdateWithValue(strafePid, getStrafeValue());
    EPidUpdateWithValue(turnPid, getTurnValue());
}

int autonStep = 0;
int autonTurn = 1;
int feedReleaseCount = 0;
systime_t autonTime;
systime_t autonLastTime;
systime_t autonWaitTime;
#define STEP(s) (autonStep == (s) && (autonTime-autonLastTime) > autonWaitTime)
#define WAIT(t) do {autonLastTime = autonTime;autonWaitTime = (t);} while(false);
#define TIMEELAPSED(t) ((autonTime-autonLastTime) > (t))

int enableFeed(int startStep, int16_t value) {
    if(STEP(startStep)) {
        vexMotorSet(M_FEED, value);
        autonStep++;
    }
    return (startStep+1);
}

int move(int startStep, int target, float speed) {
    if(STEP(startStep)) {
        vex_printf("Move\n");
        int32_t duration = (ABS(target)/speed)*1000;
        int32_t waitTime = duration * 2;
        forwardPid->easing->func = kMinJerk;
        strafePid->easing->func = kFlat;
        turnPid->easing->func = kMinJerk;
        xDrivePidEnable(target, 0, 0, duration, true, false, true);
        autonStep++;
        WAIT(waitTime);
    }
    if(STEP(startStep+1)){
        disableDrivePids();
        autonStep++;
    }
    return (startStep+2);
}

int turn(int startStep, int target, float speed) {
    target *= 2;
    if(STEP(startStep)) {
        vex_printf("Turn\n");
        int32_t duration = (ABS(target)/speed)*1000;
        int32_t waitTime = duration * 3;
        forwardPid->easing->func = kFlat;
        strafePid->easing->func = kFlat;
        turnPid->easing->func = kMinJerk;
        xDrivePidEnable(0, 0, target, duration, false, false, true);
        autonStep++;
        WAIT(waitTime);
    }
    if(STEP(startStep+1)){
        disableDrivePids();
        autonStep++;
    }
    return (startStep+2);
}

int strafe(int startStep, int target, float speed) {
    if(STEP(startStep)) {
        vex_printf("strafe\n");
        int32_t duration = (ABS(target)/speed)*1000;
        int32_t waitTime = duration * 2;
        forwardPid->easing->func = kFlat;
        strafePid->easing->func = kMinJerk;
        turnPid->easing->func = kFlat;
        xDrivePidEnable(0, target, 0, duration, false, true, false);
        autonStep++;
        WAIT(waitTime);
    }
    if(STEP(startStep+1)){
        disableDrivePids();
        autonStep++;
    }
    return (startStep+2);
}

int rampDeploy(int startStep) {
    if(STEP(startStep)) {
        vexDigitalPinSet(P_RAMP, 1);
        WAIT(1000);
        autonStep++;
    }
    if(STEP(startStep + 1)) {
        vexDigitalPinSet(P_RAMP, 0);
        autonStep++;
    }
    return (startStep+1);
}

int shootBalls(int startStep, int duration) {
    if(STEP(startStep)) {
        vex_printf("Feed release\n");
        feedReleaseCount++;
        vexDigitalPinSet(P_FEED_RELEASE, 0);
        WAIT(250);
        autonStep++;
    }
    if(STEP(startStep+1)) {
        vex_printf("Feed Out\n");
        vexMotorSet(M_FEED, FEED_SPEED);
        WAIT(duration);
        autonStep++;
    }
    if(STEP(startStep+2)) {
        vex_printf("Feed stop\n");
        vexMotorSet(M_FEED, 0);
        WAIT(250);
        autonStep++;
    }
    if(STEP(startStep+3)) {
        vex_printf("Feed close\n");
        vexDigitalPinSet(P_FEED_RELEASE, 1);
        autonStep++;
    }
    return (startStep+4);
}

msg_t
vexAutonomous( void *arg )
{
    (void)arg;
    vexTaskRegister("auton");

    if(!isRed()) {
        autonTurn *= -1;
    }
    bool compAuton = true;
    bool redSide = isRed();
    clearDriveEncoders();
    int nextStep;
    #define RUNSTEP(stepName, ...) nextStep = stepName(nextStep, ##__VA_ARGS__)
    enableFlyWheel(FLY_MID);
    WAIT(3000);

    autonStep = 0;
    vex_printf("starting Autonomous %s\n", (redSide?"red":"blue"));
    while(!chThdShouldTerminate()) {
        autonTime = chTimeNow();
        if(vexControllerGet(J_AUTON_STOP)) {
            break;
        }

        nextStep = 0;
        if(compAuton) {
            // Preloads
            RUNSTEP(turn, (redSide ? -149 : 150),  AUTON_TURN_SPEED);
            RUNSTEP(move, (redSide ? -1700: -1705), 450);
            RUNSTEP(turn, (redSide ? -65 : 45),  AUTON_TURN_SPEED);
            RUNSTEP(shootBalls, 3500); 

            // target B
            RUNSTEP(enableFeed, FEED_SPEED);
            RUNSTEP(turn, (redSide ? 155 : -180),  AUTON_TURN_SPEED);
            RUNSTEP(move, 1200,  350);
            RUNSTEP(enableFeed, 0);
            RUNSTEP(move, -1200,  350);
            RUNSTEP(turn, (redSide ? -150 : 100),  AUTON_TURN_SPEED);
            RUNSTEP(shootBalls, 3500);

            // target C
            RUNSTEP(turn, (redSide ? 250 : -300), AUTON_TURN_SPEED);
            RUNSTEP(enableFeed, FEED_SPEED);
            RUNSTEP(move, 1450,  370);
            RUNSTEP(enableFeed, 0);
            RUNSTEP(move, -1450, 370);
            RUNSTEP(turn, (redSide ? -250 : 310),  AUTON_TURN_SPEED);
            RUNSTEP(shootBalls, 3500);
        }
        else {
            RUNSTEP(shootBalls, 1500);
            RUNSTEP(strafe, (redSide ? 1500 : -1500), 350);
            RUNSTEP(shootBalls, 1500);
            RUNSTEP(move, 500, 350);
            RUNSTEP(rampDeploy);
        }

        if(STEP(nextStep)) {
            vex_printf("Exit step %d\n", autonStep);
            break;
        }

        xDrivePidUpdate();
        /* vex_printf("Forward = %d Strafe = %d Turn = %d\n", */
        /*             forwardPid->pidc->drive_cmd, */
        /*             strafePid->pidc->drive_cmd, */
        /*             turnPid->pidc->drive_cmd); */
        xDriveMotors(
            forwardPid->pidc->drive_cmd,
            strafePid->pidc->drive_cmd,
            turnPid->pidc->drive_cmd,
            M_DRIVE_FRONT_RIGHT,
            M_DRIVE_BACK_RIGHT,
            M_DRIVE_FRONT_LEFT,
            M_DRIVE_BACK_LEFT,
            25, 127
        );

        int16_t flyWheelOut = tbhUpdate(flyWheelCtrl);
        vexMotorSet(M_FLY_A, flyWheelOut);
        vexMotorSet(M_FLY_B, flyWheelOut);
        vexMotorSet(M_FLY_C, flyWheelOut);

        vexSleep(10);
    }
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
    bool feedRelease = false;
    bool feedReleaseOff = false;
    systime_t feedReleaseTime = 0;

    Debouncer dbncJFeedReleaseOff;
    debounceInit(&dbncJFeedReleaseOff, J_FEED_RELEASE_OFF, 50);

    //Speedometer *spdm = SpeedometerInit(S_ENC_FLY);

    vexDigitalPinSet(P_RAMP, 0);
    //Run until asked to terminate
    while(!chThdShouldTerminate()) {
        vex_printf("mode=%s\n", (isRed()?"red":"blue"));
        if(vexControllerGet(J_AUTON)) {
            vexAutonomous(NULL);
        }

        currentTime = chTimeNow();
        //Stop timer for piston if the button is pressed
        if(!vexControllerGet(J_RAMP)) {
            vexDigitalPinSet(P_RAMP, 0);
        } else {
            vexDigitalPinSet(P_RAMP, 1);
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
                enableFlyWheel(i);
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
            feedReleaseCount++;
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
            else if((currentTime - feedReleaseTime) > 100) {
                vexDigitalPinSet(P_FEED_RELEASE, 1);
            }
        }

        // feed release override
        /* if(feedReleaseCount >= MAX_FEED_RELEASE_COUNT) { */
        /*     vexDigitalPinSet(P_FEED_RELEASE, 0); */
        /* } */

        if(debounceKeyDown(&dbncJFeedReleaseOff)) {
            feedReleaseOff = !feedReleaseOff;
        }
        if(feedReleaseOff) {
            vexDigitalPinSet(P_FEED_RELEASE, 0);
        }

        //5U Feed In, 5D Feed Out
        if (vexControllerGet(J_FEED_D)){
            vexMotorSet(M_FEED, -FEED_SPEED);
        }
        else if (vexControllerGet(J_FEED_U) || (feedRelease && (currentTime - feedReleaseTime) > 500)){
            vexMotorSet(M_FEED, feedRelease?FEED_MAX_SPEED:FEED_SPEED);
        } else {
            vexMotorSet(M_FEED, 0);
        }

        //Don't hog CPU
        vexSleep( 10 );
    }

    return (msg_t)0;
}
