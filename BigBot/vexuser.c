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
#define J_SHOOT_SHORT   Btn8L
#define J_SHOOT_CORNER	Btn7U
#define J_SHOOT_STOP    Btn8D

#define J_START_AUTON	Btn7R

#define J_SLOW_TURN     Btn7L
#define J_PISTON 		Btn7D

#define J_FEED_SHOOT_U  Btn6U
#define J_FEED_SHOOT_D  Btn6D
#define J_FEED_FRONT_D  Btn5D
#define J_FEED_FRONT_U  Btn5U

// Constants
#define DEFAULT_FEED_SPEED 127
#define FEED_SPOOL_TIME    100

#define FLY_CORNER_SPEED 6950
#define FLY_SHORT_SPEED  4300
#define FLY_START_SPEED  6650
#define FLY_MID_SPEED    4450

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
  //Half turn speed if "Slow turn" button pressed
  if(vexControllerGet(J_SLOW_TURN))
  {
	  turn    = VALLEY(vexControllerGet(J_TURN)*0.5, 25, 127);
  }
  else
  {
	  turn    = VALLEY(vexControllerGet(J_TURN), 25, 127);
  }
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
  topWheelCtrl = TBHControllerInit(S_ENC_TOP_FLY, 0.01, 10500, false);
  topWheelCtrl->powerZeroClamp = true;
  topWheelCtrl->log = false;
  botWheelCtrl = TBHControllerInit(S_ENC_BOT_FLY, 0.01, 10500, true);
  botWheelCtrl->log = false;
  botWheelCtrl->powerZeroClamp = true;
  vex_printf("puTTy Test");

  //Initialize EPIDControllers
  rightDrive = EPidInit(kMinJerk,0.01,0,0.01,S_ENC_DRIVE_RIGHT, true);
  rightDrive->log = true;
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
  bool skills = false;
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
		  if (timeGap < 21000 && step == 0)
		  {
			  tbhEnableWithGain(topWheelCtrl, FLY_SHORT_SPEED-100, 0.05);
			  tbhEnableWithGain(botWheelCtrl, FLY_SHORT_SPEED-100, 0.05);
			  step++;
		  }
		  //Rotate 90 degrees right
		  if (timeGap >= 21000  && timeGap < 22500 && step == 1) // && timeGap < 22500
		  {
			  EPidEnable(rightDrive, 1500, -285);
			  EPidEnable(leftDrive, 1500, 285);
			  step++;
		  }
		  //Drive forward across the field
		  if (timeGap >= 22500 && timeGap < 26500 && step == 2)
		  {
			  EPidEnable(rightDrive, 4000, 1850);			// If shooting from short, 3050
			  EPidEnable(leftDrive, 4000, 1850);
			  step++;
		  }
		  //Rotate 90 degrees right
		  if (timeGap >= 26500 && timeGap < 28000 && step == 3)
		  {
			  EPidEnable(rightDrive, 1500, 380);
		 	  EPidEnable(leftDrive, 1500, -380);
		 	  step++;
		  }

		  //Shoot for 20 second

		  //Rotate 180 degrees right
		  if (timeGap >= 48000 && timeGap < 50000 && step == 4)
		  {
			  EPidEnable(rightDrive, 2000, +650);
			  EPidEnable(leftDrive, 2000, -650);
			  step++;
		  }

		  //Move and hit against the wall
		  if (timeGap >= 50000 && timeGap < 50500 && step == 5)
		  {
			  EPidEnable(rightDrive, 500, -100);
			  EPidEnable(leftDrive, 500, -100);
			  step++;

		  }

		  // Get the ramp to open and shut the flywheels
		  if(timeGap >= 50500 && timeGap < 51500 && step == 6)
		  {
			  tbhDisable(topWheelCtrl);
			  tbhDisable(botWheelCtrl);
			  EPidDisable(rightDrive);
			  EPidDisable(leftDrive);
			  vexMotorSet(M_FEED_FRONT, 0);
			  vexMotorSet(M_FEED_SHOOT, 0);
			  vexDigitalPinSet(P_PISTON, 1);

			  step++;
		  }

		  if(timeGap >= 51500 && step == 7)
		  {
		  	  vexDigitalPinSet(P_PISTON, 0);
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
  else {
	  int TURN1,TURN2,TURN3,TURN4;
	  TURN1 = 170;
	  TURN2 = -155;
	  TURN3 = 140;
	  TURN4 = 355;
	  if(!color)
	  {
		  TURN1 *= -1;
		  TURN2 *= -1;
		  TURN3 *= -1;
		  TURN4 *= -1;
	  }

	  bool shotsReady = false;
	  bool dontShoot = true;
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

		  //TODO: Fire Pre-loads
		  //Prepare flywheels
		  if(step == 0 && timeGap < 3000)
		  {
			  tbhEnableWithGain(topWheelCtrl, FLY_START_SPEED, 0.04);
			  tbhEnableWithGain(botWheelCtrl, FLY_START_SPEED, 0.04);

		  }
		  //Fire balls
		  if(step == 0 && timeGap >=3000 && timeGap < 6000)
		  {
			shotsReady = true;
			dontShoot = false;
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
		  if((timeGap >= 9000 && timeGap < 11000)&& step == 2)
		  {
			  EPidEnable(rightDrive, 1500, TURN1);
			  EPidEnable(leftDrive, 1500, -TURN1);
			  step++;
		  }
		  //Back up, get balls
		  if((timeGap >= 11000 && timeGap < 14500) && step == 3)
		  {
			  feeding = true;
			  EPidEnable(rightDrive, 3200, 1080);
			  EPidEnable(leftDrive, 3200, 1080);
			  tbhEnableWithGain(topWheelCtrl, FLY_MID_SPEED, 0.05);
			  tbhEnableWithGain(botWheelCtrl, FLY_MID_SPEED, 0.05);
			  step++;
			  //runShootFeed = true;
		  }
		  //TODO Move back and forth
		  if((timeGap >= 14500 && timeGap < 15500) && step == 4)
		  {
			  EPidEnable(rightDrive, 1000, -150);
			  EPidEnable(leftDrive, 1000, -150);
			  step++;
			  //runShootFeed = true;
		  }

		  if((timeGap >= 15500 && timeGap < 16500) && step == 5)
		  {
			  EPidEnable(rightDrive, 1000, 150);
			  EPidEnable(leftDrive, 1000, 150);
			  step++;
			  //runShootFeed = true;
		  }

		  //Align shot
		  if((timeGap >= 16500 && timeGap < 18500) && step == 6)
		  {
			  //runShootFeed = false;
			  EPidEnable(rightDrive, 2000, -250);
			  EPidEnable(leftDrive, 2000, -250);
			  step++;
		  }
		  if((timeGap >= 18500 && timeGap < 20500) && step == 7)
		  {
			  EPidEnable(rightDrive, 2000, TURN2);
			  EPidEnable(leftDrive, 2000, -TURN2);
			  step++;
		  }
		  //Shoot balls
		  if((timeGap >= 20500 && timeGap < 24500) && step == 8)
		  {
			  tbhEnableWithGain(topWheelCtrl, FLY_MID_SPEED, 0.05);
			  tbhEnableWithGain(botWheelCtrl, FLY_MID_SPEED, 0.05);
			   shotsReady = true;
			   step++;
		  }
		  //TODO: Release balls in case of a jam
		  //Turn to face center
		  if((timeGap >= 24500 && timeGap < 25500) && step == 9)
		  {
			  //runShootFeed = false;
			  EPidEnable(rightDrive, 1000, TURN3);
			  EPidEnable(leftDrive, 1000, -TURN3);
			  step++;
		  }
		  if((timeGap >= 24500 && timeGap < 25500) && step == 10)
		  		  {
		  			  //runShootFeed = false;
		  			  EPidEnable(rightDrive, 1000, -200);
		  			  EPidEnable(leftDrive, 1000, -200);
		  			  step++;
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
		  if((shotsReady || !isBallTop()) && !dontShoot)
		  {
			  vexMotorSet(M_FEED_SHOOT, 77);
		  }
		  else
		  {
			  vexMotorSet(M_FEED_SHOOT, 0);
		  }
		  if(feeding)
		  {
			  vexMotorSet(M_FEED_FRONT, 50);
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
    if(vexControllerGet(J_SHOOT_SHORT)) {
    	tbhEnableWithGain(topWheelCtrl, FLY_SHORT_SPEED,0.01);
    	tbhEnableWithGain(botWheelCtrl, FLY_SHORT_SPEED,0.01);
    }
    //Start position shot
    if(vexControllerGet(J_SHOOT_START)) {
      tbhEnableWithGain(topWheelCtrl, FLY_START_SPEED, 0.04);
      tbhEnableWithGain(botWheelCtrl, FLY_START_SPEED, 0.04);
    }
    //3/4 court shot
    if(vexControllerGet(J_SHOOT_MID)) {
    	tbhEnableWithGain(topWheelCtrl, FLY_MID_SPEED,0.05);
    	tbhEnableWithGain(botWheelCtrl, FLY_MID_SPEED,0.05);
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
    if(vexControllerGet(J_FEED_FRONT_U) || vexControllerGet(J_FEED_SHOOT_U) || motorRunning) {
       vexMotorSet(M_FEED_FRONT, 63);
    } else if(vexControllerGet(J_FEED_FRONT_D) || vexControllerGet(J_FEED_SHOOT_D)) {
       vexMotorSet(M_FEED_FRONT, -63);
    } else {
       vexMotorSet(M_FEED_FRONT, 0);
    }


    // Shoot Feed
    if(vexControllerGet(J_FEED_SHOOT_U)) {
    	   vexMotorSet(M_FEED_SHOOT, 77);

    } else if(vexControllerGet(J_FEED_SHOOT_D)) {
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

//A function for rotating degrees
//void rotate90()
//{

//}
