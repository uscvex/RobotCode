#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"
#include "../Common/takebackhalf.h"
#include "../Common/easing.h"
#include "../Common/linefollower.h"

// Motor configs

#define M_FLY_WHEEL       kVexMotor_5
#define M_PUNCHER         kVexMotor_6

#define M_FEED_FRONT      kVexMotor_2

#define M_DRIVE_RIGHT1    kVexMotor_4
#define M_DRIVE_RIGHT2    kVexMotor_8

#define M_DRIVE_LEFT1     kVexMotor_3
#define M_DRIVE_LEFT2     kVexMotor_7

#define M_FEED_SHOOT      kVexMotor_9

// Sensor channels
#define P_ENC_FLY_A       kVexDigital_2
#define P_ENC_FLY_B       kVexDigital_1

#define P_ENC_DRIVE_RIGHT_A   kVexDigital_5
#define P_ENC_DRIVE_RIGHT_B   kVexDigital_6

#define P_ENC_DRIVE_LEFT_A    kVexDigital_3
#define P_ENC_DRIVE_LEFT_B    kVexDigital_4

//#define S_BALL_BOT              1
#define S_BALL_TOP              7
#define S_COLOR_SELECTOR        5

#define S_LINE_FOLLOWER_L2      1
#define S_LINE_FOLLOWER_L1      2
#define S_LINE_FOLLOWER_M       3
#define S_LINE_FOLLOWER_R1      4
#define S_LINE_FOLLOWER_R2      5

#define S_ENC_FLY             kVexSensorDigital_1
#define S_ENC_DRIVE_RIGHT     kVexSensorDigital_6
#define S_ENC_DRIVE_LEFT      kVexSensorDigital_4

// Joystick settings
#define J_DRIVE      Ch3
#define J_TURN       Ch1

// Shot settings
#define J_PUNCH         Btn7L
#define J_SHOOT_QUARTER Btn8U
#define J_SHOOT_PB      Btn8L
#define J_SHOOT_SIDE    Btn7U
#define J_SHOOT_MID     Btn8R
#define J_SHOOT_STOP   	Btn8D

#define J_START_AUTON	Btn7R
#define J_STOP_AUTON	Btn7D

#define J_FEED_SHOOT_U 	Btn6U
#define J_FEED_SHOOT_D  Btn6D
#define J_FEED_FRONT_U 	Btn5U
#define J_COMPRESS   	  Btn5D

//Autonomous modes

#define RED_DEFENSIVE   0
#define RED_AGGRESSIVE  1
#define BLUE_DEFENSIVE  2
#define BLUE_AGGRESSIVE 3
#define SKILLS          4
  

// Constants
#define DEFAULT_FEED_SPEED 80
#define FEED_SPOOL_TIME    100

#define FLY_WHEEL_MID_GAIN      0.02
#define FLY_WHEEL_SIDE_GAIN     0.02
#define FLY_WHEEL_QUARTER_GAIN  0.05
#define FLY_WHEEL_PB_GAIN       0.005

#define FLY_SIDE_SPEED 	   9900
#define FLY_PB_SPEED   	   7400
#define FLY_QUARTER_SPEED  8550
#define FLY_MID_SPEED  	   9500

#define AUTON_FEED_FAIL_TIME 2500

/*-------------------------------MOTOR CONFIGURATIONS-------------------------------*/


// Digi IO configuration
static  vexDigiCfg  dConfig[] = {
  { P_ENC_FLY_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_1},
  { P_ENC_FLY_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_1},

  { P_ENC_DRIVE_RIGHT_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_3},
  { P_ENC_DRIVE_RIGHT_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_3},

  { P_ENC_DRIVE_LEFT_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_4},
  { P_ENC_DRIVE_LEFT_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_4}
};

// Motor Config
static  vexMotorCfg mConfig[] = {
  { M_DRIVE_LEFT1,    kVexMotor393S, kVexMotorReversed, kVexSensorIME,  kVexQuadEncoder_4 },
  { M_DRIVE_RIGHT1,   kVexMotor393S, kVexMotorNormal,   kVexSensorIME,  kVexQuadEncoder_3 },
  { M_FEED_SHOOT,     kVexMotor393S, kVexMotorNormal,   kVexSensorNone, 0 },

  { M_PUNCHER,    kVexMotor393T, kVexMotorNormal, kVexSensorQuadEncoder, kVexQuadEncoder_2 },
  { M_FLY_WHEEL,  kVexMotor393T, kVexMotorNormal, kVexSensorQuadEncoder, kVexQuadEncoder_1 },

  { M_FEED_FRONT,     kVexMotor393S, kVexMotorReversed, kVexSensorNone, 0 },
  { M_DRIVE_LEFT2,    kVexMotor393S, kVexMotorNormal,   kVexSensorNone, 0 },
  { M_DRIVE_RIGHT2,   kVexMotor393S, kVexMotorNormal,   kVexSensorNone, 0 }
};

/*----------------------CONTROLLERS FOR FLYWHEELS AND MOTORS------------------------*/


// TBH Controllers
TBHController *flyWheelCtrl;

//PID Controllers
EPidController *rightDrive;
EPidController *leftDrive;

//Speedometer
Speedometer* speedometer;
double maxSpeed = 0;



/*----------------------------MISCELLANEOUS FUNCTIONS-------------------------------*/

// Line Folower
LineFollower *lfol;
const int16_t lfolThresholds[5] = {300, 300, 300, 300, 300};
const float lfolDrives[5] = {-1, -0.8, -0.7, 0.6, 0};

bool isBlue(void) {
  return (vexAdcGet(S_COLOR_SELECTOR) < 2000);
}

int getMode(void){
  int mode = 0;
  if(vexAdcGet(S_COLOR_SELECTOR) < 400){
    mode = RED_DEFENSIVE;
  } else if (vexAdcGet(S_COLOR_SELECTOR) >=400 && vexAdcGet(S_COLOR_SELECTOR) < 1600){
    mode = RED_AGGRESSIVE;
  } else if (vexAdcGet(S_COLOR_SELECTOR) >=1600 && vexAdcGet(S_COLOR_SELECTOR) < 2400){
    mode = BLUE_DEFENSIVE;
  } else if (vexAdcGet(S_COLOR_SELECTOR) >=2400 && vexAdcGet(S_COLOR_SELECTOR) < 3600){
    mode = BLUE_AGGRESSIVE;
  } else if (vexAdcGet(S_COLOR_SELECTOR) >=3600){
    mode = SKILLS;
  }
  vex_printf("Mode is: %d \n", mode);
  return mode;
}

bool isLineOn(int sensor) {
  return (vexAdcGet(sensor) < lfolThresholds[(sensor - S_LINE_FOLLOWER_L2)]);
}

bool isAllLineOn(void) {
  int i = 0;
  for(i = 0;i < 5;i++) {
    if(vexAdcGet(i+S_LINE_FOLLOWER_L2) >= lfolThresholds[i]) {
      return false;
    }
  }
  return true;
}

bool isEndsOn(void) {
  return ((isLineOn(S_LINE_FOLLOWER_L2) || isLineOn(S_LINE_FOLLOWER_L1)) &&
          (isLineOn(S_LINE_FOLLOWER_R2) || isLineOn(S_LINE_FOLLOWER_R1)));
}

bool driveMotors(void) {
  short ld, rd ;
  //Calculate Motor Power
  int forward = VALLEY(vexControllerGet(J_DRIVE), 10, 127);
  int turnChannel = vexControllerGet(J_TURN) * 0.7;
  /*
  if(vexControllerGet(J_HALF_SPEED))
  {
	  turnChannel  *= .4;
  }
  */
  int turn  = VALLEY(turnChannel, 10, 127);

  ld = VALLEY(forward + turn, 20, 127);
  rd = VALLEY(forward - turn, 20, 127);


  vexMotorSet(M_DRIVE_LEFT1,  ld);
  vexMotorSet(M_DRIVE_LEFT2,  ld);
  vexMotorSet(M_DRIVE_RIGHT1, rd);
  vexMotorSet(M_DRIVE_RIGHT2, rd);

  return (ld != 0 || rd != 0);
}

bool isBallTop(void) {
   //int32_t sensorValue = ;
   //vex_printf("Sensor value: %d", sensorValue);
   return vexAdcGet(S_BALL_TOP) < 2200;
}

// bool isBallBot(void) {
//   return (vexAdcGet(S_BALL_BOT) < 2880);
// }

void
vexUserSetup()
{
  vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
  vexMotorConfigure( mConfig, MOT_CONFIG_SIZE( mConfig ) );
}

void
vexUserInit()
{
  flyWheelCtrl = TBHControllerInit(S_ENC_FLY, FLY_WHEEL_MID_GAIN, 10500, false);
  flyWheelCtrl->log = false;
  flyWheelCtrl->powerZeroClamp = true;

  //Initialize EPIDControllers
  rightDrive = EPidInit(kMinJerk,0.01,0,0, S_ENC_DRIVE_RIGHT, true);
  leftDrive = EPidInit(kMinJerk,0.01,0,0, S_ENC_DRIVE_LEFT, false);
  speedometer = SpeedometerInit(S_ENC_FLY);

  // init line followers
  lfol = LineFollowerInit(
    5,                   // five sensors
    S_LINE_FOLLOWER_L2,  // starting from the leftmost
    30,                 // max speed
    lfolThresholds,
    lfolDrives
  );
  lfol->log = false;
}


/*------------------------FUNCTIONS AND CONSTANTS FOR AUTON--------------------------------*/


int autonStep = 0;
int autonShootCount = 0;
int autonTurn = -1;
int mode;
bool readyToShoot = false;
systime_t autonTime;
systime_t autonLastTime;
systime_t autonWaitTime;


#define STEP(s) (autonStep == (s) && (autonTime-autonLastTime) > autonWaitTime)
#define WAIT(t) do {autonLastTime = autonTime;autonWaitTime = (t);} while(false);
#define TIMEELAPSED(t) ((autonTime-autonLastTime) > (t))

int rotateClockWise(int startStep, int target, float speed) {
  if(STEP(startStep)) {
    vex_printf("rotateClockWise run\n");
    int32_t duration = (ABS(target)/speed)*1000;
    int32_t waitTime = duration * 1.1;
    EPidEnable(leftDrive, duration, target);
    EPidEnable(rightDrive, duration, -target);
    autonStep++;
    WAIT(waitTime);
  }
  return (startStep + 1);
}

int punchBall(int startStep){
  if(STEP(startStep)) {
    vex_printf("Punch ball\n");
    vexMotorSet(M_PUNCHER, 80);
    autonStep++;
    WAIT(2000);
  }
  return (startStep + 1);
}

int punchContinuously(int startStep){
  if(STEP(startStep)) {
    vex_printf("DON'T STOP PUNCHING!!!\n");
    vexMotorSet(M_PUNCHER, 80);
    autonStep++;
    WAIT(0);
  }
  return (startStep + 1);
}

int fuckAround(int startStep){
  if(STEP(startStep)) {
    vex_printf("fuck around for a bitl\n");
    autonStep++;
    WAIT(3000);
  }
  return (startStep + 1);
}

int shutOffPuncher(int startStep){
  if(STEP(startStep)) {
     vex_printf("Shut off Puncher\n");
    vexMotorSet(M_PUNCHER, 0);
    autonStep++;
    WAIT(0);
  }
  return (startStep + 1);
}

int setFlySpeed(int startStep, int32_t speed, float gain){
  if(STEP(startStep)){
    vex_printf("Setting flywheel speed to: %d \n", speed);
    vexMotorSet(M_FLY_WHEEL, 0);
    tbhEnableWithGain(flyWheelCtrl, speed, gain);
    autonStep++;
    WAIT(0);
  }
  return (startStep + 1); 
}

int shutFlyWheelOff(int startStep){
  if(STEP(startStep)){
    vex_printf("Disabling flywheel\n");
    tbhDisable(flyWheelCtrl);
    autonStep++;
    WAIT(0);
  }
  return (startStep + 1); 
}

int move(int startStep, int target, float speed) {
  if(STEP(startStep)) {
    vex_printf("move run\n");
    int32_t duration = (ABS(target)/speed)*1000;
    int32_t waitTime = duration * 1.5;
    EPidEnable(leftDrive, duration, target);
    EPidEnable(rightDrive, duration, target);
    autonStep++;
    WAIT(waitTime);
  }
  return (startStep + 1);
}

int rotateCWTillLine(int startStep, int motorPower) {
  int sensor;
  if(motorPower > 0) {
    sensor = S_LINE_FOLLOWER_L2;
  } else {
    sensor = S_LINE_FOLLOWER_R2;
  }
  if(STEP(startStep)) {
    vex_printf("rotateCWTillLine run\n");
    EPidDisable(leftDrive);
    EPidDisable(rightDrive);
	  vexMotorSet(M_DRIVE_LEFT1, motorPower);
	  vexMotorSet(M_DRIVE_LEFT2, motorPower);
	  vexMotorSet(M_DRIVE_RIGHT1, -motorPower);
	  vexMotorSet(M_DRIVE_RIGHT2, -motorPower);
    autonStep++;
    WAIT(0);
  }
  if(STEP(startStep+1) && isLineOn(sensor)) {
    vex_printf("rotateCWTillLine lineup\n");
    EPidEnable(leftDrive, 2000, 0);
    EPidEnable(rightDrive, 2000, 0);
    autonStep++;
    WAIT(2500);
  }
  return (startStep + 2);
}

int rotateCCWTillLine(int startStep, int motorPower) {
  int sensor;
  if(motorPower > 0) {
    sensor = S_LINE_FOLLOWER_L2;
  } else {
    sensor = S_LINE_FOLLOWER_R2;
  }
  if(STEP(startStep)) {
    vex_printf("rotateCCWTillLine run\n");
    EPidDisable(leftDrive);
    EPidDisable(rightDrive);
    vexMotorSet(M_DRIVE_LEFT1, -motorPower);
    vexMotorSet(M_DRIVE_LEFT2, -motorPower);
    vexMotorSet(M_DRIVE_RIGHT1, motorPower);
    vexMotorSet(M_DRIVE_RIGHT2, motorPower);
    autonStep++;
    WAIT(0);
  }
  if(STEP(startStep+1) && isLineOn(sensor)) {
    vex_printf("rotateCCWTillLine lineup\n");
    EPidEnable(leftDrive, 2000, 0);
    EPidEnable(rightDrive, 2000, 0);
    autonStep++;
    WAIT(2500);
  }
  return (startStep + 2);
}

int restDown(int startStep) {
  if(STEP(startStep)) {
    // rest
    vex_printf("rest!!\n");
    EPidDisable(leftDrive);
    EPidDisable(rightDrive);
    LineFollowerDisable(lfol);
    vexMotorSet(M_DRIVE_LEFT1, 0);
    vexMotorSet(M_DRIVE_LEFT2, 0);
    vexMotorSet(M_DRIVE_RIGHT1, 0);
    vexMotorSet(M_DRIVE_RIGHT2, 0);
    autonStep++;
    WAIT(2000);
  }
  return (startStep + 1);
}

int punch(int startStep) {
  if(STEP(startStep)) {
    vex_printf("punch!! punch!!\n");
    EPidDisable(leftDrive);
    EPidDisable(rightDrive);
	  vexMotorSet(M_DRIVE_LEFT1, 127);
	  vexMotorSet(M_DRIVE_LEFT2, 127);
	  vexMotorSet(M_DRIVE_RIGHT1, 127);
	  vexMotorSet(M_DRIVE_RIGHT2, 127);
    autonStep++;
    WAIT(3000);
  }
  return (startStep + 1);
}

int backwardsPunch(int startStep) {
  if(STEP(startStep)) {
    vex_printf("punch!! punch!!\n");
    EPidDisable(leftDrive);
    EPidDisable(rightDrive);
    vexMotorSet(M_DRIVE_LEFT1,-127);
    vexMotorSet(M_DRIVE_LEFT2, -127);
    vexMotorSet(M_DRIVE_RIGHT1, -127);
    vexMotorSet(M_DRIVE_RIGHT2, -127);
    autonStep++;
    WAIT(3000);
  }
  return (startStep + 1);
}

#define TILLMIDDLE 0
#define TILLENDS   1
#define SOMETIME 2

int lineFollow(int startStep, int end) {
  bool endCondition = false;
  if(end == TILLMIDDLE) {
    endCondition = isAllLineOn();
  } else if(end == TILLENDS) {
    endCondition = isEndsOn();
  } else if(end == SOMETIME) {
    endCondition = true;
  }

  if(STEP(startStep)) {
    vex_printf("lineFollowTillMiddle start\n");
    EPidDisable(leftDrive);
    EPidDisable(rightDrive);
    LineFollowerEnable(lfol);
    autonStep++;
    if(end == SOMETIME) {
      WAIT(2000);
    } else {
      WAIT(0);
    }
  }
  if(STEP(startStep+1) && endCondition) {
    vex_printf("lineFollowTillMiddle start\n");
    LineFollowerDisable(lfol);
    EPidEnable(leftDrive, 2000, 0);
    EPidEnable(rightDrive, 2000, 0);
    autonStep++;
    WAIT(2500);
  }
  return (startStep + 2);
}

int shootAllBalls(int startStep, int32_t time){
   if(STEP(startStep)) {
    vex_printf("Shooting balls!\n");
    readyToShoot = true;
    autonStep++;
    WAIT(time);
  }
  return (startStep + 1);
}

int setReadyToShootFalse(int startStep){
   if(STEP(startStep)) {
    readyToShoot = false;
    autonStep++;
    WAIT(0);
  }
  return (startStep + 1);
}
/**
 * 4 step routine that shoots N Balls
 
int shootNBalls(int startStep, int n, int failSafeStep) {
    failSafeStep = failSafeStep < 0 ? (startStep+5) : failSafeStep;

    if(STEP(startStep)) {
      vex_printf("shootNBalls step Init\n", autonStep);
      autonShootCount = 0;
      vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
      autonStep++;
      WAIT(0);
    }
    if(STEP(startStep+1)) {
      vex_printf("shootNBalls step Feed through\n", autonStep);
      vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
      autonStep++;
      WAIT(0);
    }
    if(autonStep == (startStep+2) && TIMEELAPSED(AUTON_FEED_FAIL_TIME)) {
      vex_printf("shootNBalls Failsafe\n");
      //fail safe
      autonShootCount = 0;
      vexMotorSet(M_FEED_SHOOT, 0);
      vexMotorSet(M_FEED_FRONT, 0);
      autonStep = failSafeStep;
    }
    if(STEP(startStep+2) && isBallTop()) {
      vex_printf("shootNBalls Shoot\n");
      autonStep++;
      WAIT(500);
    }
    if (STEP(startStep+3)) {
      vex_printf("shootNBalls Tiny Backup\n");
      vexMotorSet(M_FEED_FRONT, -DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, -DEFAULT_FEED_SPEED);
      autonStep++;
      WAIT(100);
    }
    if (STEP(startStep+4)) {
      vex_printf("shootNBalls Repeat step=%d ballCount=%d\n", autonStep, autonShootCount);
      vexMotorSet(M_FEED_FRONT, 0);
      vexMotorSet(M_FEED_SHOOT, 0);
      autonShootCount++;
      if(autonShootCount == n) {
        autonShootCount = 0;
        autonStep++;
      } 
      else {
        autonStep = startStep+1;
        WAIT(1000);
      }
    }
    return (startStep + 5);
}
*/





/*-------------------------------------AUTONOMOUS-----------------------------------*/



msg_t
vexAutonomous( void *arg )
{
  (void)arg;
  vexTaskRegister("auton");

  //Reset sensors
  vexSensorValueSet(S_ENC_DRIVE_LEFT, 0);
  vexSensorValueSet(S_ENC_DRIVE_RIGHT, 0);

  autonTurn = 1;
  mode = getMode();
  autonStep = 0;
  autonShootCount = 0;
  autonTime = chTimeNow();
  autonWaitTime = 0;
  readyToShoot = false;

  vex_printf("starting autonomous\n");
  tbhEnable(flyWheelCtrl, FLY_MID_SPEED);
  //vex_printf("Starting speed at %d", flyWheelCtrl->targetSpeed);
  //WAIT(2000);

  int nextStep;
  #define RUNSTEP(stepName, ...) nextStep = stepName(nextStep, ##__VA_ARGS__)

  //vex_printf("enc=%d,%d\n", vexSensorValueGet(S_ENC_DRIVE_LEFT), vexSensorValueGet(S_ENC_DRIVE_RIGHT));
  while(!chThdShouldTerminate())
  {
    autonTime = chTimeNow();

    
  if(vexControllerGet(J_STOP_AUTON)) {
  		break;
  }

    if(!isBallTop() || readyToShoot == true){
      //Run feeds
      vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
    } else {
      vexMotorSet(M_FEED_FRONT, 0);
      vexMotorSet(M_FEED_SHOOT, 0);
    }

    nextStep = 0;

    if(mode == RED_DEFENSIVE){
        RUNSTEP(punchBall);
        RUNSTEP(shutOffPuncher);
        RUNSTEP(move, 1950, 500);
        RUNSTEP(rotateClockWise, 60*autonTurn, 60);
        RUNSTEP(shootAllBalls, 2750);
        RUNSTEP(setFlySpeed, FLY_PB_SPEED, FLY_WHEEL_PB_GAIN);
        RUNSTEP(setReadyToShootFalse);
        RUNSTEP(rotateClockWise, 100*autonTurn, 100);
        RUNSTEP(move, 500, 350);
        RUNSTEP(rotateClockWise, -120*autonTurn, 120);
        RUNSTEP(setReadyToShootFalse);
        RUNSTEP(move, 1200, 500);
        RUNSTEP(shootAllBalls, 2750);
        RUNSTEP(setFlySpeed, FLY_QUARTER_SPEED, FLY_WHEEL_QUARTER_GAIN);
        RUNSTEP(setReadyToShootFalse);
        RUNSTEP(move, -300, 250);
        RUNSTEP(rotateClockWise, 300*autonTurn, 300);
        RUNSTEP(move, 1200, 400);
        RUNSTEP(rotateClockWise, -395*autonTurn, 395);
        RUNSTEP(move, 150, 70);
        RUNSTEP(shootAllBalls, 2750);
        RUNSTEP(setReadyToShootFalse);
        RUNSTEP(rotateClockWise, 205*autonTurn, 205);
        RUNSTEP(move, 400, 150);
        RUNSTEP(move, -180, 100);
        RUNSTEP(move, 180,100);
        RUNSTEP(move, -200,100);
        RUNSTEP(rotateClockWise, -210*autonTurn, 210);
        RUNSTEP(shootAllBalls, 2750);
    }

    else if(mode == BLUE_DEFENSIVE){
        autonTurn = -1;
        RUNSTEP(punchBall);
        RUNSTEP(shutOffPuncher);
        RUNSTEP(move, 1950, 500);
        //RUNSTEP(rotateClockWise, 60*autonTurn, 60);
        RUNSTEP(shootAllBalls, 2750);
        RUNSTEP(setFlySpeed, FLY_PB_SPEED, FLY_WHEEL_PB_GAIN);
        RUNSTEP(setReadyToShootFalse);
        RUNSTEP(rotateClockWise, 90*autonTurn, 90);
        RUNSTEP(move, 250, 200);
        RUNSTEP(rotateClockWise, -100*autonTurn, 100);
        RUNSTEP(setReadyToShootFalse);
        RUNSTEP(move, 1350, 550);
        RUNSTEP(shootAllBalls, 2750);
        RUNSTEP(setFlySpeed, FLY_QUARTER_SPEED, FLY_WHEEL_QUARTER_GAIN);
        RUNSTEP(setReadyToShootFalse);
        RUNSTEP(move, -300, 250);
        RUNSTEP(rotateClockWise, 300*autonTurn, 300);
        RUNSTEP(move, 1200, 400);
        RUNSTEP(rotateClockWise, -395*autonTurn, 395);
        RUNSTEP(move, 150, 70);
        RUNSTEP(shootAllBalls, 2750);
        RUNSTEP(setReadyToShootFalse);
        RUNSTEP(rotateClockWise, 205*autonTurn, 205);
        RUNSTEP(move, 400, 150);
        RUNSTEP(move, -180, 100);
        RUNSTEP(move, 180,100);
        RUNSTEP(move, -200,100);
        RUNSTEP(rotateClockWise, -210*autonTurn, 210);
        RUNSTEP(shootAllBalls, 2750);
    }
    else if(mode == SKILLS){
        RUNSTEP(setFlySpeed, FLY_SIDE_SPEED, FLY_WHEEL_SIDE_GAIN);
        RUNSTEP(punchContinuously);
        RUNSTEP(shootAllBalls, 24000);
        RUNSTEP(shutOffPuncher);
        RUNSTEP(rotateClockWise, 205, 205);
        RUNSTEP(move, 4000, 600);
        RUNSTEP(rotateClockWise, -205, 205);
        RUNSTEP(punchContinuously);
        RUNSTEP(shootAllBalls, 14000);
        RUNSTEP(move, 500, 300);
        RUNSTEP(fuckAround);
        RUNSTEP(move, -600, 500);
        RUNSTEP(restDown);
        RUNSTEP(backwardsPunch);
    }



    //vex_printf("motorPower at: %d", flyWheelCtrl->motorPower);
    if(!isBallTop() || readyToShoot == true){
      //Run feeds
      vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
    } else {
      vexMotorSet(M_FEED_FRONT, 0);
      vexMotorSet(M_FEED_SHOOT, 0);
    }

    vexMotorSet(M_FLY_WHEEL, tbhUpdate(flyWheelCtrl));

    if(STEP(nextStep)) {
     // vex_printf("Exit step %d\n", autonStep);
      break;
    }

    LineFollowerUpdate(lfol);
  	int16_t motorValL = EPidUpdate(leftDrive);
  	int16_t motorValR = EPidUpdate(rightDrive);
    if(leftDrive->pidc->enabled) {
  	  vexMotorSet(M_DRIVE_LEFT1, motorValL);
  	  vexMotorSet(M_DRIVE_LEFT2, motorValL);
    }
    if(rightDrive->pidc->enabled) {
  	  vexMotorSet(M_DRIVE_RIGHT1, motorValR);
  	  vexMotorSet(M_DRIVE_RIGHT2, motorValR);
    }
    if(lfol->enabled) {
  	  vexMotorSet(M_DRIVE_LEFT1, lfol->leftDrive);
  	  vexMotorSet(M_DRIVE_LEFT2, lfol->leftDrive);
  	  vexMotorSet(M_DRIVE_RIGHT1, lfol->rightDrive);
  	  vexMotorSet(M_DRIVE_RIGHT2, lfol->rightDrive);
    }
    vexSleep( 10 );
	}


  tbhDisable(flyWheelCtrl);
  EPidDisable(leftDrive);
  EPidDisable(rightDrive);
  vexMotorSet(M_FEED_FRONT, 0);
  vexMotorSet(M_FEED_SHOOT, 0);
  vexMotorSet(M_PUNCHER, 0);
  vexMotorSet(M_FLY_WHEEL, 0);
  vex_printf("Exit autonomous");
  return (msg_t)0;
}



/*---------------------------------------DRIVER CONTROL ----------------------------------------*/



msg_t
vexOperator( void *arg )
{
  (void)arg;
  // Must call this
  vexTaskRegister("operator");

  //Speedometer *spdm = SpeedometerInit(S_ENC_FLY);
  //double maxSpeed = 0;
  // Run until asked to terminate


  while(!chThdShouldTerminate())
  {
     bool motorRunning = driveMotors();

     //Auton testing
     if(vexControllerGet(J_START_AUTON)){
      vexAutonomous(NULL); 
     }

     if(vexControllerGet(J_SHOOT_SIDE)) { 
       tbhEnableWithGain(flyWheelCtrl, FLY_SIDE_SPEED, FLY_WHEEL_SIDE_GAIN); 
     } 
     if(vexControllerGet(J_SHOOT_PB)) { 
       tbhEnableWithGain(flyWheelCtrl, FLY_PB_SPEED, FLY_WHEEL_PB_GAIN); 
     } 
     if(vexControllerGet(J_SHOOT_QUARTER)) { 
       tbhEnableWithGain(flyWheelCtrl, FLY_QUARTER_SPEED, FLY_WHEEL_QUARTER_GAIN); 
     } 
     if(vexControllerGet(J_SHOOT_MID)) { 
       tbhEnableWithGain(flyWheelCtrl, FLY_MID_SPEED, FLY_WHEEL_MID_GAIN); 
     } 
    
     if(vexControllerGet(J_SHOOT_STOP)) { 
       tbhDisable(flyWheelCtrl); 
     }

     //if(isBallTop()){
     // vex_printf("Ball is top\n");
     //} 


    vexMotorSet(M_FLY_WHEEL, tbhUpdate(flyWheelCtrl));

    //Code for measuring Max Speed
    /*vexMotorSet(M_FLY_WHEEL, vexControllerGet(J_DRIVE));
    double speed = SpeedometerUpdate(spdm);
    if(speed > maxSpeed) {
        maxSpeed = speed;
    }

    vex_printf("maxSpeed=%f\n", maxSpeed);
    */

    // Shoot Feed
    if((vexControllerGet(J_FEED_FRONT_U) || vexControllerGet(J_FEED_SHOOT_U)) /*&& !isBallTop()*/) {
       vexMotorSet(M_FEED_FRONT, 60);
    } else if((/*vexControllerGet(J_FEED_FRONT_D) ||*/ vexControllerGet(J_FEED_SHOOT_D)) /*&& !isBallTop()*/) {
       vexMotorSet(M_FEED_FRONT, -60);
    } else if(vexControllerGet(J_COMPRESS)) {
        vexMotorSet(M_FEED_SHOOT, -40);
    } else {
       vexMotorSet(M_FEED_FRONT, 0);
    }

    // Front Feed
    if(vexControllerGet(J_FEED_SHOOT_U)) {
       vexMotorSet(M_FEED_SHOOT, 90);
    } else if(vexControllerGet(J_FEED_SHOOT_D)) {
       vexMotorSet(M_FEED_SHOOT, -90);
    //} else if(!isBallTop() && isBallBot()) {
       //vexMotorSet(M_FEED_FRONT, 100);
    } else if(vexControllerGet(J_COMPRESS)){
        vexMotorSet(M_FEED_FRONT, 60);
    } else {
       vexMotorSet(M_FEED_SHOOT, 0);
    }


    if (vexControllerGet(J_PUNCH)){
      vexMotorSet(M_PUNCHER, 80);
    } 
    else
    {
      vexMotorSet(M_PUNCHER, 0);
    }

    vexSleep( 10 );
  }

  return (msg_t)0;
}
