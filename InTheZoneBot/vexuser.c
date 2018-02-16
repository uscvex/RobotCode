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
#define J_TANK_RIGHT Ch2
#define J_TANK_LEFT  Ch3

#define J_MOBILE_BASE_UP   Btn6U
#define J_MOBILE_BASE_DOWN Btn6D
#define J_CHAIN_LIFT_UP    Btn5U
#define J_CHAIN_LIFT_DOWN  Btn5D
#define J_SWEEP_IN         Btn8L
#define J_SWEEP_OUT        Btn8R
#define J_MODE_TOGGLE      Btn7U
#define J_MISC             Btn7L
#define J_REVERSE_DRIVE    Btn8U
#define J_TEST_AUTON       Btn7D

#define DIRECTION_UP       0
#define DIRECTION_FALLING  1
#define DIRECTION_BOTTOM   2

#define LIFT_MIN_HEIGHT      485
#define LIFT_MAX_HEIGHT      5
#define LIFT_START_HEIGHT    55

#define LIFT_MOBILE_BASE_HEIGHT 240
#define CONE_HEIGHT          79

#define SWEEP_IN_POS         1
#define SWEEP_START_POS      90
#define SWEEP_HALFWAY        270
#define SWEEP_OUT_POS        540
#define BREAK_FREE           9000

#define LIFT_CLOSE_ENOUGH    5
#define SWEEP_CLOSE_ENOUGH   5
#define LIFT_SEEK_RATE       1.0
#define SWEEP_SEEK_RATE      4.2
#define DRIVE_SEEK_RATE      55
#define DRIVE_TURN_SEEK_RATE 65

#define TICKS_PER_TILE      10725
#define TICKS_PER_90DEG     4725
#define TICKS_PER_DEG       25





// PID Controls
EPidController *leftDrivePid;
EPidController *rightDrivePid;

bool justChangedMode;
bool justReversed = false;
bool returning;
short stackCount = 0;
short stackStep = -1;
int sweepDesiredPos = -1;
int liftDesiredPos = -1;
int reversed = 1;
systime_t currTime;

//first column = actual value,second = target
//M_DRIVE_LEFT_F
//M_DRIVE_LEFT_B
//M_DRIVE_RIGHT_F
//M_DRIVE_RIGHT_B
int driveValues[4][2] = {0};

//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {

    { M_MOBILE_GOAL_L,       kVexMotor393S, kVexMotorReversed,        kVexSensorQuadEncoder,  kVexQuadEncoder_2 },
    { M_MOBILE_GOAL_R,       kVexMotor393S, kVexMotorReversed,        kVexSensorQuadEncoder,  kVexQuadEncoder_2 },

    { M_DRIVE_RIGHT_F,       kVexMotor393S, kVexMotorReversed,          kVexSensorQuadEncoder,  kVexQuadEncoder_1 },
    { M_DRIVE_RIGHT_B,       kVexMotor393S, kVexMotorReversed,          kVexSensorQuadEncoder,  kVexQuadEncoder_1 },
    { M_DRIVE_LEFT_F,        kVexMotor393S, kVexMotorReversed,          kVexSensorQuadEncoder,  kVexQuadEncoder_3 },
    { M_DRIVE_LEFT_B,        kVexMotor393S, kVexMotorReversed,          kVexSensorQuadEncoder,  kVexQuadEncoder_3 },

    { M_CHAIN_LIFT,          kVexMotor393S, kVexMotorReversed,          kVexSensorQuadEncoder,  kVexQuadEncoder_5 },

    { M_SWEEP,               kVexMotor393S, kVexMotorReversed,        kVexSensorQuadEncoder,  kVexQuadEncoder_4 }
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
    short topLimitSwitch = 1;
    //Calculate Motor Power

    //Regular drive
    ///*
    int forward = VALLEY(vexControllerGet(J_DRIVE), 20, 127);
    float turn  =  (-0.5)*VALLEY(vexControllerGet(J_TURN), 20, 127);

    ld = VALLEY(forward + (int)turn, 25, 127);
    rd = VALLEY(forward - (int)turn, 25, 127);
    //*/

    //Tank drive
    /*
    rd = VALLEY(vexControllerGet(J_TANK_LEFT),20,127);
    ld = VALLEY(vexControllerGet(J_TANK_RIGHT),20,127);
    //*/

    /*
    SetMotor(M_DRIVE_LEFT_F,  ld);
    SetMotor(M_DRIVE_LEFT_B,  ld);
    SetMotor(M_DRIVE_RIGHT_F, rd);
    SetMotor(M_DRIVE_RIGHT_B, rd);
    //*/
    ///*
    driveValues[0][1] = ld;
    driveValues[1][1] = ld;
    driveValues[2][1] = rd;
    driveValues[3][1] = rd;
    //*/

    ////vex_printf("%f %f\n", rd, ld);
    return (ld != 0 || rd != 0);
}

int getTimeDifference(systime_t startTime) {
    return (int) chTimeNow() - startTime;
}

void autostack() {

    {
      if (stackStep == 1) {
        sweepDesiredPos = BREAK_FREE;
        currTime = chTimeNow();
        stackStep++;
      }
      //Assuming sweep starting in
        if(stackStep == 2) {
            if((abs(sweepDesiredPos - vexSensorValueGet(S_SWEEP_ENC)) < SWEEP_CLOSE_ENOUGH
                && getTimeDifference(currTime) > 450) || getTimeDifference(currTime) > 700){
              //vexSensorValueSet(S_CHAIN_LIFT_ENC, LIFT_START_HEIGHT);
              //vexSensorValueSet(S_SWEEP_ENC, SWEEP_START_POS);
              liftDesiredPos = LIFT_MIN_HEIGHT;
              sweepDesiredPos = SWEEP_OUT_POS;
              currTime = chTimeNow();
              stackStep++;
            }
        }

        //Raise up
        if(stackStep == 3) {
            //vex_printf("Completing step 4\n");
            if((abs(liftDesiredPos - vexSensorValueGet(S_CHAIN_LIFT_ENC)) < LIFT_CLOSE_ENOUGH
                && getTimeDifference(currTime) > 850) || getTimeDifference(currTime) > 1000) {
                //vex_printf("Starting step 5\n");
                liftDesiredPos = LIFT_MOBILE_BASE_HEIGHT - (stackCount*CONE_HEIGHT);
                sweepDesiredPos = SWEEP_HALFWAY;
                currTime = chTimeNow();
                stackStep++;
            }
        }

        //Sweep in
        if(stackStep == 4) {
            //vex_printf("Completing step 5\n");
            if((abs(liftDesiredPos - vexSensorValueGet(S_CHAIN_LIFT_ENC)) < LIFT_CLOSE_ENOUGH
                && getTimeDifference(currTime) > 900) || getTimeDifference(currTime) > 1200) {
                //vex_printf("Starting step 6\n");
                sweepDesiredPos = SWEEP_IN_POS;
                currTime = chTimeNow();
                stackStep++;
            }
        }

        //Drop cone
        if(stackStep == 5) {
            //vex_printf("Completing step 6\n");
            if((abs(sweepDesiredPos - vexSensorValueGet(S_SWEEP_ENC)) < SWEEP_CLOSE_ENOUGH
                && getTimeDifference(currTime) > 1100) || getTimeDifference(currTime) > 1300) {
                //vex_printf("Starting step 7\n");
                currTime = chTimeNow();
                stackStep++;
                stackCount++;
                liftDesiredPos = LIFT_MIN_HEIGHT;
            }
        }


        //Sweep out
        if(stackStep == 6) {
            //vex_printf("Completing step 7\n");
            if((abs(liftDesiredPos - vexSensorValueGet(S_CHAIN_LIFT_ENC)) < LIFT_CLOSE_ENOUGH
                && getTimeDifference(currTime) > 850) || getTimeDifference(currTime) > 1000) {
                //vex_printf("Starting step 8\n");
                sweepDesiredPos = BREAK_FREE ;
                currTime = chTimeNow();
                stackStep++;
            }
        }
        //Exit
        if(stackStep == 7) {
            //vex_printf("Completing step 7\n");
            if(getTimeDifference(currTime) > 450) {liftDesiredPos = LIFT_MAX_HEIGHT;}
            if((abs(sweepDesiredPos - vexSensorValueGet(S_SWEEP_ENC)) < SWEEP_CLOSE_ENOUGH
                && getTimeDifference(currTime) > 850) || getTimeDifference(currTime) > 1500) {
                sweepDesiredPos = -1;
                liftDesiredPos = -1;
                stackStep = -1;
            }
        }

    }
}

//---------------------Autonomous Functions ----------------------------------//

/**
 drive_forward(double ratio);
 param:
 @ratio is the ditance you want to move. One block take a second to move
 */

task slewMotors(void *arg) {
    vexTaskRegister("slewMotors");
    //vex_printf("Starting task slewMotors\n");
    int liftPos;
    int sweepPos;
    while(1) {
        if(sweepDesiredPos > 0) {
            sweepPos = vexSensorValueGet(S_SWEEP_ENC);
            SetMotor(M_SWEEP, (-1)*(sweepDesiredPos - sweepPos)/SWEEP_SEEK_RATE);
        } else {
            SetMotor(M_SWEEP, 0);
        }
        if(liftDesiredPos> 0) {
            liftPos = vexSensorValueGet(S_CHAIN_LIFT_ENC);
            SetMotor(M_CHAIN_LIFT, (-1)*(liftDesiredPos - liftPos)/LIFT_SEEK_RATE);
        } else {
            SetMotor(M_CHAIN_LIFT, 0);
        }
        wait1Msec(20);
    }
}

task slewDriveTask(void *arg) {
    int rate = 10;
    while(true) {
      driveValues[0][0] = driveValues[0][0]-((driveValues[0][0]-driveValues[0][1])/rate);
      driveValues[1][0] = driveValues[1][0]-((driveValues[1][0]-driveValues[1][1])/rate);
      driveValues[2][0] =  driveValues[2][0]-((driveValues[2][0]-driveValues[2][1])/rate);
      driveValues[3][0] =  driveValues[3][0]-((driveValues[3][0]-driveValues[3][1])/rate);

      SetMotor(M_DRIVE_LEFT_F, driveValues[0][0]);
      SetMotor(M_DRIVE_LEFT_B, driveValues[1][0]);
      SetMotor(M_DRIVE_RIGHT_F, driveValues[2][0]);
      SetMotor(M_DRIVE_RIGHT_B, driveValues[3][0]);

      wait1Msec(20);
    }
}

void slewDriveMotor(int index, float rate) {

}


void drive_forward(double tiles){
  vexSensorValueSet(S_DRIVE_ENC_RIGHT, 0);
  vexSensorValueSet(S_DRIVE_ENC_LEFT, 0);

  int duration = abs(700*tiles);
  int target = TICKS_PER_TILE*tiles;
  int leftDrivePos;
  int rightDrivePos;
  int ld;
  int rd;
  systime_t init_time = chTimeNow();
  systime_t auton_time;
  while(!chThdShouldTerminate()) {
    if(vexControllerGet(Btn7R)){
        break;
    }
    auton_time = chTimeNow() - init_time;

    leftDrivePos = vexSensorValueGet(S_DRIVE_ENC_LEFT);
    rightDrivePos = vexSensorValueGet(S_DRIVE_ENC_RIGHT);
    ld = (target - leftDrivePos)/DRIVE_SEEK_RATE;
    rd = (target - rightDrivePos)/DRIVE_SEEK_RATE;
    if(auton_time < 150) {
      ld = target < 0 ? -30 : 30;
      rd = target < 0 ? -30 : 30;
    }
    if(target > 0){
      if(auton_time >= duration || leftDrivePos >= target || rightDrivePos >= target) {
        backpulse(false);
        break;
      } else {
        SetMotor(M_DRIVE_LEFT_F, ld);
        SetMotor(M_DRIVE_LEFT_B, ld);
        SetMotor(M_DRIVE_RIGHT_F, rd);
        SetMotor(M_DRIVE_RIGHT_B, rd);
      }
    }
    if(target < 0) {
      if(auton_time >= duration ||leftDrivePos <= target || rightDrivePos <= target){
        backpulse(true);
        break;
      } else {
        SetMotor(M_DRIVE_LEFT_F, ld);
        SetMotor(M_DRIVE_LEFT_B, ld);
        SetMotor(M_DRIVE_RIGHT_F, rd);
        SetMotor(M_DRIVE_RIGHT_B, rd);
      }
    }
  }
}

void backpulse(bool backwards){
  systime_t backtime = chTimeNow();
  while(chTimeNow() - backtime < 225){
    int speed = backwards ? 40 : -40;
    SetMotor(M_DRIVE_LEFT_F, speed);
    SetMotor(M_DRIVE_LEFT_B, speed);
    SetMotor(M_DRIVE_RIGHT_F, -speed);
    SetMotor(M_DRIVE_RIGHT_B, -speed);
  }
  SetMotor(M_DRIVE_LEFT_F, 0);
  SetMotor(M_DRIVE_LEFT_B, 0);
  SetMotor(M_DRIVE_RIGHT_F, 0);
  SetMotor(M_DRIVE_RIGHT_B, 0);
}

//Postive = counterclockwise
void turn_deg(double degrees){

  vexSensorValueSet(S_DRIVE_ENC_RIGHT, 0);
  vexSensorValueSet(S_DRIVE_ENC_LEFT, 0);

  int duration = abs(16*degrees);
  int target = TICKS_PER_DEG*degrees;
  int leftDrivePos;
  int rightDrivePos;
  int ld;
  int rd;
  systime_t init_time = chTimeNow();
  systime_t auton_time;
  while(!chThdShouldTerminate()) {
    if(vexControllerGet(Btn7R)){
        break;
    }
    auton_time = chTimeNow() - init_time;

    leftDrivePos = vexSensorValueGet(S_DRIVE_ENC_LEFT);
    rightDrivePos = vexSensorValueGet(S_DRIVE_ENC_RIGHT);
    ld = (target - leftDrivePos)/DRIVE_TURN_SEEK_RATE;
    rd = (-1)*(target - rightDrivePos)/DRIVE_TURN_SEEK_RATE;

    if(target > 0){
      if(auton_time >= duration || leftDrivePos >= target || rightDrivePos <= -target) {
        SetMotor(M_DRIVE_LEFT_F,   0);
        SetMotor(M_DRIVE_RIGHT_F, 0);
        SetMotor(M_DRIVE_LEFT_B,  0);
        SetMotor(M_DRIVE_RIGHT_B, 0);
        break;
      } else {
        SetMotor(M_DRIVE_LEFT_F, ld);
        SetMotor(M_DRIVE_LEFT_B, ld);
        SetMotor(M_DRIVE_RIGHT_F, rd);
        SetMotor(M_DRIVE_RIGHT_B, rd);
      }
    }
    if(target < 0) {
      if(auton_time >= duration ||leftDrivePos <= target || rightDrivePos >= -target){
        SetMotor(M_DRIVE_LEFT_F,  0);
        SetMotor(M_DRIVE_RIGHT_F, 0);
        SetMotor(M_DRIVE_LEFT_B,  0);
        SetMotor(M_DRIVE_RIGHT_B, 0);
        break;
      } else {
          SetMotor(M_DRIVE_LEFT_F, ld);
          SetMotor(M_DRIVE_LEFT_B, ld);
          SetMotor(M_DRIVE_RIGHT_F, rd);
          SetMotor(M_DRIVE_RIGHT_B, rd);
      }
    }
  }
}

void autostack_auton(int toStack){
  int stack = 0;
  stackStep = 1;
  while(stack < toStack){
    autostack();
    if(stackStep == -1){
      stack +=1;
      stackStep = 1;
    }
  }
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

void deployChain(void) {
  systime_t init_time = chTimeNow();
  systime_t duration = abs(250);
  while(!chThdShouldTerminate()) {
    systime_t currTime = chTimeNow() - init_time;
    if(currTime < duration) {
      vexMotorSet(M_MOBILE_GOAL_R, 127);
      vexMotorSet(M_MOBILE_GOAL_L, 127);
    } else {
        vexMotorSet(M_MOBILE_GOAL_R, 0);
        vexMotorSet(M_MOBILE_GOAL_L, 0);
        break;
    }
  }
}

void raiseCone() {
  systime_t init_time = chTimeNow();
  systime_t duration = abs(500);
  while(!chThdShouldTerminate()) {
    systime_t currTime = chTimeNow() - init_time;
    if(currTime < duration || vexSensorValueGet(S_CHAIN_LIFT_ENC)==LIFT_MAX_HEIGHT) {
      SetMotor(M_CHAIN_LIFT, 127);
      SetMotor(M_CHAIN_LIFT, 127);
    } else {
        SetMotor(M_CHAIN_LIFT, 0);
        SetMotor(M_CHAIN_LIFT, 0);
        break;
    }
  }
}

void extendMobileBase(void) {
  systime_t init_time = chTimeNow();
  systime_t duration = abs(3000);
  while(!chThdShouldTerminate()) {
    if(vexControllerGet(Btn7R)){
        break;
    }
    systime_t currTime = chTimeNow() - init_time;
    if(vexSensorValueGet(S_DOWN_LIMIT_SWITCH) != 0 && currTime < duration) {
      vexMotorSet(M_MOBILE_GOAL_R, -127);
      vexMotorSet(M_MOBILE_GOAL_L, -127);
    } else {
        vexMotorSet(M_MOBILE_GOAL_R, 0);
        vexMotorSet(M_MOBILE_GOAL_L, 0);
        break;
    }
  }
}

void dropOffGoal(void) {
  systime_t init_time = chTimeNow();
  systime_t duration = abs(3000);
  while(!chThdShouldTerminate()) {
    if(vexControllerGet(Btn7R)){
        break;
    }
    systime_t currTime = chTimeNow() - init_time;
    if(vexSensorValueGet(S_DOWN_LIMIT_SWITCH) != 0 && currTime < duration) {
      vexMotorSet(M_MOBILE_GOAL_R, -127);
      vexMotorSet(M_MOBILE_GOAL_L, -127);
      SetMotor(M_DRIVE_LEFT_F,  80);
      SetMotor(M_DRIVE_RIGHT_F, 80);
      SetMotor(M_DRIVE_LEFT_B,  80);
      SetMotor(M_DRIVE_RIGHT_B, 80);
    } else {
        vexMotorSet(M_MOBILE_GOAL_R, 0);
        vexMotorSet(M_MOBILE_GOAL_L, 0);
        SetMotor(M_DRIVE_LEFT_F,   0);
        SetMotor(M_DRIVE_RIGHT_F, 0);
        SetMotor(M_DRIVE_LEFT_B,  0);
        SetMotor(M_DRIVE_RIGHT_B, 0);
        break;
    }
  }
}
void retractMobileBase(void) {
  systime_t init_time = chTimeNow();
  systime_t duration = abs(3000);
  while(!chThdShouldTerminate()) {
    if(vexControllerGet(Btn7R)){
        break;
    }
    systime_t currTime = chTimeNow() - init_time;
    if(vexSensorValueGet(S_UP_LIMIT_SWITCH) != 0 && currTime < duration) {
      vexMotorSet(M_MOBILE_GOAL_R, 127);
      vexMotorSet(M_MOBILE_GOAL_L, 127);
    } else {
        vexMotorSet(M_MOBILE_GOAL_R, 0);
        vexMotorSet(M_MOBILE_GOAL_L, 0);
        break;
    }
  }
}

void backAwayFromGoal(){
  systime_t init_time = chTimeNow();
  systime_t duration = abs(1200);
  while(!chThdShouldTerminate()) {
    if(vexControllerGet(Btn7R)){
        break;
    }
    systime_t currTime = chTimeNow() - init_time;
    if(vexSensorValueGet(S_UP_LIMIT_SWITCH) != 0 && currTime < duration) {
      vexMotorSet(M_MOBILE_GOAL_R, 127);
      vexMotorSet(M_MOBILE_GOAL_L, 127);
      SetMotor(M_DRIVE_LEFT_F,  -80);
      SetMotor(M_DRIVE_RIGHT_F, -80);
      SetMotor(M_DRIVE_LEFT_B,  -80);
      SetMotor(M_DRIVE_RIGHT_B, -80);
    } else {
        vexMotorSet(M_MOBILE_GOAL_R, 0);
        vexMotorSet(M_MOBILE_GOAL_L, 0);
        SetMotor(M_DRIVE_LEFT_F,  0);
        SetMotor(M_DRIVE_RIGHT_F, 0);
        SetMotor(M_DRIVE_LEFT_B,  0);
        SetMotor(M_DRIVE_RIGHT_B, 0);
        break;
    }
  }
}

void alignAgainstBar() {
  systime_t init_time = chTimeNow();
  systime_t duration = abs(1200);
  while(!chThdShouldTerminate()) {
    if(vexControllerGet(Btn7R)){
        break;
    }
    systime_t currTime = chTimeNow() - init_time;
    if(currTime < duration) {
      SetMotor(M_DRIVE_LEFT_F,  30);
      SetMotor(M_DRIVE_RIGHT_F, 30);
      SetMotor(M_DRIVE_LEFT_B,  30);
      SetMotor(M_DRIVE_RIGHT_B, 30);
    } else {
        SetMotor(M_DRIVE_LEFT_F,  0);
        SetMotor(M_DRIVE_RIGHT_F, 0);
        SetMotor(M_DRIVE_LEFT_B,  0);
        SetMotor(M_DRIVE_RIGHT_B, 0);
        break;
    }
  }
}

void dropCone(void){
  systime_t init_time = chTimeNow();
  systime_t duration = abs(1000);
  while(!chThdShouldTerminate()) {
    if(vexControllerGet(Btn7R)){
        break;
    }
    systime_t currTime = chTimeNow() - init_time;
    if(currTime < duration) {
      vexMotorSet(M_CHAIN_LIFT, -127);
    } else {
      vexMotorSet(M_CHAIN_LIFT, 0);
        break;
    }
  }
}

void sweepOut(){
  systime_t init_time = chTimeNow();
  systime_t duration = abs(1000);
  while(!chThdShouldTerminate()) {
    if(vexControllerGet(Btn7R)){
        break;
    }
    systime_t currTime = chTimeNow() - init_time;
    if(currTime < duration) {
      vexMotorSet(M_SWEEP, -69);
    } else {
      vexMotorSet(M_CHAIN_LIFT, 0);
        break;
    }
  }
}

//---------------------Autonomous routine-------------------------------------//

msg_t vexAutonomous( void *arg )
{
    (void)arg;
    vexTaskRegister("auton");


    //drive_forward(1);
    //wait(1);
    //turn_deg(-90);
    ///*
    vexSensorValueSet(S_SWEEP_ENC, SWEEP_START_POS);
    raiseCone();
    vexSensorValueSet(S_CHAIN_LIFT_ENC, LIFT_START_HEIGHT);
    extendMobileBase();
    //Pick up mobile goal
    drive_forward(2);
    wait(1);
    retractMobileBase();
    if(vexSensorValueGet(S_UP_LIMIT_SWITCH) == 0){
      //Drop preload
      dropCone();

      //Return to drive load platform
      drive_forward(-2);
      wait(1);
      turn_deg(175);
      wait(0.5);
      drive_forward(1.2);
      dropOffGoal();
      backAwayFromGoal();

      //Second goal
      /*
      wait(1);
      alignAgainstBar();
      wait(0.5);
      turn_deg(-90);
      wait(0.5);
      drive_forward(0.75);
      wait(0.5);
      turn_deg(-20);
      //*/
    }

    //autostack 2 cones
    /*
    autostack_auton(2);
    turn_deg(-90);
    //drive forward
    drive_forward(1);
    wait(0.5);
    turn_deg(45);
    wait(0.25);
    drive_forward(0.5);
    turn_deg(-45);
    drive_forward(1);
    //Drop base
    extendMobileBase();;
    //*/
    //turn_deg(32);


    return (msg_t)0;
}

//---------------------User control settings----------------------------------//


msg_t vexOperator( void *arg )
{

    (void)arg;
    vexTaskRegister("operator");

    StartTask(slewMotors);
    StartTask(slewDriveTask);
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

        if(vexControllerGet(J_MISC)){
          vexSensorValueSet(S_DRIVE_ENC_LEFT, 0);
          vexSensorValueSet(S_DRIVE_ENC_RIGHT, 0);
          vexSensorValueSet(S_CHAIN_LIFT_ENC, LIFT_START_HEIGHT);
          vexSensorValueSet(S_SWEEP_ENC, SWEEP_START_POS);
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
            if (stackStep <= 0 && !returning){
              SetMotor(M_CHAIN_LIFT, 0);
              //liftDesiredPos = vexSensorValueGet(S_CHAIN_LIFT_ENC);
            }                                       // don't override w/o button press if stacking
        }

        if (vexControllerGet(J_SWEEP_IN)) {
            vexMotorSet(M_SWEEP, 55);
            stackStep = -1;                                         // abort stack if manual movement of sweep
            sweepDesiredPos = -1;
            liftDesiredPos = -1;
        } else if (vexControllerGet(J_SWEEP_OUT)) {
            vexMotorSet(M_SWEEP, -127);
            stackStep = -1;                                         // abort stack if manual movement of sweep
            sweepDesiredPos = -1;
            liftDesiredPos = -1;
        } else {
            if (stackStep <= 0 && !returning)                                     // don't override w/o button press if stacking
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

        if(vexControllerGet(J_TEST_AUTON)){
          vexAutonomous(NULL);
        }


        //Don't hog cpu
        vexSleep(10);
    }

    return (msg_t)0;

}
