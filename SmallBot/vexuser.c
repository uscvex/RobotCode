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
#define M_FLY_TOP_WHEEL      kVexMotor_6

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

#define P_ENC_DRIVE_RIGHT_A   kVexDigital_5
#define P_ENC_DRIVE_RIGHT_B   kVexDigital_6

#define P_ENC_DRIVE_LEFT_A    kVexDigital_7
#define P_ENC_DRIVE_LEFT_B    kVexDigital_8


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
#define S_ENC_DRIVE_RIGHT     kVexSensorDigital_6
#define S_ENC_DRIVE_LEFT      kVexSensorDigital_8


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

  { M_FEED_FRONT,     kVexMotor393S, kVexMotorNormal, kVexSensorNone, 0 },
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
const int16_t lfolThresholds[5] = {200, 200, 200, 200, 200};
const float lfolDrives[5] = {0, 0.5, 1,  0.5,  0};
const float lfolTurns[5] = {-0.3, -0.1, 0, 0.1, 0.3};

bool isBlue(void) {
  return (vexAdcGet(S_COLOR_SELECTOR) < 2000);
}

bool driveMotors(void) {
  short ld, rd ;
  //Calculate Motor Power
  int forward = VALLEY(vexControllerGet(J_DRIVE), 10, 127);
  int turnChannel = vexControllerGet(J_TURN) * 0.8;
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
  botWheelCtrl->log = true;
  botWheelCtrl->powerZeroClamp = true;

  //Initialize EPIDControllers
  rightDrive = EPidInit(kMinJerk,0.001,0,0,S_ENC_DRIVE_RIGHT, true);
  leftDrive = EPidInit(kMinJerk,0.001,0,0,S_ENC_DRIVE_LEFT, true);


  // init line followers
  lfol = LineFollowerInit(
    5,                   // five sensors
    S_LINE_FOLLOWER_L2,  // starting from the leftmost
    50,                 // max forward speed
    50,                 // max turn speed
    lfolThresholds,
    lfolDrives,
    lfolTurns
  );
  lfol->log = true;
}

msg_t
vexAutonomous( void *arg )
{
  (void)arg;
  //vexTaskRegister("auton");
  while(!chThdShouldTerminate())
  {
    if(vexControllerGet(Btn5U)) {
      vexMotorSet(M_DRIVE_LEFT2, 0);
      vexMotorSet(M_DRIVE_LEFT1, 0);
      vexMotorSet(M_DRIVE_RIGHT1, 0);
      vexMotorSet(M_DRIVE_RIGHT2, 0);
      break;
    }

    LineFollowerUpdate(lfol);
    vexMotorSet(M_DRIVE_LEFT2, lfol->leftDrive);
    vexMotorSet(M_DRIVE_LEFT1, lfol->leftDrive);
    vexMotorSet(M_DRIVE_RIGHT1, lfol->rightDrive);
    vexMotorSet(M_DRIVE_RIGHT2, lfol->rightDrive);
    vexSleep( 10 );
  }
  return (msg_t)0;
}

/*
msg_t
vexAutonomous( void *arg )
{
  (void)arg;
  vexTaskRegister("auton");

  vexSensorValueSet(S_ENC_DRIVE_LEFT, 0);
  vexSensorValueSet(S_ENC_DRIVE_RIGHT, 0);

  int turn = isBlue()?1:-1;

  int16_t step = 0;
  int16_t shootCount = 0;
  systime_t time = chTimeNow();
  systime_t lastTime;
  tbhEnable(topWheelCtrl, FLY_START_SPEED);
  tbhEnable(botWheelCtrl, FLY_START_SPEED);
  systime_t waitTime = 0;

  //Skills or Competition
  bool skills = true;
  //true for red, false for blue
//  bool color = isRed();

	systime_t startTime = chTimeNow();
	systime_t currTime = chTimeNow();
	int32_t timeGap;
	if (skills)
	{
	  while(!chThdShouldTerminate())
	  {
		  if(vexControllerGet(Btn7U))
		  {
		   	 break;
		  }

		  // Get Time
		  currTime = chTimeNow();
		  timeGap = currTime - startTime;

		  //Enable flywheels for ~2 seconds on SHORT
		  if (timeGap < 3000 && step==0)
		  {
			  tbhEnableWithGain(topWheelCtrl, FLY_SIDE_SPEED, 0.05);
			  tbhEnableWithGain(botWheelCtrl, FLY_SIDE_SPEED, 0.05);
			  step ++;

		  }
		  if (timeGap >= 3000 && timeGap < 8000 && step==1)
		  {
			  vexMotorSet(M_FEED_SHOOT, 100);
			  vexMotorSet(M_FEED_FRONT, 50);
//			  EPidEnable(rightDrive, 3000, 1300);			// If shooting from short, 3050
//			  EPidEnable(leftDrive, 3000, 1300);
			  step++;
		  }
		  if (timeGap >=8000 && timeGap < 11000 && step ==2){

			  EPidEnable(rightDrive, 3000, 1000);
			  EPidEnable(leftDrive, 3000, 1000);
			  step++;
		  }
		  if (timeGap >= 11000 && step ==3){
			  break;
		  }

		  //Drive motors
		  int16_t motorValL = EPidUpdate(leftDrive);
		  int16_t motorValR = EPidUpdate(rightDrive);
		  vex_printf ("%d       %d", motorValL, motorValR);
		  vexMotorSet(M_DRIVE_RIGHT1, motorValR);
		  vexMotorSet(M_DRIVE_RIGHT2, motorValR);
		  vexMotorSet(M_DRIVE_LEFT1, motorValL);
		  vexMotorSet(M_DRIVE_LEFT2, motorValL);

		  //Set flywheels
		  vexMotorSet(M_FLY_TOP_WHEEL, tbhUpdate(topWheelCtrl));
		  vexMotorSet(M_FLY_BOT_WHEEL, tbhUpdate(botWheelCtrl));

		  //Control shoot feed

		  // Don't hog cpu
		  vexSleep( 10 );
	  }

	}
	else{
		#define STEP(s) (step == s && (time-lastTime) > waitTime)
		#define WAIT(t) do {lastTime = time;waitTime = (t);} while(false);
  	#define TIMEELAPSED(t) ((time-lastTime) > t)

  // wait for 3 seconds to let the flywheel spool
  WAIT(3000);


  while(!chThdShouldTerminate())
  {
    time = chTimeNow();
  	// if(vexControllerGet(J_STOP_AUTON))
  	// {
  	// 	break;
  	// }


    if(STEP(0)) {
      vex_printf("Autonomos Step %d\n", step);
      vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
      step++;
    }
    if(step == 1 && TIMEELAPSED(4000)) {
      vex_printf("Failsafe Autonomos Step %d\n", step);
      //fail safe
      shootCount= 0;
      vexMotorSet(M_FEED_SHOOT, 0);
      vexMotorSet(M_FEED_FRONT, 0);
      step = 4;
    }
    if (STEP(1) && isBallTop()) {
      vex_printf("Autonomos Step %d\n", step);
      step++;
      WAIT(700);
    }
    if (STEP(2)) {
      vex_printf("Autonomos Step %d\n", step);
      vexMotorSet(M_FEED_FRONT, -DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, -DEFAULT_FEED_SPEED);
      step++;
      WAIT(100);
    }
    if (STEP(3)) {
      vex_printf("autonomos step %d\n", step);
      vexMotorSet(M_FEED_FRONT, 0);
      vexMotorSet(M_FEED_SHOOT, 0);
      shootCount++;
      if(shootCount == 4) {
        vex_printf("Finished Preloads%d\n", step);
        shootCount = 0;
        step++;
      } else {
        // if we still have preloads, wait for spool up
        // and go to step 0
        step = 0;
        WAIT(3000);
      }
    }
    // start alte

    // if(STEP(4)) {
    //   vex_printf("autonomos step %d\n", step);
    //   EPidEnable(leftDrive, 2500, 500);
    //   EPidEnable(rightDrive, 2500, 500);
    //   tbhEnable(topWheelCtrl, FLY_PB_SPEED);
    //   tbhEnable(botWheelCtrl, FLY_PB_SPEED);
    //   step++;
    //   WAIT(3000);
    // }
    // if(STEP(5)) {
    //   vex_printf("autonomos step %d\n", step);
    //   EPidEnable(leftDrive, 2000, (-turn * 520));
    //   EPidEnable(rightDrive, 2000, (turn * 520));
    //   step++;
    //   WAIT(2500);
    // }
    // if(STEP(6)) {
    //   vex_printf("autonomos step %d\n", step);
    //   EPidEnable(leftDrive, 2500, 700);
    //   EPidEnable(rightDrive, 2500, 700);
    //   vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
    //   vexMottorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
    //   step++;
    //   WAIT(3000);
    // }
    // if(STEP(7)) {
    //   vex_printf("autonomos step %d\n", step);
    //   EPidEnable(leftDrive, 2000, (turn * 520));
    //   EPidEnable(rightDrive, 2000, (-turn * 520));
    //   step++;
    //   WAIT(3000);
    // }
    // if(STEP(8)) {
    //   vex_printf("autonomos step %d\n", step);
    //   EPidEnable(leftDrive, 5000, 2800);
    //   EPidEnable(rightDrive, 5000, 2800);
    //   step++;
    //   WAIT(6000);
    // }
    // if((step >= 6 && step <= 8) && isBallTop()) {
    //   vex_printf("Ball reached top %d\n", step);
    //   vexMotorSet(M_FEED_SHOOT, 0);
    // }
    // if(STEP(9)) {
    //   vex_printf("autonomos step %d\n", step);
    //   vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
    //   vexMotorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
    //   step++;
    //   WAIT(0);
    // }
    // if(step == 9 && TIMEELAPSED(4000)) {
    //   vex_printf("Failsafe Autonomos Step %d\n", step);
    //   //fail safe
    //   shootCount= 0;
    //   vexMotorSet(M_FEED_SHOOT, 0);
    //   vexMotorSet(M_FEED_FRONT, 0);
    //   step = 13;
    // }
    // if(STEP(10) && isBallTop()) {
    //   vex_printf("Autonomos Step %d\n", step);
    //   step++;
    //   WAIT(700);
    // }
    // if (STEP(11)) {
    //   vex_printf("Autonomos Step %d\n", step);
    //   vexMotorSet(M_FEED_FRONT, -DEFAULT_FEED_SPEED);
    //   vexMotorSet(M_FEED_SHOOT, -DEFAULT_FEED_SPEED);
    //   step++;
    //   WAIT(100);
    // }
    // if (STEP(12)) {
    //   vex_printf("autonomos step %d\n", step);
    //   vexMotorSet(M_FEED_FRONT, 0);
    //   vexMotorSet(M_FEED_SHOOT, 0);
    //   shootCount++;
    //   if(shootCount == 4) {
    //     vex_printf("Finished Preloads%d\n", step);
    //     shootCount = 0;
    //     step++;
    //   } else {
    //     step = 9;
    //     WAIT(3000);
    //   }
    // }
    // if(STEP(13)) {
    //   break;
    // }

    // start original

    if(STEP(4)) {
      vex_printf("autonomos step %d\n", step);
      EPidEnable(leftDrive, 6600, 2520);
      EPidEnable(rightDrive, 6600, 2520);
      tbhEnable(topWheelCtrl, FLY_PB_SPEED);
      tbhEnable(botWheelCtrl, FLY_PB_SPEED);
      step++;
      WAIT(7000);
    }
    if(STEP(5)) {
      vex_printf("autonomos step %d\n", step);
      //turn
      EPidEnable(leftDrive, 1000, (-turn * 520));
      EPidEnable(rightDrive, 1000, (turn * 520));
      step++;
      WAIT(2000);
    }
    if(STEP(6)) {
      vex_printf("autonomos step %d\n", step);
      EPidEnable(leftDrive, 3000, 750);
      EPidEnable(rightDrive, 3000, 750);
      vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
      step++;
      WAIT(2500);
    }
    if(STEP(7)) {
      vex_printf("autonomos step %d\n", step);
      vexMotorSet(M_FEED_FRONT, 0);
      vexMotorSet(M_FEED_SHOOT, 0);
      step++;
      WAIT(100);
    }
    if(STEP(8)) {
      vex_printf("autonomos step %d\n", step);
      EPidEnable(leftDrive, 900, 700);
      EPidEnable(rightDrive, 900, 700);
      vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
      step++;
      WAIT(1500);
    }
    if((step == 8 || step == 9 || step == 7) && isBallTop()) {
      vex_printf("Ball reached top %d\n", step);
      vexMotorSet(M_FEED_SHOOT, 0);
    }
    if(STEP(9)) {
      vex_printf("autonomos step %d\n", step);
      EPidEnable(leftDrive, 1000, (turn * 560));
      EPidEnable(rightDrive, 1000, (-turn * 560));
      vexMotorSet(M_FEED_FRONT, 0);
      vexMotorSet(M_FEED_SHOOT, 0);
      step++;
      WAIT(2500);
    }
    if(STEP(10)) {
      vex_printf("autonomos step %d\n", step);
      EPidEnable(leftDrive, 1000, 520);
      EPidEnable(rightDrive, 1000, 520);
      step++;
      WAIT(2500);
    }
    if(STEP(11)) {
      vex_printf("autonomos step %d\n", step);
      vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
      step++;
      WAIT(0);
    }
    if(step == 12 && TIMEELAPSED(4000)) {
      vex_printf("Failsafe Autonomos Step %d\n", step);
      //fail safe
      shootCount= 0;
      vexMotorSet(M_FEED_SHOOT, 0);
      vexMotorSet(M_FEED_FRONT, 0);
      step = 15;
    }
    if(STEP(12) && isBallTop()) {
      vex_printf("Autonomos Step %d\n", step);
      step++;
      WAIT(700);
    }
    if (STEP(13)) {
      vex_printf("Autonomos Step %d\n", step);
      vexMotorSet(M_FEED_FRONT, -DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, -DEFAULT_FEED_SPEED);
      step++;
      WAIT(100);
    }
    if (STEP(14)) {
      vex_printf("autonomos step %d\n", step);
      vexMotorSet(M_FEED_FRONT, 0);
      vexMotorSet(M_FEED_SHOOT, 0);
      shootCount++;
      if(shootCount == 4) {
        vex_printf("Finished Preloads%d\n", step);
        shootCount = 0;
        step++;
      } else {
        step = 11;
        WAIT(3000);
      }
    }
    if(STEP(15)) {
      vex_printf("autonomos step %d\n", step);
      EPidEnable(leftDrive, 2500, -1000);
      EPidEnable(rightDrive, 2500, -1000);
      step++;
      WAIT(3000);
    }
    if(STEP(16)) {
      vex_printf("autonomos step %d\n", step);
      EPidEnable(leftDrive, 2000, (-turn * 560));
      EPidEnable(rightDrive, 2000, (turn * 560));
      step++;
      WAIT(2500);
    }
    if(STEP(17)) {
      vex_printf("autonomos step %d\n", step);
      vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
      EPidEnable(leftDrive, 2000, 1050);
      EPidEnable(rightDrive, 2000, 1050);
      step++;
      WAIT(3000);
    }
    if((step == 17 || step == 18 || step == 19) && isBallTop()) {
      vex_printf("Ball reached top %d\n", step);
      vexMotorSet(M_FEED_SHOOT, 0);
    }
    if(STEP(18)) {
      vex_printf("autonomos step %d\n", step);
      EPidEnable(leftDrive, 2000, -1050);
      EPidEnable(rightDrive, 2000, -1050);
      step++;
      WAIT(2500);
    }
    if(STEP(19)) {
      vex_printf("autonomos step %d\n", step);
      vexMotorSet(M_FEED_FRONT, 0);
      vexMotorSet(M_FEED_SHOOT, 0);
      EPidEnable(leftDrive, 2000, (turn * 550));
      EPidEnable(rightDrive, 2000, (-turn * 550));
      step++;
      WAIT(2500);
    }
    if(STEP(20)) {
      vex_printf("autonomos step %d\n", step);
      EPidEnable(leftDrive, 2500, 1000);
      EPidEnable(rightDrive, 2500, 1000);
      step++;
      WAIT(3000);
    }
    if(STEP(21)) {
      vex_printf("autonomos step %d\n", step);
      vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
      step++;
      WAIT(0);
    }
    if(step == 22 && TIMEELAPSED(4000)) {
      vex_printf("Failsafe Autonomos Step %d\n", step);
      //fail safe
      shootCount= 0;
      vexMotorSet(M_FEED_SHOOT, 0);
      vexMotorSet(M_FEED_FRONT, 0);
      step = 25;
    }
    if(STEP(22) && isBallTop()) {
      vex_printf("Autonomos Step %d\n", step);
      step++;
      WAIT(700);
    }
    if (STEP(23)) {
      vex_printf("Autonomos Step %d\n", step);
      vexMotorSet(M_FEED_FRONT, -DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, -DEFAULT_FEED_SPEED);
      step++;
      WAIT(100);
    }
    if (STEP(24)) {
      vex_printf("autonomos step %d\n", step);
      vexMotorSet(M_FEED_FRONT, 0);
      vexMotorSet(M_FEED_SHOOT, 0);
      shootCount++;
      if(shootCount == 4) {
        vex_printf("Finished Preloads%d\n", step);
        shootCount = 0;
        step++;
      } else {
        step = 21;
        WAIT(3000);
      }
    }
    if(STEP(25)) {
      break;
    }

  	int16_t motorValL = EPidUpdate(leftDrive);
  	int16_t motorValR = EPidUpdate(rightDrive);
	  vexMotorSet(M_DRIVE_RIGHT1, motorValR);
	  vexMotorSet(M_DRIVE_RIGHT2, motorValR);
	  vexMotorSet(M_DRIVE_LEFT1, motorValL);
	  vexMotorSet(M_DRIVE_LEFT2, motorValL);
    vexMotorSet(M_FLY_TOP_WHEEL, tbhUpdate(topWheelCtrl));
    vexMotorSet(M_FLY_BOT_WHEEL, tbhUpdate(botWheelCtrl));
    vexSleep( 10 );
  }
	}

  tbhDisable(topWheelCtrl);
  tbhDisable(botWheelCtrl);
  vexMotorSet(M_FEED_FRONT, 0);
  vexMotorSet(M_FEED_SHOOT, 0);
  vexMotorSet(M_FLY_TOP_WHEEL, 0);
  vexMotorSet(M_FLY_BOT_WHEEL, 0);
  vex_printf("Exit autonomous");
  return (msg_t)0;
} */


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
  	 if(vexControllerGet(J_START_AUTON))
  	 {
  	 	vexAutonomous(NULL);
  	 }


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
    vexMotorSet(M_FLY_TOP_WHEEL, tbhUpdate(topWheelCtrl));
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

    vexSleep( 10 );
  }

  return (msg_t)0;
}
