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
#define M_DRIVE_RIGHT1    kVexMotor_7

#define M_FLY_BOT_WHEEL      kVexMotor_5
#define M_FLY_TOP_WHEEL      kVexMotor_6

#define M_FEED_FRONT      kVexMotor_2
#define M_DRIVE_RIGHT2    kVexMotor_4
#define M_DRIVE_LEFT1     kVexMotor_3
#define M_DRIVE_LEFT2     kVexMotor_8
#define M_FEED_SHOOT      kVexMotor_9

// Sensor channels
#define P_PISTON              kVexDigital_9

#define P_ENC_BOT_FLY_A       kVexDigital_3
#define P_ENC_BOT_FLY_B       kVexDigital_4

#define P_ENC_TOP_FLY_A       kVexDigital_1
#define P_ENC_TOP_FLY_B       kVexDigital_2

#define P_ENC_DRIVE_RIGHT_A   kVexDigital_7
#define P_ENC_DRIVE_RIGHT_B   kVexDigital_8

#define P_ENC_DRIVE_LEFT_A    kVexDigital_5
#define P_ENC_DRIVE_LEFT_B    kVexDigital_6

#define S_BALL_BOT              0
#define S_BALL_TOP              1
#define S_COLOR_SELECTOR		2

#define S_ENC_BOT_FLY         kVexSensorDigital_4
#define S_ENC_TOP_FLY         kVexSensorDigital_2
#define S_ENC_DRIVE_RIGHT     kVexSensorDigital_8
#define S_ENC_DRIVE_LEFT      kVexSensorDigital_6

// Joystick settings
#define J_DRIVE      Ch3
#define J_TURN       Ch1


#define J_SHOOT_START   Btn8R
#define J_SHOOT_MID     Btn8U

#define J_SHOOT_CORNER	Btn7U
#define J_SHOOT_STOP    Btn8D

#define J_START_AUTON	Btn7R

#define J_SHOOT_LESS      Btn8L
#define J_PISTON 		Btn7D

#define J_FEED_SHOOT_U  Btn6U
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
  { M_DRIVE_LEFT1,    kVexMotor393S,           kVexMotorReversed,     kVexSensorIME,         kImeChannel_2 },
  { M_DRIVE_RIGHT1,   kVexMotor393S,           kVexMotorReversed,     kVexSensorIME,         kImeChannel_1 },
  { M_FEED_SHOOT,     kVexMotor393S,           kVexMotorReversed,     kVexSensorNone,        0 },

  { M_FLY_BOT_WHEEL,     kVexMotor393T,      kVexMotorNormal,     kVexSensorQuadEncoder, kVexQuadEncoder_1 },
  { M_FLY_TOP_WHEEL,     kVexMotor393T,      kVexMotorNormal,     kVexSensorQuadEncoder, kVexQuadEncoder_2 },

  { M_FEED_FRONT,     kVexMotor393S,           kVexMotorNormal,   	  kVexSensorNone,        0 },
  { M_DRIVE_LEFT2,    kVexMotor393S,           kVexMotorReversed,     kVexSensorNone,        0 },
  { M_DRIVE_RIGHT2,   kVexMotor393S,           kVexMotorReversed,     kVexSensorNone,        0 }
};

// TBH Controllers
TBHController *botWheelCtrl;
TBHController *topWheelCtrl;

//PID Controllers
EPidController *rightDrive;
EPidController *leftDrive;

bool driveMotors(void) {
  short ld, rd ;
  //Calculate Motor Power
  int forward = VALLEY(vexControllerGet(J_DRIVE), 25, 127);
  int turn;

  turn = VALLEY(vexControllerGet(J_TURN), 25, 127);
  ld = VALLEY(forward + turn, 25, 127);
  rd = VALLEY(forward - turn, 25, 127);

  vexMotorSet(M_DRIVE_LEFT1,  ld);
  vexMotorSet(M_DRIVE_LEFT2,  ld);
  vexMotorSet(M_DRIVE_RIGHT1, rd);
  vexMotorSet(M_DRIVE_RIGHT2, rd);

  return (ld != 0 || rd != 0);
}


bool isBallTop(void) {
  return (vexAdcGet(S_BALL_TOP) < 1000);
}

bool isBallTopAuton(void) {
  return (vexAdcGet(S_BALL_TOP) < 1500);
}

bool isBallBot(void) {
  return (vexAdcGet(S_BALL_BOT) < 2500);
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
  topWheelCtrl = TBHControllerInit(S_ENC_TOP_FLY, 0.01, 10500, true);
  topWheelCtrl->powerZeroClamp = true;
  topWheelCtrl->log = false;
  botWheelCtrl = TBHControllerInit(S_ENC_BOT_FLY, 0.01, 10500, true);
  botWheelCtrl->log = false;
  botWheelCtrl->powerZeroClamp = true;
  vex_printf("puTTy Test");

  //Initialize EPIDControllers
  rightDrive = EPidInit(kMinJerk,0.01,0,0.01,S_ENC_DRIVE_RIGHT, true);
  rightDrive->log = false;
  leftDrive = EPidInit(kMinJerk,0.01,0,0.01,S_ENC_DRIVE_LEFT, false);
}

#include "auton.c"
#include "vex_operator.c"
