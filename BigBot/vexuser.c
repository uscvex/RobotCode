#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"
#include "../Common/takebackhalf.h"

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

#define S_ENC_BOT_FLY         kVexSensorDigital_4
#define S_ENC_TOP_FLY         kVexSensorDigital_2
#define S_ENC_DRIVE_RIGHT     kVexSensorDigital_8
#define S_ENC_DRIVE_LEFT      kVexSensorDigital_6

// Joystick settings
#define J_DRIVE      Ch3
#define J_TURN       Ch1

//#define J_SHOOT      Btn6U
#define J_SHOOT_MAX     Btn8R
#define J_SHOOT_75      Btn8U
#define J_SHOOT_STOP    Btn8D
#define J_SHOOT_50      Btn8L
#define J_SHOOT_LONG    Btn7L

#define J_PISTON 		Btn7D

#define J_FEED_SHOOT_U  Btn6U
#define J_FEED_SHOOT_D  Btn6D
#define J_FEED_FRONT_D  Btn5D
#define J_FEED_FRONT_U  Btn5U

// Constants
#define DEFAULT_FEED_SPEED 127
#define FEED_SPOOL_TIME 100

#define FLY_LONG_SPEED 7100
#define FLY_50_SPEED   6300
#define FLY_MAX_SPEED  6900
#define FLY_75_SPEED   6550

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
  { M_DRIVE_LEFT1,    kVexMotor393S,           kVexMotorNormal,     kVexSensorIME,         kImeChannel_2 },
  { M_DRIVE_RIGHT1,   kVexMotor393S,           kVexMotorNormal,     kVexSensorIME,         kImeChannel_1 },
  { M_FEED_SHOOT,     kVexMotor393S,           kVexMotorReversed,     kVexSensorNone,        0 },

  { M_FLY_BOT_WHEEL,      kVexMotor393T,      kVexMotorNormal,   kVexSensorQuadEncoder, kVexQuadEncoder_1 },
  { M_FLY_TOP_WHEEL,     kVexMotor393T,      kVexMotorNormal,     kVexSensorQuadEncoder, kVexQuadEncoder_2 },

  { M_FEED_FRONT,     kVexMotor393S,           kVexMotorNormal,   kVexSensorNone,        0 },
  { M_DRIVE_LEFT2,    kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
  { M_DRIVE_RIGHT2,   kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 }
};

// TBH Controllers
TBHController *botWheelCtrl;
TBHController *topWheelCtrl;

bool driveMotors(void) {
  short ld, rd ;
  //Calculate Motor Power
  int forward = VALLEY(-vexControllerGet(J_DRIVE), 45, 127);
  int turn    = VALLEY(-vexControllerGet(J_TURN), 45, 127);
  ld = VALLEY(forward + turn, 45, 127);
  rd = VALLEY(forward - turn, 45, 127);

  vexMotorSet(M_DRIVE_LEFT1,  ld);
  vexMotorSet(M_DRIVE_LEFT2,  ld);
  vexMotorSet(M_DRIVE_RIGHT1, rd);
  vexMotorSet(M_DRIVE_RIGHT2, rd);

  return (ld != 0 || rd != 0);
}


bool isBallTop(void) {
  return (vexAdcGet(S_BALL_TOP) < 1000);
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
  topWheelCtrl = TBHControllerInit(S_ENC_TOP_FLY, 0.05, 10500, false);
  topWheelCtrl->powerZeroClamp = true;
  topWheelCtrl->log = false;
  botWheelCtrl = TBHControllerInit(S_ENC_BOT_FLY, 0.05, 10500, true);
  botWheelCtrl->log = false;
  botWheelCtrl->powerZeroClamp = true;
  vex_printf("puTTy Test");
}

bool isBotFlyWheelStable(void) {
  return (botWheelCtrl->acceleration < 0.01);
}

bool isTopFlyWheelStable(void) {
  return (topWheelCtrl->acceleration < 0.01);
}

msg_t
vexAutonomous( void *arg )
{
  (void)arg;

  tbhEnable(botWheelCtrl, FLY_MAX_SPEED+175);
  tbhEnable(topWheelCtrl, FLY_MAX_SPEED+175);

  //vexTaskRegister("auton");
  vex_printf("starting autonomous\n");
  int shootCount = 0;
  int step = 0;
  systime_t lastTime = chTimeNow();
  while(!chThdShouldTerminate())
  {
    systime_t time = chTimeNow();
    int32_t timeGap = time-lastTime;
    //if(vexControllerGet(Btn8D)){
    //	break;
    //}
    if(step == 0 && timeGap > 2000) {
      vex_printf("step 0\n");
      step++;
      lastTime = time;
    } else if(step == 1 && timeGap > 2000) {
      vex_printf("step 1\n");
      vexMotorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
      step++;
    } else if (step == 2 && isBallTop()) {
      vex_printf("step 2\n");
      step++;
      lastTime = time;
    } else if (step == 3 && timeGap > 300) {
      vex_printf("step 3\n");
      vexMotorSet(M_FEED_SHOOT, -DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_FRONT, -DEFAULT_FEED_SPEED);
      step++;
    } else if (step == 4 && timeGap > 500) {
      vex_printf("step 4\n");
      step++;
    } else if(step == 5) {
      vex_printf("step 5\n");
      vexMotorSet(M_FEED_SHOOT, 0);
      vexMotorSet(M_FEED_FRONT, 0);
      tbhEnable(topWheelCtrl, FLY_MAX_SPEED);
      tbhEnable(botWheelCtrl, FLY_MAX_SPEED);
      step = 1;
      shootCount++;
      vex_printf("shootcount = %d\n", shootCount);
      if(shootCount == 4) {
        tbhDisable(topWheelCtrl);
        tbhDisable(botWheelCtrl);
        vexMotorSet(M_FLY_TOP_WHEEL, 0);
        vexMotorSet(M_FLY_BOT_WHEEL, 0);
        break;
      }
    }
    vexMotorSet(M_FLY_TOP_WHEEL, tbhUpdate(topWheelCtrl));
    vexMotorSet(M_FLY_BOT_WHEEL, tbhUpdate(botWheelCtrl));
    // Don't hog cpu
    vexSleep( 10 );
  }
  vex_printf("End\n");
  return (msg_t)0;
}

msg_t
vexOperator( void *arg )
{
  (void)arg;
  vex_printf("Putty Connection test");
  // Must call this
  vexTaskRegister("operator");
  // Run until asked to terminate
  systime_t currTime = chTimeNow();
  systime_t startTime = chTimeNow();
  int32_t timeGap = currTime + 2000;

  while(!chThdShouldTerminate())
  {
	currTime = chTimeNow();
	timeGap = startTime - currTime;
    driveMotors();
    //vex_printf("left=%d right=%d\n", vexSensorValueGet(S_ENC_DRIVE_LEFT), vexSensorValueGet(S_ENC_DRIVE_RIGHT));
    //if(vexControllerGet(Btn7R)){
    //	vexAutonomous(NULL);
    //}
    if(vexControllerGet(J_PISTON)) {
      vexDigitalPinSet(P_PISTON, 1);
    } else {
      vexDigitalPinSet(P_PISTON, 0);
    }

    if(vexControllerGet(J_SHOOT_50)) {
      tbhEnable(topWheelCtrl, FLY_50_SPEED);
      tbhEnable(botWheelCtrl, FLY_50_SPEED);
    }
    if(vexControllerGet(J_SHOOT_MAX)) {
      tbhEnable(topWheelCtrl, FLY_MAX_SPEED);
      tbhEnable(botWheelCtrl, FLY_MAX_SPEED);
    }
    if(vexControllerGet(J_SHOOT_75)) {
      tbhEnable(topWheelCtrl, FLY_75_SPEED);
      tbhEnable(botWheelCtrl, FLY_75_SPEED);
    }
    if(vexControllerGet(J_SHOOT_LONG)) {
      tbhEnable(topWheelCtrl, FLY_LONG_SPEED);
      tbhEnable(botWheelCtrl, FLY_LONG_SPEED);
    }
    if(vexControllerGet(J_SHOOT_STOP)) {
      tbhDisable(topWheelCtrl);
      tbhDisable(botWheelCtrl);
    }
    vexMotorSet(M_FLY_TOP_WHEEL, tbhUpdate(topWheelCtrl));
    vexMotorSet(M_FLY_BOT_WHEEL, tbhUpdate(botWheelCtrl));

    // Front Feed Controls
    if(vexControllerGet(J_FEED_FRONT_U) || vexControllerGet(J_FEED_SHOOT_U)) {
       vexMotorSet(M_FEED_FRONT, 100);
    } else if(vexControllerGet(J_FEED_FRONT_D) || vexControllerGet(J_FEED_SHOOT_D)) {
       vexMotorSet(M_FEED_FRONT, -100);
    } else if(!isBallTop() && isBallBot()) {
       vexMotorSet(M_FEED_FRONT, 100);
    } else if (driveMotors()){
    	vexMotorSet(M_FEED_FRONT, 100);
    	startTime = chTimeNow();
    } else if(timeGap < 2000){
    	vexMotorSet(M_FEED_FRONT, 100);
    } else {
       vexMotorSet(M_FEED_FRONT, 0);
    }

    // Shoot Feed
    if(vexControllerGet(J_FEED_SHOOT_U)) {
       vexMotorSet(M_FEED_SHOOT, 77);
    } else if(vexControllerGet(J_FEED_SHOOT_D)) {
       vexMotorSet(M_FEED_SHOOT, -77);
    } else if(!isBallTop() && isBallBot()) {
       vexMotorSet(M_FEED_SHOOT, 77);
    } else {
       vexMotorSet(M_FEED_SHOOT, 0);
    }
    vexSleep( 10 );
  }

  return (msg_t)0;
}
