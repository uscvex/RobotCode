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

#include "smallbot_defineval.cpp"
#include "val.cpp"
#define DEFAULT_FEED_SPEED 100
#define FEED_SPOOL_TIME 100

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

  { M_FLY_TOP_WHEEL,  kVexMotor393T, kVexMotorNormal, kVexSensorQuadEncoder, kVexQuadEncoder_2 },
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

#include "SmallBot_Sensor.cpp"

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

#include "auton.cpp"
#include "vex_op.cpp"
