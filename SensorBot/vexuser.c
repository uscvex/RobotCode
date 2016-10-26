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
#define M_DRIVE_LEFT_A    kVexMotor_7
#define M_DRIVE_LEFT_B    kVexMotor_4
#define M_DRIVE_RIGHT_A   kVexMotor_8
#define M_DRIVE_RIGHT_B   kVexMotor_3

#define M_FLY_A              kVexMotor_2
#define M_FLY_B              kVexMotor_5
#define M_FLY_C              kVexMotor_6

#define M_FEED            kVexMotor_9

// Sensor channels
#define P_RAMP        kVexDigital_4
#define P_FEED_RELEASE        kVexDigital_3

#define P_ENC_TOP_FLY_A       kVexDigital_9
#define P_ENC_TOP_FLY_B       kVexDigital_10

#define P_ENC_DRIVE_RIGHT_A   kVexDigital_7
#define P_ENC_DRIVE_RIGHT_B   kVexDigital_8

#define P_ENC_DRIVE_LEFT_A    kVexDigital_5
#define P_ENC_DRIVE_LEFT_B    kVexDigital_6

#define S_COLOR_SELECTOR        6

#define S_ENC_FLY             kVexSensorDigital_9
#define S_ENC_DRIVE_RIGHT     kVexSensorDigital_8
#define S_ENC_DRIVE_LEFT      kVexSensorDigital_6

// Joystick settings
#define J_TURN       Ch1
#define J_DRIVE      Ch3

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
#define AUTON_DRIVE_SPEED 350
#define AUTON_TURN_SPEED 450

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
    { P_ENC_DRIVE_LEFT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2, kVexQuadEncoder_4 }
};

// Motor Config
static  vexMotorCfg mConfig[] = {
    { M_DRIVE_LEFT_A,   kVexMotor393S, kVexMotorNormal,  kVexSensorNone, 0 },
    { M_DRIVE_LEFT_B,   kVexMotor393S, kVexMotorReversed,  kVexSensorNone, 0 },
    { M_DRIVE_RIGHT_A,  kVexMotor393S, kVexMotorNormal,  kVexSensorNone, 0 },
    { M_DRIVE_RIGHT_B,  kVexMotor393S, kVexMotorNormal,  kVexSensorNone, 0 },

    { M_FEED,              kVexMotor393S, kVexMotorReversed, kVexSensorNone, 0 },

    { M_FLY_A,             kVexMotor393T, kVexMotorReversed, kVexSensorNone, 0 },
    { M_FLY_B,             kVexMotor393T, kVexMotorReversed, kVexSensorNone, 0 },
    { M_FLY_C,             kVexMotor393T, kVexMotorReversed, kVexSensorNone, 0 }
};

// TBH Controllers
TBHController *flyWheelCtrl;

// PID Controllers
EPidController *leftDrivePid;
EPidController *rightDrivePid;

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
    flyWheelCtrl = TBHControllerInit(S_ENC_FLY, 0.01, 11000, false);
    flyWheelCtrl->powerZeroClamp = true;
    flyWheelCtrl->log = false;

    // Initialize PID
    leftDrivePid = EPidInit(kFlat, 0.001, 0, 0.01, S_ENC_DRIVE_LEFT, true);
    rightDrivePid = EPidInit(kFlat, 0.001, 0, 0.01, S_ENC_DRIVE_RIGHT, true);
}

bool driveMotors(void) {
  short ld, rd ;
  //Calculate Motor Power
  int forward = VALLEY(vexControllerGet(J_DRIVE), 25, 127);
  int turn;

  turn = VALLEY(vexControllerGet(J_TURN), 25, 127);
  ld = VALLEY(forward + turn, 25, 127);
  rd = VALLEY(forward - turn, 25, 127);

  vexMotorSet(M_DRIVE_LEFT_A,  ld);
  vexMotorSet(M_DRIVE_LEFT_B,  ld);
  vexMotorSet(M_DRIVE_RIGHT_A, rd);
  vexMotorSet(M_DRIVE_RIGHT_B, rd);

  return (ld != 0 || rd != 0);
}

bool isRed(void) {
    return (vexAdcGet(S_COLOR_SELECTOR) < 2000);
}

void disableDrivePids(void) {
    EPidDisable(leftDrivePid);
    EPidDisable(rightDrivePid);
}

int autonStep = 0;
int autonTurnDir = 1;
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

int moveForward(int startStep, int target, float speed) {
    if(STEP(startStep)) {
        vex_printf("Move\n");
        int32_t duration = (ABS(target)/speed)*1000;
        int32_t waitTime = duration * 2;
        EPidEnable(rightDrivePid, duration, target);
        EPidEnable(leftDrivePid, duration, target);
        autonStep++;
        WAIT(waitTime);
    }
    if(STEP(startStep+1)){
        disableDrivePids();
        autonStep++;
    }
    return (startStep+2);
}

int turn(int startStep, int target, int dir, float speed) {
    if(STEP(startStep)) {
        vex_printf("Turn\n");
        int32_t duration = (ABS(target)/speed)*1000;
        int32_t waitTime = duration * 3;
        EPidEnable(rightDrivePid, duration, dir * target);
        EPidEnable(rightDrivePid, duration, -dir * target);
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

    if(isRed()) {
        autonTurnDir *= -1;
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
            RUNSTEP(turn, 150, autonTurnDir, AUTON_TURN_SPEED);
            RUNSTEP(moveForward, -1700, AUTON_DRIVE_SPEED);
            RUNSTEP(turn, 65, autonTurnDir, AUTON_TURN_SPEED);
            RUNSTEP(shootBalls, 3500);

            // Target E
            RUNSTEP(moveForward, 400, AUTON_DRIVE_SPEED); // move back
            RUNSTEP(turn, 235, autonTurnDir, AUTON_TURN_SPEED); // line up for E
            RUNSTEP(enableFeed, FEED_SPEED); // Run feeds and intake balls
            RUNSTEP(moveForward, 200, AUTON_DRIVE_SPEED);
            /* RUNSTEP(moveForward, -200, AUTON_DRIVE_SPEED); */
            /* RUNSTEP(moveForward, 200, AUTON_DRIVE_SPEED); */
            RUNSTEP(moveForward, -1200, AUTON_DRIVE_SPEED); // go to center of field
            RUNSTEP(enableFeed, 0);
            RUNSTEP(turn, -235, autonTurnDir, AUTON_TURN_SPEED); // line up for shot
            RUNSTEP(shootBalls, 3500);

            // Target G
            /* RUNSTEP(enableFeed, FEED_SPEED); */
            /* RUNSTEP(moveForward, 500, AUTON_DRIVE_SPEED); */
            /* RUNSTEP(moveForward, -500, AUTON_DRIVE_SPEED); */
            /* RUNSTEP(enableFeed, 0); */
            /* RUNSTEP(shootBalls, 3500); */

            /* // Target I/J */
            /* RUNSTEP(turn, -255, autonTurnDir, AUTON_TURN_SPEED); // turn 45 */
            /* RUNSTEP(enableFeed, FEED_SPEED); */
            /* RUNSTEP(moveForward, 1200, AUTON_DRIVE_SPEED); */
            /* RUNSTEP(moveForward, -1200, AUTON_DRIVE_SPEED); */
            /* RUNSTEP(enableFeed, 0); */
            /* RUNSTEP(turn, 255, autonTurnDir, AUTON_TURN_SPEED); */
            /* RUNSTEP(shootBalls, 3500); */
        }
        else {
            RUNSTEP(shootBalls, 1500);
            RUNSTEP(turn, 510, autonTurnDir, AUTON_TURN_SPEED); // turn 90
            RUNSTEP(moveForward, -2000, AUTON_DRIVE_SPEED);
            RUNSTEP(turn, -510, autonTurnDir, AUTON_TURN_SPEED); // turn -90
            RUNSTEP(shootBalls, 1500);
            RUNSTEP(moveForward, 150, AUTON_DRIVE_SPEED);//slam against corner
        }

        if(STEP(nextStep)) {
            vex_printf("Exit step %d\n", autonStep);
            break;
        }

        int16_t rightDriveVal = EPidUpdate(rightDrivePid);
        int16_t leftDriveVal = EPidUpdate(leftDrivePid);
        vexMotorSet(M_DRIVE_RIGHT_A, rightDriveVal);
        vexMotorSet(M_DRIVE_RIGHT_B, rightDriveVal);
        vexMotorSet(M_DRIVE_LEFT_A, leftDriveVal);
        vexMotorSet(M_DRIVE_LEFT_B, leftDriveVal);

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
        vex_printf("mode = %s\n", (isRed()?"red":"bue"));
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

        // user control drive
        driveMotors();

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
