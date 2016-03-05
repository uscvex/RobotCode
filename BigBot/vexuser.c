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

//#define J_SHOOT      Btn6U
#define J_SHOOT_START   Btn8R
#define J_SHOOT_MID     Btn8U
//#define J_SHOOT_SHORT   Btn8L
#define J_SHOOT_CORNER	Btn7U
#define J_SHOOT_STOP    Btn8D

#define J_START_AUTON	Btn7R

#define J_COMPRESS      Btn8L
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
#define FLY_MID_SPEED    6200

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

bool isBotFlyWheelStable(void) {
  return (botWheelCtrl->acceleration < 0.01);
}

bool isTopFlyWheelStable(void) {
  return (topWheelCtrl->acceleration < 0.01);
}
bool isRed(void) {
  return (vexAdcGet(S_COLOR_SELECTOR) > 2000);
}
msg_t
vexAutonomous( void *arg )
{
  (void)arg;
  vexTaskRegister("auton");

  vexSensorValueSet(S_ENC_DRIVE_LEFT,0);
  vexSensorValueSet(S_ENC_DRIVE_RIGHT,0);
  //Skills or Competition
  bool skills = true;
  //true for red, false for blue
  bool color = isRed();

  int step = 0;
  systime_t startTime = chTimeNow();
  systime_t currTime = chTimeNow();
  int32_t timeGap;
  if (skills)
  {
	  //Run feeds continuously
	  vexMotorSet(M_FEED_SHOOT, 100);
	  vexMotorSet(M_FEED_FRONT, 50);
	  while(!chThdShouldTerminate())
	  {
		  //Break out of loop
		  if(vexControllerGet(Btn7U))
		  {
		   	 break;
		  }

		  //Get time
		  currTime = chTimeNow();
		  timeGap = currTime - startTime;
		  //Enable flywheels for ~21 seconds on SHORT
		  if (timeGap < 15000 && step == 0)
		  {
			  tbhEnableWithGain(topWheelCtrl, FLY_MID_SPEED, 0.05);
			  tbhEnableWithGain(botWheelCtrl, FLY_MID_SPEED, 0.05);
			  step++;
		  }
		  //Rotate 90 degrees right
		  if (timeGap >= 15000  && timeGap < 16500 && step == 1) // && timeGap < 22500
		  {
			  EPidEnable(rightDrive, 1500, -285);
			  EPidEnable(leftDrive, 1500, 285);
			  step++;
		  }
		  //Drive forward across the field
		  if (timeGap >= 16500 && timeGap < 20500 && step == 2)
		  {
			  EPidEnable(rightDrive, 4000, 1850);			// If shooting from short, 3050
			  EPidEnable(leftDrive, 4000, 1850);
			  step++;
		  }
		  //Rotate 90 degrees right
		  if (timeGap >= 20500 && timeGap < 22000 && step == 3)
		  {
			  EPidEnable(rightDrive, 1500, 380);
		 	  EPidEnable(leftDrive, 1500, -380);
		 	  step++;
		  }

		  //Shoot for 20 second

		  //Rotate ~90 degrees right
		  if (timeGap >= 42000 && timeGap < 44000 && step == 4)
		  {
			  EPidEnable(rightDrive, 2000, +300);
			  EPidEnable(leftDrive, 2000, -300);
			  step++;
		  }

		  //Move forward ~a foot
		  if (timeGap >= 44000 && timeGap < 45500 && step == 5)
		  {
			  EPidEnable(rightDrive, 1500, -540);
			  EPidEnable(leftDrive, 1500, -540);
			  step++;

		  }
		  //Turn 45 degrees
		  if (timeGap >= 45500 && timeGap < 47000 && step == 6)
		  {
			  EPidEnable(rightDrive, 1500, +170);
			  EPidEnable(leftDrive, 1500, -170);
			  step++;
		  }
		  //Slam the wall
		  if (timeGap >= 47000 && timeGap < 48500 && step == 7)
		  {
			  EPidEnable(rightDrive, 1500, -280);
			  EPidEnable(leftDrive, 1500, -280);
			  step++;
		  }
		  // Get the ramp to open and shut the flywheels
		  if(timeGap >= 48500 && timeGap < 50000 && step == 8)
		  {
			  tbhDisable(topWheelCtrl);
			  tbhDisable(botWheelCtrl);
			  //EPidDisable(rightDrive);
			  //EPidDisable(leftDrive);
			  vexMotorSet(M_FEED_FRONT, 0);
			  vexMotorSet(M_FEED_SHOOT, 0);
			  vexDigitalPinSet(P_PISTON, 1);

			  step++;
		  }

		  if(timeGap >= 50000 && step == 9)
		  {
		  	  vexDigitalPinSet(P_PISTON, 0);
		  }


		  //Drive motors
		  int16_t motorValL = EPidUpdate(leftDrive);
		  int16_t motorValR = EPidUpdate(rightDrive);

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
  else {
	  int TURN1,TURN2,TURN3,TURN4,TURN5;
	  TURN1 = 170;
	  TURN2 = 155;
	  TURN3 = 130;
	  TURN4 = 315;
	  TURN5 = 100;
	  if(!color)
	  {
		  TURN1 *= -1;
		  TURN2 *= -1;
		  TURN3 *= -1;
		  TURN4 *= -1;
		  TURN5 *= -1;
	  }

	  bool shotsReady = false;
	  bool feeding = true;
	  //bool runFrontFeed = false;
	  //bool runShootFeed = false;
	  while(!chThdShouldTerminate())
	  {
		  //Break out of loop
		  if(vexControllerGet(Btn7U))
		  {
			  break;
		  }

		  //Get time
		  currTime = chTimeNow();
		  timeGap = currTime - startTime;
		  /*
		  //Set flywheels
		  if(step == 0 && timeGap < 3000)
		  {
			  //dontShoot = false;
			  shotsReady = false;
		  	  tbhEnableWithGain(topWheelCtrl, FLY_START_SPEED-200, 0.03);
		  	  tbhEnableWithGain(botWheelCtrl, FLY_START_SPEED-200, 0.03);
		  	  step++;
		  }

		  //Set flywheels
		  if(step == 0 && timeGap < 3000)
		  {
			  //dontShoot = false;
			  shotsReady = false;
		  	  tbhEnableWithGain(topWheelCtrl, FLY_MID_SPEED, 0.05);
		  	  tbhEnableWithGain(botWheelCtrl, FLY_MID_SPEED, 0.05);
		  	  step++;
		  }
		  //Turn ~90 degrees
		  if((timeGap >= 3000 && timeGap < 5000) && step == 1)
		  {
			  EPidEnable(rightDrive, 2000, -TURN4);
		 	  EPidEnable(leftDrive, 2000, TURN4);
		 	  step++;
		  }
		  //Drive towards piles
		  if((timeGap >= 5000 && timeGap < 8000) && step == 2)
		  {
			  //reverseFeed = true;
		 	  EPidEnable(rightDrive, 3000, 1600);
		 	  EPidEnable(leftDrive, 3000, 1600);
		 	  step++;
		  }
		  //Drive forward and backward
		  if((timeGap >= 8000 && timeGap < 9000) && step == 3)
		  {
			  EPidEnable(rightDrive, 1000, -150);
			  EPidEnable(leftDrive, 1000, -150);
			  step++;
			  //runShootFeed = true;
		  }

		  if((timeGap >= 9000 && timeGap < 10000) && step == 4)
		  {
			  EPidEnable(rightDrive, 1000, 150);
			  EPidEnable(leftDrive, 1000, 150);
			  step++;
		  }
		  //Align shot
		  if((timeGap >= 10000 && timeGap < 11500) && step == 5)
		  {
			  EPidEnable(rightDrive, 1500, TURN5);
			  EPidEnable(leftDrive, 1500, -TURN5);
			  step++;
		  }
		  //Fire shots
		  if((timeGap >= 11500 && timeGap < 15000) && step == 6)
		  {
			  shotsReady = true;
			  step++;
		  }
		  //*/

		  ///*
		  //Prepare flywheels
		  if(step == 0 && timeGap < 3000)
		  {
			  tbhEnableWithGain(topWheelCtrl, FLY_START_SPEED, 0.05);
			  tbhEnableWithGain(botWheelCtrl, FLY_START_SPEED, 0.05);

		  }
		  //Fire balls
		  if(step == 0 && timeGap >=3000 && timeGap < 6000)
		  {
			shotsReady = true;
			step++;
		  }

		  //Drive forward two feet
		  if(timeGap >= 6000 && timeGap <= 9000 && step == 1)
		  {
			  feeding = false;
			  shotsReady = false;
			  EPidEnable(rightDrive, 2600, -1300);
			  EPidEnable(leftDrive, 2600, -1300);
			  step++;
		  }
		  //Rotate ~30 degrees
		  if((timeGap >= 9000 && timeGap < 10500)&& step == 2)
		  {
			  EPidEnable(rightDrive, 1500, TURN1);
			  EPidEnable(leftDrive, 1500, -TURN1);
			  step++;
		  }
		  //Back up, get balls
		  if((timeGap >= 10500 && timeGap < 13700) && step == 3)
		  {
			  feeding = true;
			  EPidEnable(rightDrive, 3200, 1050);
			  EPidEnable(leftDrive, 3200, 1050);
			  tbhEnableWithGain(topWheelCtrl, FLY_MID_SPEED, 0.05);
			  tbhEnableWithGain(botWheelCtrl, FLY_MID_SPEED, 0.05);
			  step++;
			  //runShootFeed = true;
		  }
		  //Move back and forth
		  if((timeGap >= 13700 && timeGap < 14700) && step == 4)
		  {
			  EPidEnable(rightDrive, 1000, -150);
			  EPidEnable(leftDrive, 1000, -150);
			  step++;
			  //runShootFeed = true;
		  }

		  if((timeGap >= 14700 && timeGap < 15700) && step == 5)
		  {
			  EPidEnable(rightDrive, 1000, 150);
			  EPidEnable(leftDrive, 1000, 150);
			  step++;
			  //runShootFeed = true;
		  }

		  //Align shot
		  if((timeGap >= 15700 && timeGap < 17700) && step == 6)
		  {
			  EPidEnable(rightDrive, 2000, -250);
			  EPidEnable(leftDrive, 2000, -250);
			  step++;
		  }
		  if((timeGap >= 17700 && timeGap < 19700) && step == 7)
		  {
			  EPidEnable(rightDrive, 2000, -TURN2);
			  EPidEnable(leftDrive, 2000, TURN2);
			  step++;
		  }
		  //Shoot balls
		  if((timeGap >= 19700 && timeGap < 23700) && step == 8)
		  {
			  tbhEnableWithGain(topWheelCtrl, FLY_MID_SPEED, 0.05);
			  tbhEnableWithGain(botWheelCtrl, FLY_MID_SPEED, 0.05);
			  shotsReady = true;
			  step++;
		  }
		  //TODO: Release balls in case of a jam

		  //Turn to face center
		  if((timeGap >= 23700 && timeGap < 24700) && step == 9)
		  {
			  shotsReady = false;
			  feeding = false;
			  EPidEnable(rightDrive, 1000, TURN3);
			  EPidEnable(leftDrive, 1000, -TURN3);
			  step++;
		  }
		  //Drive to center of field
		  if((timeGap >= 24700 && timeGap < 27500) && step == 10)
		  {
		  	  EPidEnable(rightDrive, 2800, -1600);
			  EPidEnable(leftDrive, 2800, -1600);
 			  step++;
  		  }
		  //Turn ~90 degrees
		  if((timeGap >= 27500 && timeGap < 29500) && step == 11)
		  {
			  EPidEnable(rightDrive, 2000, -TURN4);
		 	  EPidEnable(leftDrive, 2000, TURN4);
		 	  step++;
		  }
		  //Drive towards piles
		  if((timeGap >= 29500 && timeGap < 32500) && step == 12)
		  {
			  feeding = true;
		 	  EPidEnable(rightDrive, 3000, 1600);
		 	  EPidEnable(leftDrive, 3000, 1600);
		 	  step++;
		  }
		  //Drive forward and backward
		  if((timeGap >= 32500 && timeGap < 33500) && step == 13)
		  {
			  EPidEnable(rightDrive, 1000, -150);
			  EPidEnable(leftDrive, 1000, -150);
			  step++;
			  //runShootFeed = true;
		  }

		  if((timeGap >= 33500 && timeGap < 34500) && step == 14)
		  {
			  EPidEnable(rightDrive, 1000, 150);
			  EPidEnable(leftDrive, 1000, 150);
			  step++;
		  }
		  //Align shot
		  if((timeGap >= 34500 && timeGap < 36000) && step == 15)
		  {
			  EPidEnable(rightDrive, 1500, TURN5);
			  EPidEnable(leftDrive, 1500, -TURN5);
			  step++;
		  }
		  //Fire shots
		  if((timeGap >= 36000 && timeGap < 38500) && step == 16)
		  {
			  shotsReady = true;
			  step++;
		  }

		  //*/

		  //Drive motors
		  int16_t motorValL = EPidUpdate(leftDrive);
		  int16_t motorValR = EPidUpdate(rightDrive);
		  vexMotorSet(M_DRIVE_RIGHT1, motorValR);
		  vexMotorSet(M_DRIVE_RIGHT2, motorValR);
		  vexMotorSet(M_DRIVE_LEFT1, motorValL);
		  vexMotorSet(M_DRIVE_LEFT2, motorValL);

		  //Set flywheels
		  vexMotorSet(M_FLY_TOP_WHEEL, tbhUpdate(topWheelCtrl));
		  vexMotorSet(M_FLY_BOT_WHEEL, tbhUpdate(botWheelCtrl));

		  //Control shoot feed
		  if((shotsReady || !isBallTop())) {
			  vexMotorSet(M_FEED_SHOOT, 77);
		  } else {
			  vexMotorSet(M_FEED_SHOOT, 0);
		  }
		  //Front feed control
		  if(feeding) {
			  vexMotorSet(M_FEED_FRONT, 50);
		  } else {
			  vexMotorSet(M_FEED_FRONT, 0);
		  }
		  // Don't hog cpu
		  vexSleep( 10 );
	  }
  }
  EPidDisable(rightDrive);
  EPidDisable(leftDrive);
  vex_printf("End\n");
  return (msg_t)0;

}

msg_t
//User control settings
vexOperator( void *arg )
{
  (void)arg;
  //vex_printf("Putty Connection test");
  // Must call this
  vexTaskRegister("operator");
  systime_t currentTime = 0;
  systime_t botSensorTime = 0;
  systime_t pneumaticPressed = 0;
  int32_t sensorTimeGap = currentTime - botSensorTime;
  int32_t pneumaticTimeGap;

  //Run until asked to terminate
  while(!chThdShouldTerminate())
  {
	  //  if(vexControllerGet(Btn8U)) {
		 // driveMotors();
	  //} else {
	//	  vexMotorSet(M_DRIVE_RIGHT1, 0);
	//	  vexMotorSet(M_DRIVE_LEFT1, 0);
	//	  vexMotorSet(M_DRIVE_RIGHT2, 0);
	//	  vexMotorSet(M_DRIVE_LEFT2, 0);
    // }
	  //vexSleep(10);
	  //continue;
	currentTime = chTimeNow();
	sensorTimeGap = currentTime - botSensorTime;
	pneumaticTimeGap = currentTime - pneumaticPressed;
	//Stop timer for piston if the button is pressed
	if(!vexControllerGet(J_PISTON))
	{
		pneumaticPressed = currentTime;
	}

	//Test autonomous
	if(vexControllerGet(J_START_AUTON))
	{
		vexAutonomous(NULL);
	}

    bool motorRunning = driveMotors();
    //vex_printf("left=%d right=%d\n", vexSensorValueGet(S_ENC_DRIVE_LEFT), vexSensorValueGet(S_ENC_DRIVE_RIGHT));
    //if(vexControllerGet(Btn7R)){
    //	vexAutonomous(NULL);
    //}
    if(pneumaticTimeGap >= 250) {
      vexDigitalPinSet(P_PISTON, 1);
    } else {
      vexDigitalPinSet(P_PISTON, 0);
    }
    //Short range shot
    //if(vexControllerGet(J_SHOOT_SHORT)) {
    //	tbhEnableWithGain(topWheelCtrl, FLY_SHORT_SPEED,0.01);
    //	tbhEnableWithGain(botWheelCtrl, FLY_SHORT_SPEED,0.01);
    //}
    //Start position shot
    if(vexControllerGet(J_SHOOT_START)) {
      tbhEnableWithGain(topWheelCtrl, FLY_START_SPEED, 0.0375);
      tbhEnableWithGain(botWheelCtrl, FLY_START_SPEED, 0.0375);
    }
    //3/4 court shot
    if(vexControllerGet(J_SHOOT_MID)) {
    	tbhEnableWithGain(topWheelCtrl, FLY_MID_SPEED,0.0375);
    	tbhEnableWithGain(botWheelCtrl, FLY_MID_SPEED,0.0375);
    }
    //Full court shot
    if(vexControllerGet(J_SHOOT_CORNER)) {
    	tbhEnableWithGain(topWheelCtrl, FLY_CORNER_SPEED,0.0125);
    	tbhEnableWithGain(botWheelCtrl, FLY_CORNER_SPEED,0.0125);
    }


    //Turn off flywheels
    if(vexControllerGet(J_SHOOT_STOP)) {
      tbhDisable(topWheelCtrl);
      tbhDisable(botWheelCtrl);
    }
    //Activate/deactivate flywheel motors
    vexMotorSet(M_FLY_TOP_WHEEL, tbhUpdate(topWheelCtrl));
    vexMotorSet(M_FLY_BOT_WHEEL, tbhUpdate(botWheelCtrl));

    if(isBallBot())
    {
    	botSensorTime = chTimeNow();
    }
    // Front Feed Controls
    if(vexControllerGet(J_COMPRESS) || vexControllerGet(J_FEED_FRONT_U) || vexControllerGet(J_FEED_SHOOT_U) || motorRunning) {
       vexMotorSet(M_FEED_FRONT, 63);
    } else if(vexControllerGet(J_FEED_FRONT_D) || vexControllerGet(J_FEED_SHOOT_D)) {
       vexMotorSet(M_FEED_FRONT, -63);
    } else {
       vexMotorSet(M_FEED_FRONT, 0);
    }


    // Shoot Feed
    if(vexControllerGet(J_FEED_SHOOT_U)) {
    	   vexMotorSet(M_FEED_SHOOT, 77);

    } else if(vexControllerGet(J_COMPRESS) || vexControllerGet(J_FEED_SHOOT_D)) {
       vexMotorSet(M_FEED_SHOOT, -77);
    } else if(!isBallTop() && (sensorTimeGap < 250)) {
       vexMotorSet(M_FEED_SHOOT, 77);
    } else {
       vexMotorSet(M_FEED_SHOOT, 0);
    }

    //Don't hog CPU
    vexSleep( 10 );
  }

  return (msg_t)0;
  return (msg_t)0;
  return (msg_t)0;
}
