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

#define M_FLY_BOT_WHEEL      kVexMotor_5
#define M_PUNCHER            kVexMotor_6

#define M_FEED_FRONT      kVexMotor_2

#define M_DRIVE_RIGHT1    kVexMotor_4
#define M_DRIVE_RIGHT2    kVexMotor_8

#define M_DRIVE_LEFT1     kVexMotor_3
#define M_DRIVE_LEFT2     kVexMotor_7

#define M_FEED_SHOOT      kVexMotor_9

// Sensor channels
#define P_ENC_BOT_FLY_A       kVexDigital_2
#define P_ENC_BOT_FLY_B       kVexDigital_1

#define P_ENC_TOP_FLY_A       kVexDigital_3
#define P_ENC_TOP_FLY_B       kVexDigital_4

#define P_ENC_DRIVE_RIGHT_A   kVexDigital_7
#define P_ENC_DRIVE_RIGHT_B   kVexDigital_8

#define P_ENC_DRIVE_LEFT_A    kVexDigital_5
#define P_ENC_DRIVE_LEFT_B    kVexDigital_6


#define S_BALL_BOT              1
#define S_BALL_TOP              2
#define S_COLOR_SELECTOR        0

#define S_LINE_FOLLOWER_L2      3
#define S_LINE_FOLLOWER_L1      4
#define S_LINE_FOLLOWER_M       5
#define S_LINE_FOLLOWER_R1      6
#define S_LINE_FOLLOWER_R2      7


#define S_ENC_BOT_FLY         kVexSensorDigital_1
#define S_ENC_TOP_FLY         kVexSensorDigital_4
#define S_ENC_DRIVE_RIGHT     kVexSensorDigital_8
#define S_ENC_DRIVE_LEFT      kVexSensorDigital_6


// Joystick settings
#define J_DRIVE      Ch3
#define J_TURN       Ch1
//#define J_SHOOT      Btn6U

#define J_SHOOT_START   Btn7L
#define J_SHOOT_CLOSE   Btn8U
#define J_SHOOT_PB      Btn8L
#define J_SHOOT_SIDE    Btn7U
#define J_SHOOT_MID     Btn8R
#define J_SHOOT_STOP   	Btn8D

#define J_START_AUTON	Btn7R
#define J_STOP_AUTON	Btn5U


#define J_FEED_SHOOT_U 	Btn6U
#define J_FEED_SHOOT_D  Btn6D
#define J_FEED_FRONT_U 	Btn5U

#define J_HALF_SPEED 	Btn5D

#define J_PUNCH       Btn7R

// Constants
#define DEFAULT_FEED_SPEED 100
#define FEED_SPOOL_TIME 100

#define TOP_FLY_WHEEL_DEFAULT_GAIN  0.05
#define BOT_FLY_WHEEL_DEFAULT_GAIN  0.05

#define TOP_FLY_WHEEL_LOW_GAIN  0.05
#define BOT_FLY_WHEEL_LOW_GAIN  0.05

#define TOP_FLY_WHEEL_PB_GAIN  0.005
#define BOT_FLY_WHEEL_PB_GAIN  0.005

#define FLY_START_SPEED  7600
#define FLY_SIDE_SPEED 	 6450
#define FLY_PB_SPEED   	 4900
#define FLY_CLOSE_SPEED  6100
#define FLY_MID_SPEED  	 5000

#define AUTON_FEED_FAIL_TIME 2500


// Digi IO configuration
static  vexDigiCfg  dConfig[] = {
  { P_ENC_BOT_FLY_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_1},
  { P_ENC_BOT_FLY_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_1},

  { P_ENC_TOP_FLY_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_2},
  { P_ENC_TOP_FLY_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_2},

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

  { M_PUNCHER,  kVexMotor393T, kVexMotorNormal, kVexSensorQuadEncoder, kVexQuadEncoder_2 },
  { M_FLY_BOT_WHEEL,  kVexMotor393T, kVexMotorNormal, kVexSensorQuadEncoder, kVexQuadEncoder_1 },

  { M_FEED_FRONT,     kVexMotor393S, kVexMotorReversed, kVexSensorNone, 0 },
  { M_DRIVE_LEFT2,    kVexMotor393S, kVexMotorNormal,   kVexSensorNone, 0 },
  { M_DRIVE_RIGHT2,   kVexMotor393S, kVexMotorNormal,   kVexSensorNone, 0 }
};


// TBH Controllers
TBHController *topWheelCtrl;
TBHController *botWheelCtrl;

//PID Controllers
EPidController *rightDrive;
EPidController *leftDrive;

// Line Folower
LineFollower *lfol;
const int16_t lfolThresholds[5] = {300, 300, 300, 300, 300};
const float lfolDrives[5] = {-1, -0.8, -0.7, 0.6, 0};

bool isBlue(void) {
  return (vexAdcGet(S_COLOR_SELECTOR) < 2000);
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
  if(vexControllerGet(J_HALF_SPEED))
  {
	  turnChannel  *= .4;
  }
  int turn  = VALLEY(turnChannel, 10, 127);

  ld = VALLEY(forward + turn, 45, 127);
  rd = VALLEY(forward - turn, 45, 127);


  vexMotorSet(M_DRIVE_LEFT1,  ld);
  vexMotorSet(M_DRIVE_LEFT2,  ld);
  vexMotorSet(M_DRIVE_RIGHT1, rd);
  vexMotorSet(M_DRIVE_RIGHT2, rd);

  return (ld != 0 || rd != 0);
}

bool isBallTop(void) {
  return (vexAdcGet(S_BALL_TOP) < 2200);
}

bool isBallBot(void) {
  return (vexAdcGet(S_BALL_BOT) < 2880);
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
  topWheelCtrl = TBHControllerInit(S_ENC_TOP_FLY, TOP_FLY_WHEEL_DEFAULT_GAIN, 10500, false);
  topWheelCtrl->powerZeroClamp = true;
  topWheelCtrl->log = false;

  botWheelCtrl = TBHControllerInit(S_ENC_BOT_FLY, TOP_FLY_WHEEL_DEFAULT_GAIN, 10500, false);
  botWheelCtrl->log = false;
  botWheelCtrl->powerZeroClamp = true;

  //Initialize EPIDControllers
  rightDrive = EPidInit(kMinJerk,0.01,0,0,S_ENC_DRIVE_RIGHT, true);
  leftDrive = EPidInit(kMinJerk,0.01,0,0,S_ENC_DRIVE_LEFT, true);


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

int autonStep = 0;
int autonShootCount = 0;
int autonTurn = 1;
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

/**
 * 4 step routine that shoots N Balls
 */
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

msg_t
vexAutonomous( void *arg )
{
  (void)arg;
  //vexTaskRegister("auton");

  vexSensorValueSet(S_ENC_DRIVE_LEFT, 0);
  vexSensorValueSet(S_ENC_DRIVE_RIGHT, 0);

  autonTurn = isBlue()?1:-1;
  autonStep = 0;
  autonShootCount = 0;
  autonTime = chTimeNow();
  autonWaitTime = 0;

  vex_printf("starting autonomous\n");
  tbhEnable(topWheelCtrl, FLY_SIDE_SPEED);
  tbhEnable(botWheelCtrl, FLY_SIDE_SPEED);
  WAIT(2000);

  int nextStep;
  #define RUNSTEP(stepName, ...) nextStep = stepName(nextStep, ##__VA_ARGS__)

  vex_printf("enc=%d,%d\n", vexSensorValueGet(S_ENC_DRIVE_LEFT), vexSensorValueGet(S_ENC_DRIVE_RIGHT));
  while(!chThdShouldTerminate())
  {
    autonTime = chTimeNow();
  	if(vexControllerGet(J_STOP_AUTON)) {
  		break;
  	}

    nextStep = 0;
    RUNSTEP(shootNBalls, 8, -1);
    RUNSTEP(rotateClockWise, 475, 300);
    RUNSTEP(move, -2020, 300);
    RUNSTEP(rotateCWTillLine, -35);
    RUNSTEP(lineFollow, TILLMIDDLE);
    RUNSTEP(move, -100, 100);
    RUNSTEP(lineFollow, TILLENDS);
    RUNSTEP(rotateClockWise, 300, 300);
    RUNSTEP(rotateCWTillLine, 35);
    RUNSTEP(rotateClockWise, 30, 300);
    //RUNSTEP(lineFollow, SOMETIME);
    RUNSTEP(move, 700, 400);
    RUNSTEP(restDown);
    RUNSTEP(punch);
    if(STEP(nextStep)) {
      vex_printf("Exit step %d\n", autonStep);
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
    vexMotorSet(M_PUNCHER, tbhUpdate(topWheelCtrl));
    vexMotorSet(M_FLY_BOT_WHEEL, tbhUpdate(botWheelCtrl));
    vexSleep( 10 );
	}


  tbhDisable(topWheelCtrl);
  tbhDisable(botWheelCtrl);
  EPidDisable(leftDrive);
  EPidDisable(rightDrive);
  vexMotorSet(M_FEED_FRONT, 0);
  vexMotorSet(M_FEED_SHOOT, 0);
  vexMotorSet(M_PUNCHER, 0);
  vexMotorSet(M_FLY_BOT_WHEEL, 0);
  vex_printf("Exit autonomous");
  return (msg_t)0;
}


msg_t
vexOperator( void *arg )
{
  (void)arg;
  // Must call this
  vexTaskRegister("operator");


  // Run until asked to terminate
  while(!chThdShouldTerminate())
  {
    // if(vexControllerGet(Btn8U)) {
    //   int16_t val = 20;
    //   vexMotorSet(M_DRIVE_LEFT2, val);
    //   vexMotorSet(M_DRIVE_LEFT1, val);
    //   vexMotorSet(M_DRIVE_RIGHT1, val);
    //   vexMotorSet(M_DRIVE_RIGHT2, val);
    // } else {
    //   vexMotorSet(M_DRIVE_LEFT2, 0);
    //   vexMotorSet(M_DRIVE_LEFT1, 0);
    //   vexMotorSet(M_DRIVE_RIGHT1, 0);
    //   vexMotorSet(M_DRIVE_RIGHT2, 0);
    // }
    // continue;
    driveMotors();

    //vex_printf("right=%d left=%d\n", vexSensorValueGet(S_ENC_DRIVE_RIGHT), vexSensorValueGet(S_ENC_DRIVE_LEFT));
    //Test autonomous
  	 // if(vexControllerGet(J_START_AUTON))
  	 // {
  	 // 	vexAutonomous(NULL);
  	 // }


    if(vexControllerGet(J_SHOOT_PB)) {
      tbhEnableWithGain(topWheelCtrl, FLY_PB_SPEED, TOP_FLY_WHEEL_PB_GAIN);
      tbhEnableWithGain(botWheelCtrl, FLY_PB_SPEED, BOT_FLY_WHEEL_PB_GAIN);
    }
    if(vexControllerGet(J_SHOOT_START)) {
      tbhEnableWithGain(topWheelCtrl, FLY_START_SPEED, TOP_FLY_WHEEL_DEFAULT_GAIN);
      tbhEnableWithGain(botWheelCtrl, FLY_START_SPEED, TOP_FLY_WHEEL_DEFAULT_GAIN);
    }
    if(vexControllerGet(J_SHOOT_CLOSE)) {
      tbhEnableWithGain(topWheelCtrl, FLY_CLOSE_SPEED, TOP_FLY_WHEEL_DEFAULT_GAIN);
      tbhEnableWithGain(botWheelCtrl, FLY_CLOSE_SPEED, TOP_FLY_WHEEL_DEFAULT_GAIN);
    }
    if(vexControllerGet(J_SHOOT_SIDE)) {
      tbhEnableWithGain(topWheelCtrl, FLY_SIDE_SPEED, TOP_FLY_WHEEL_LOW_GAIN);
      tbhEnableWithGain(botWheelCtrl, FLY_SIDE_SPEED, TOP_FLY_WHEEL_LOW_GAIN);
    }
    if(vexControllerGet(J_SHOOT_STOP)) {
      tbhDisable(topWheelCtrl);
      tbhDisable(botWheelCtrl);
    }

    // vexMotorSet(M_PUNCHER, tbhUpdate(topWheelCtrl));
    vexMotorSet(M_FLY_BOT_WHEEL, tbhUpdate(botWheelCtrl));

    // Shoot Feed
    if((vexControllerGet(J_FEED_FRONT_U) || vexControllerGet(J_FEED_SHOOT_U)) /*&& !isBallTop()*/) {
       vexMotorSet(M_FEED_FRONT, 60);
    } else if((/*vexControllerGet(J_FEED_FRONT_D) ||*/ vexControllerGet(J_FEED_SHOOT_D)) /*&& !isBallTop()*/) {
       vexMotorSet(M_FEED_FRONT, -60);
    //} else if(!isBallTop() && isBallBot()) {
      // vexMotorSet(M_FEED_SHOOT, 100);
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
    } else {
       vexMotorSet(M_FEED_SHOOT, 0);
    }

    if (vexControllerGet(J_PUNCH)){
      vexMotorSet(M_PUNCHER, 80);
    } 
    else if (vexControllerGet(J_STOP_AUTON))
    {
      vexMotorSet(M_PUNCHER, 0);
    }



    vexSleep( 10 );
  }

  return (msg_t)0;
}
