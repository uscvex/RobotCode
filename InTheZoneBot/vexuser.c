#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"
#include "../Common/easing.h"
#include "../Common/linefollower.h"
#include "smartmotor.h"
#include "vexspi.h"

// Motor mappings

//Sweep
#define M_SWEEP      kVexMotor_2

//Chain lift
#define M_CHAIN_LIFT kVexMotor_9

//Drive
#define M_DRIVE_RIGHT_F  kVexMotor_5
#define M_DRIVE_RIGHT_B  kVexMotor_8
#define M_DRIVE_LEFT_F   kVexMotor_6
#define M_DRIVE_LEFT_B   kVexMotor_3

//Mobile goal
#define M_MOBILE_GOAL_R   kVexMotor_4
#define M_MOBILE_GOAL_L   kVexMotor_7

// Sensor mappings
#define P_DRIVE_ENC_LEFT_A    kVexDigital_1
#define P_DRIVE_ENC_LEFT_B    kVexDigital_2
#define P_DRIVE_ENC_RIGHT_A   kVexDigital_3
#define P_DRIVE_ENC_RIGHT_B   kVexDigital_4
#define P_LIFT_ENC_A          kVexDigital_6
#define P_LIFT_ENC_B          kVexDigital_7
#define P_UP_LIMIT_SWITCH     kVexDigital_5
#define P_DOWN_LIMIT_SWITCH   kVexDigital_10
#define P_SWEEP_ENC_A         kVexDigital_8
#define P_SWEEP_ENC_B         kVexDigital_9

#define S_DRIVE_ENC_LEFT      kVexSensorDigital_1
#define S_DRIVE_ENC_RIGHT     kVexSensorDigital_3
#define S_UP_LIMIT_SWITCH     kVexSensorDigital_5
#define S_CHAIN_LIFT_ENC      kVexSensorDigital_7
#define S_DOWN_LIMIT_SWITCH   kVexSensorDigital_10
#define S_SWEEP_ENC           kVexSensorDigital_8

#define POWER_EXPANDER        kVexAnalog_2

// Controller mappings
#define J_DRIVE Ch3
#define J_TURN  Ch1

#define J_MOBILE_BASE_UP   Btn6U
#define J_MOBILE_BASE_DOWN Btn6D
#define J_CHAIN_LIFT_UP    Btn5U
#define J_CHAIN_LIFT_DOWN  Btn5D
#define J_SWEEP_IN         Btn8L
#define J_SWEEP_OUT        Btn8R
#define J_MODE_TOGGLE      Btn7U
#define J_MISC             Btn7L
#define J_REVERSE_DRIVE    Btn8D
#define J_DECREASE_COUNT   Btn7R

#define DIRECTION_UP       0
#define DIRECTION_FALLING  1
#define DIRECTION_BOTTOM   2

#define LIFT_START_HEIGHT    1
#define CONE_PICKUP_HEIGHT   320
#define JIGGLE_CONE_LEFT     300
#define BASE_CONE_HEIGHT     730
#define BASE_DROP_HEIGHT     330
#define CHAIN_TICKS_PER_CONE 80

#define SWEEP_IN_POS         420
#define SWEEP_OUT_POS        1
#define SWEEP_PRELOAD_POS    205

#define LIFT_CLOSE_ENOUGH    30
#define JIGGLE_CLOSE_ENOUGH  20
#define SWEEP_CLOSE_ENOUGH   20
#define LIFT_SEEK_RATE       1.5
#define SWEEP_SEEK_RATE      5



// PID Controls
EPidController *leftDrivePid;
EPidController *rightDrivePid;

//bool autoStackMode;                                           // not needed, can use stackStep for everything
bool justChangedMode;
bool justReversed = false;
short stackCount = 0;
short stackStep = -1;
int sweepDesiredPos = -1;
int liftDesiredPos = -1;
int reversed = 1;
systime_t currTime;

//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {

    { M_MOBILE_GOAL_L,       kVexMotor393S, kVexMotorReversed,        kVexSensorQuadEncoder,  kVexQuadEncoder_2 },
    { M_MOBILE_GOAL_R,       kVexMotor393S, kVexMotorReversed,        kVexSensorQuadEncoder,  kVexQuadEncoder_2 },

    { M_DRIVE_RIGHT_F,       kVexMotor393S, kVexMotorReversed,          kVexSensorQuadEncoder,  kVexQuadEncoder_1 },
    { M_DRIVE_RIGHT_B,       kVexMotor393S, kVexMotorReversed,          kVexSensorQuadEncoder,  kVexQuadEncoder_1 },
    { M_DRIVE_LEFT_F,        kVexMotor393S, kVexMotorReversed,          kVexSensorQuadEncoder,  kVexQuadEncoder_3 },
    { M_DRIVE_LEFT_B,        kVexMotor393S, kVexMotorReversed,          kVexSensorQuadEncoder,  kVexQuadEncoder_3 },

    { M_CHAIN_LIFT,          kVexMotor393S, kVexMotorReversed,        kVexSensorQuadEncoder,  kVexQuadEncoder_5 },

    { M_SWEEP,               kVexMotor393S, kVexMotorReversed,        kVexSensorQuadEncoder,  kVexQuadEncoder_5 }
};

static vexDigiCfg dConfig[] = {
    { P_DRIVE_ENC_RIGHT_A, kVexSensorQuadEncoder,  kVexConfigQuadEnc1,  kVexQuadEncoder_1 },
    { P_DRIVE_ENC_RIGHT_B, kVexSensorQuadEncoder,  kVexConfigQuadEnc2,  kVexQuadEncoder_1 },

    { P_DRIVE_ENC_LEFT_A,  kVexSensorQuadEncoder,  kVexConfigQuadEnc1,  kVexQuadEncoder_3 },
    { P_DRIVE_ENC_LEFT_B,  kVexSensorQuadEncoder,  kVexConfigQuadEnc2,  kVexQuadEncoder_3 },

    { P_LIFT_ENC_A,        kVexSensorQuadEncoder,  kVexConfigQuadEnc1,  kVexQuadEncoder_5 },
    { P_LIFT_ENC_B,        kVexSensorQuadEncoder,  kVexConfigQuadEnc2,  kVexQuadEncoder_5 },

    { P_SWEEP_ENC_A,        kVexSensorQuadEncoder,  kVexConfigQuadEnc1,  kVexQuadEncoder_4},
    { P_SWEEP_ENC_B,        kVexSensorQuadEncoder,  kVexConfigQuadEnc2,  kVexQuadEncoder_4},

    { P_UP_LIMIT_SWITCH,    kVexSensorDigitalInput, kVexConfigInput,     0},
    { P_DOWN_LIMIT_SWITCH,  kVexSensorDigitalInput, kVexConfigInput,     0}
};

//-------------------Setup----------------------------------------------------//
void vexUserSetup()
{
    vexMotorConfigure   ( mConfig,   MOT_CONFIG_SIZE( mConfig ) )  ;
    vexDigitalConfigure ( dConfig, DIG_CONFIG_SIZE( dConfig ) );
}

void vexUserInit()
{
    //bStopTasksBetweenModes = false;

    SmartMotorsInit();
    SmartMotorSetPowerExpanderStatusPort(POWER_EXPANDER);
    SmartMotorsAddPowerExtender(M_DRIVE_RIGHT_F, M_DRIVE_LEFT_F, M_MOBILE_GOAL_R, M_MOBILE_GOAL_L);
    SmartMotorLinkMotors(M_DRIVE_RIGHT_F, M_DRIVE_RIGHT_B);
    SmartMotorLinkMotors(M_DRIVE_LEFT_F, M_DRIVE_LEFT_B);
    SmartMotorPtcMonitorEnable();
    SmartMotorRun();

    leftDrivePid  =  EPidInit(kMinJerk, 0.01, 0, 0.01, S_DRIVE_ENC_LEFT,  false);
    rightDrivePid =  EPidInit(kMinJerk, 0.01, 0, 0.01, S_DRIVE_ENC_RIGHT,  true);

}

//-------------Miscellaneous functions----------------------------------------//




bool driveMotors(void) {
    short ld, rd;
    //Calculate Motor Power
    int forward = VALLEY(vexControllerGet(J_DRIVE), 20, 127)*reversed;
    float turn  =  (-0.6)*VALLEY(vexControllerGet(J_TURN), 20, 127);

    ld = VALLEY(forward + (int)turn, 25, 127);
    rd = VALLEY(forward - (int)turn, 25, 127);

    SetMotor(M_DRIVE_LEFT_F,  ld);
    SetMotor(M_DRIVE_LEFT_B,  ld);
    SetMotor(M_DRIVE_RIGHT_F, rd);
    SetMotor(M_DRIVE_RIGHT_B, rd);

    ////vex_printf("%f %f\n", rd, ld);
    return (ld != 0 || rd != 0);
}

int getTimeDifference(systime_t startTime) {
    return (int) chTimeNow() - startTime;
}



void autostack() {

    ////vex_printf("Entering autostack");
    //while(autoStackMode)
    // no while means we can still drive whilst stacking
    {
        // removed 'toggle part 2'
        // this should work exactly the same as before
        //Drop crane and pick up cone
        /
        if(stackStep == 1) {
            currTime = chTimeNow();
            vexSensorValueSet(S_CHAIN_LIFT_ENC, CONE_PICKUP_HEIGHT);
            vexSensorValueSet(S_SWEEP_ENC, SWEEP_OUT_POS);
            //vex_printf("step 1\n");
            liftDesiredPos = LIFT_START_HEIGHT;
            stackStep = 2;
        }
        //jiggle to hold cone tight
        if(stackStep == 2) {
            //vex_printf("Completing step 1\n");
            if((abs(liftDesiredPos - vexSensorValueGet(S_CHAIN_LIFT_ENC)) < LIFT_CLOSE_ENOUGH
                && getTimeDifference(currTime) > 700) || getTimeDifference(currTime) > 900) {
                //vex_printf("Starting step 2\n");
                sweepDesiredPos = JIGGLE_CONE_LEFT;
                currTime = chTimeNow();
                stackStep = 3;
            }
        }
        if(stackStep == 3) {
            //vex_printf("Completing step 2\n");
            if((abs(liftDesiredPos - vexSensorValueGet(S_CHAIN_LIFT_ENC)) < JIGGLE_CLOSE_ENOUGH
                && getTimeDifference(currTime) > 300) || getTimeDifference(currTime) > 400) {
                //vex_printf("Starting step 3\n");
                sweepDesiredPos = SWEEP_OUT_POS;
                currTime = chTimeNow();
                stackStep = 4;
            }
        }
        */
        //Raise cone to appropriate height
        if(stackStep == 4) {
            //vex_printf("Completing step 3\n");
            if((abs(liftDesiredPos - vexSensorValueGet(S_CHAIN_LIFT_ENC)) < JIGGLE_CLOSE_ENOUGH
                && getTimeDifference(currTime) > 300) || getTimeDifference(currTime) > 400) {
                //vex_printf("Starting step 4\n");
                liftDesiredPos = BASE_CONE_HEIGHT + stackCount*CHAIN_TICKS_PER_CONE;
                currTime = chTimeNow();
                stackStep = 5;
            }
        }

        //Spin around
        if(stackStep == 5) {
            //vex_printf("Completing step 4\n");
            if((abs(liftDesiredPos - vexSensorValueGet(S_CHAIN_LIFT_ENC)) < LIFT_CLOSE_ENOUGH
                && getTimeDifference(currTime) > 1200) || getTimeDifference(currTime) > 1600) {
                //vex_printf("Starting step 5\n");
                sweepDesiredPos = SWEEP_IN_POS;
                currTime = chTimeNow();
                stackStep = 6;
            }
        }

        //Drop cone
        if(stackStep == 6) {
            //vex_printf("Completing step 5\n");
            if((abs(sweepDesiredPos - vexSensorValueGet(S_SWEEP_ENC)) < SWEEP_CLOSE_ENOUGH
                && getTimeDifference(currTime) > 2000) || getTimeDifference(currTime) > 2500) {
                //vex_printf("Starting step 6\n");
                liftDesiredPos = BASE_DROP_HEIGHT + stackCount*CHAIN_TICKS_PER_CONE;
                currTime = chTimeNow();
                stackStep = 7;
                stackCount++;
            }
        }

        //Sweep out
        if(stackStep == 7) {
            //vex_printf("Completing step 6\n");
            if((abs(liftDesiredPos - vexSensorValueGet(S_CHAIN_LIFT_ENC)) < LIFT_CLOSE_ENOUGH
                && getTimeDifference(currTime) > 1500) || getTimeDifference(currTime) > 2000) {
                //vex_printf("Starting step 7\n");
                sweepDesiredPos = SWEEP_OUT_POS;
                currTime = chTimeNow();
                stackStep = 8;
            }
        }

        //Drop to start height
        if(stackStep == 8) {
            //vex_printf("Completing step 7\n");
            if((abs(sweepDesiredPos - vexSensorValueGet(S_SWEEP_ENC)) < SWEEP_CLOSE_ENOUGH
                && getTimeDifference(currTime) > 2500) || getTimeDifference(currTime) > 3000) {
                //vex_printf("Starting step 8\n");
                liftDesiredPos = LIFT_START_HEIGHT;
                currTime = chTimeNow();
                stackStep = -1;
            }
        }
        //wait1Msec(25);
    }
}

//---------------------Autonomous Functions ----------------------------------//

/**
 drive_forward(double ratio);
 param:
 @ratio is the ditance you want to move. One block take a second to move
 */

task slewMotors(void *arg)
{
    vexTaskRegister("slewMotors");
    //vex_printf("Starting task slewMotors\n");
    int liftPos;
    int sweepPos;
    while(1) {
        //if(autoStackMode)
        // and we wouldn't want to move the arm
        {
            if(sweepDesiredPos > 0) {
                sweepPos = vexSensorValueGet(S_SWEEP_ENC);
                SetMotor(M_SWEEP, (sweepDesiredPos - sweepPos)/SWEEP_SEEK_RATE);
            } else {
                SetMotor(M_SWEEP, 0);
            }
            if(liftDesiredPos> 0) {
                liftPos = vexSensorValueGet(S_CHAIN_LIFT_ENC);
                SetMotor(M_CHAIN_LIFT, (liftDesiredPos - liftPos)/LIFT_SEEK_RATE);
            } else {
                SetMotor(M_CHAIN_LIFT, 0);
            }
        }
        wait1Msec(20);
    }
}


void drive_forward(double ratio){
    systime_t init_time = chTimeNow();

    systime_t duration = abs(ratio*700);
    int32_t target = 1500*ratio;

    EPidEnable(rightDrivePid, duration, target);
    EPidEnable(leftDrivePid, duration, target);
    // systime_t end_time = chTimeNow() + duration;

    while (!chThdShouldTerminate()){
        if(vexControllerGet(Btn7R)){
            SetMotor(M_DRIVE_LEFT_F,  0);
            SetMotor(M_DRIVE_RIGHT_F, 0);
            SetMotor(M_DRIVE_LEFT_B,  0);
            SetMotor(M_DRIVE_RIGHT_B, 0);
            break;
        }
        // int backupLevel = BackupBatteryLevel;
        ////vex_printf("%d    %d   ", vexSpiGetMainBattery(), vexSpiGetBackupBattery());
        systime_t autonTime = chTimeNow() - init_time;
        if (autonTime < duration){
            int16_t ld = EPidUpdate(leftDrivePid);
            int16_t rd = EPidUpdate(rightDrivePid);
            SetMotor(M_DRIVE_LEFT_F,  ld);
            SetMotor(M_DRIVE_RIGHT_F, rd);
            SetMotor(M_DRIVE_LEFT_B,  ld);
            SetMotor(M_DRIVE_RIGHT_B, rd);
            vexSleep(10);
        }
        else{
            SetMotor(M_DRIVE_LEFT_F,  0);
            SetMotor(M_DRIVE_RIGHT_F, 0);
            SetMotor(M_DRIVE_LEFT_B,  0);
            SetMotor(M_DRIVE_RIGHT_B, 0);
            break;
        }
    }
    SetMotor(M_DRIVE_LEFT_F,  0);
    SetMotor(M_DRIVE_RIGHT_B, 0);
    SetMotor(M_DRIVE_LEFT_F,  0);
    SetMotor(M_DRIVE_RIGHT_B, 0);
    ////vex_printf("%d\n", vexSensorValueGet(S_DRIVE_ENC_RIGHT));
}

void turn_deg(double ratio){
    systime_t init_time = chTimeNow();


    systime_t duration = abs(ratio*1700);
    int32_t target = 4200*ratio;
    EPidEnable(rightDrivePid,duration, -target);
    EPidEnable(leftDrivePid,duration, target);
    // systime_t end_time = chTimeNow() + duration;

    while (!chThdShouldTerminate()){
        if(vexControllerGet(Btn7R)){
            SetMotor(M_DRIVE_LEFT_F,  0);
            SetMotor(M_DRIVE_RIGHT_F, 0);
            SetMotor(M_DRIVE_LEFT_B,  0);
            SetMotor(M_DRIVE_RIGHT_B, 0);

            break;
        }

        systime_t autonTime = chTimeNow() - init_time;
        if (autonTime < duration){
            int16_t ld = EPidUpdate(leftDrivePid);
            int16_t rd = EPidUpdate(rightDrivePid);
            SetMotor(M_DRIVE_LEFT_F,  ld);
            SetMotor(M_DRIVE_RIGHT_F, rd);
            SetMotor(M_DRIVE_LEFT_B,  ld);
            SetMotor(M_DRIVE_RIGHT_B, rd);
            vexSleep(10);
        }
        else{

            break;
        }
    }
    SetMotor(M_DRIVE_LEFT_F,   0);
    SetMotor(M_DRIVE_RIGHT_F, 0);
    SetMotor(M_DRIVE_LEFT_B,  0);
    SetMotor(M_DRIVE_RIGHT_B, 0);
}


void wait(double ratio){
    systime_t init_time = chTimeNow();
    systime_t duration = abs(ratio*1000);
    SetMotor(M_DRIVE_LEFT_F,   0);
    SetMotor(M_DRIVE_RIGHT_F, 0);
    SetMotor(M_DRIVE_LEFT_B,  0);
    SetMotor(M_DRIVE_RIGHT_B, 0);

    while (!chThdShouldTerminate()){
        if(vexControllerGet(Btn7R)){
            break;
        }

        systime_t autonTime = chTimeNow() - init_time;
        if (autonTime < duration){

            vexSleep(10);
        }
        else{
            break;
        }
    }
}


//---------------------Autonomous routine-------------------------------------//

msg_t vexAutonomous( void *arg )
{
    (void)arg;
    vexTaskRegister("auton");

    return (msg_t)0;
}

//---------------------User control settings----------------------------------//


msg_t vexOperator( void *arg )
{

    (void)arg;
    vexTaskRegister("operator");

    StartTask(slewMotors);
    //autoStackMode = false;                                            // variable never used

    while(!chThdShouldTerminate())
    {
        ////vex_printf("In operator task");
        if(vexControllerGet(J_MODE_TOGGLE)) {
            if(justChangedMode == false && stackStep > 0) {
                liftDesiredPos = -1;
                sweepDesiredPos = -1;
                stackStep = -1;
            }
            if(justChangedMode == false && stackStep <= 0) {
                stackStep = 1;
            }
            justChangedMode = true;
        } else {
            justChangedMode = false;
        }

        autostack();
        driveMotors();

        if(vexControllerGet(J_MISC)) {
            vexSensorValueSet(S_SWEEP_ENC, 150);
        }

        if (vexControllerGet(J_MOBILE_BASE_UP)) {
            vexMotorSet(M_MOBILE_GOAL_R, 127);
            vexMotorSet(M_MOBILE_GOAL_L, 127);
        } else if (vexControllerGet(J_MOBILE_BASE_DOWN)) {
            vexMotorSet(M_MOBILE_GOAL_R, -127);
            vexMotorSet(M_MOBILE_GOAL_L, -127);
        } else {
            vexMotorSet(M_MOBILE_GOAL_R, 0);
            vexMotorSet(M_MOBILE_GOAL_L, 0);
        }

        if (vexControllerGet(J_CHAIN_LIFT_UP)) {
            vexMotorSet(M_CHAIN_LIFT, 127);
            stackStep = -1;                                         // abort stack if manual movement of lift
            liftDesiredPos = -1;
            sweepDesiredPos = -1;
        } else if (vexControllerGet(J_CHAIN_LIFT_DOWN)) {
            vexMotorSet(M_CHAIN_LIFT, -127);
            stackStep = -1;                                         // abort stack if manual movement of lift
            liftDesiredPos = -1;
            sweepDesiredPos = -1;
        } else {
            if (stackStep <= 0){
              SetMotor(M_CHAIN_LIFT, 15);
              //liftDesiredPos = vexSensorValueGet(S_CHAIN_LIFT_ENC);
            }                                       // don't override w/o button press if stacking
        }

        if (vexControllerGet(J_SWEEP_IN)) {
            vexMotorSet(M_SWEEP, 60);
            stackStep = -1;                                         // abort stack if manual movement of sweep
            sweepDesiredPos = -1;
            liftDesiredPos = -1;
        } else if (vexControllerGet(J_SWEEP_OUT)) {
            vexMotorSet(M_SWEEP, -60);
            stackStep = -1;                                         // abort stack if manual movement of sweep
            sweepDesiredPos = -1;
            liftDesiredPos = -1;
        } else {
            if (stackStep <= 0)                                       // don't override w/o button press if stacking
                vexMotorSet(M_SWEEP, 0);
        }

        if(vexControllerGet(J_REVERSE_DRIVE)){
          if(justReversed == false) {
              reversed *= -1;
              justReversed = true;
          }
        } else {
            justReversed = false;
        }


        //Don't hog cpu
        vexSleep(10);
    }

    return (msg_t)0;

}
