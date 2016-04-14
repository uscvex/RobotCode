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
#define M_DRIVE_FRONT_RIGHT  kVexMotor_3
#define M_DRIVE_FRONT_LEFT   kVexMotor_4
#define M_DRIVE_BACK_LEFT    kVexMotor_7
#define M_DRIVE_BACK_RIGHT   kVexMotor_8

#define M_FLY_A				 kVexMotor_2
#define M_FLY_B 		     kVexMotor_5
#define M_FLY_C      		 kVexMotor_6

#define M_FEED_SHOOT      	  kVexMotor_9

// Sensor channels
#define P_PISTON              kVexDigital_4
#define P_SOLENOID			  kVexDigital_3

//#define P_ENC_BOT_FLY_A       kVexDigital_9
//#define P_ENC_BOT_FLY_B       kVexDigital_4

#define P_ENC_TOP_FLY_A       kVexDigital_1
#define P_ENC_TOP_FLY_B       kVexDigital_2

#define P_ENC_DRIVE_RIGHT_A   kVexDigital_7
#define P_ENC_DRIVE_RIGHT_B   kVexDigital_8

#define P_ENC_DRIVE_LEFT_A    kVexDigital_5
#define P_ENC_DRIVE_LEFT_B    kVexDigital_6

#define S_BALL_BOT              0
#define S_BALL_TOP              1
#define S_COLOR_SELECTOR		2

//#define S_ENC_BOT_FLY         kVexSensorDigital_4
#define S_ENC_FLY         	  kVexSensorDigital_1
#define S_ENC_DRIVE_RIGHT     kVexSensorDigital_8
#define S_ENC_DRIVE_LEFT      kVexSensorDigital_6

// Joystick settings
#define J_TURN       Ch1
#define J_DRIVE      Ch3
#define J_STRAFE	 Ch4


//#define J_SHOOT      Btn6U
#define J_SHOOT_START   Btn8R
#define J_SHOOT_MID     Btn8U
//#define J_SHOOT_SHORT   Btn8L
#define J_SHOOT_CORNER	Btn7U
#define J_SHOOT_STOP    Btn8D

#define J_START_AUTON	Btn7R

#define J_SHOOT_LESS    Btn8L
#define J_PISTON 		Btn7D

#define J_FEED_SOLE_U  Btn6U
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
//  { P_ENC_BOT_FLY_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_1},
//  { P_ENC_BOT_FLY_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_1},

  { P_ENC_TOP_FLY_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_2},
  { P_ENC_TOP_FLY_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_2},

  { P_ENC_DRIVE_RIGHT_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_3},
  { P_ENC_DRIVE_RIGHT_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_3},

  { P_ENC_DRIVE_LEFT_A,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_4},
  { P_ENC_DRIVE_LEFT_B,      kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_4}
};

// Motor Config
static  vexMotorCfg mConfig[] = {
  { M_DRIVE_FRONT_RIGHT,    kVexMotor393S,           kVexMotorNormal,    	kVexSensorIME,         0 },
  { M_DRIVE_BACK_LEFT, 	    kVexMotor393S,           kVexMotorReversed,     kVexSensorIME,         0 },
  { M_DRIVE_BACK_RIGHT,    	kVexMotor393S,           kVexMotorNormal,     	kVexSensorNone,        0 },
  { M_DRIVE_FRONT_LEFT,    	kVexMotor393S,           kVexMotorNormal,     	kVexSensorNone,        0 },

  { M_FEED_SHOOT,           kVexMotor393S,           kVexMotorReversed,     kVexSensorNone,        0 },

  { M_FLY_A,     kVexMotor393T,      kVexMotorNormal,     kVexSensorNone, 0 },
  { M_FLY_B,     kVexMotor393T,      kVexMotorNormal,     kVexSensorNone, 0 },
  { M_FLY_C,     kVexMotor393T,      kVexMotorNormal,     kVexSensorNone, 0 }

  //Motors

};

// TBH Controllers
TBHController *botWheelCtrl;
TBHController *topWheelCtrl;

//PID Controllers
EPidController *rightDrive;
EPidController *leftDrive;
double maxSpeed;

/* Top down view of robot:

    MotorFL, Port4    MotorFR, Port3
   +++      ###    F    ###      +--
          ###             ###
        ###                 ###


		   L 					             R


        ###                 ###
          ###             ###
   +-+      ###    B    ###      ++-
    MotorBL, Port7    MotorBR, Port8
*/

bool driveMotors(void) {
  short fld, frd, brd, bld;
  //Calculate Motor Power
  int forward = VALLEY(vexControllerGet(J_DRIVE), 25, 127);
  int strafe = VALLEY(vexControllerGet(J_STRAFE), 25, 127);
  int turn = VALLEY(vexControllerGet(J_TURN), 25, 127);

  //Four drives
  fld = VALLEY(forward + strafe + turn, 25, 127);
  frd = VALLEY(forward - strafe - turn, 25, 127);
  bld = VALLEY(forward - strafe + turn, 25, 127);
  brd = VALLEY(forward + strafe - turn, 25, 127);


  vexMotorSet(M_DRIVE_FRONT_RIGHT,  frd);
  vexMotorSet(M_DRIVE_BACK_RIGHT,  brd);
  vexMotorSet(M_DRIVE_BACK_LEFT, bld);
  vexMotorSet(M_DRIVE_FRONT_LEFT, fld);

  return (fld != 0 || frd != 0 || bld != 0 || brd != 0);
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
 // topWheelCtrl = TBHControllerInit(S_ENC_TOP_FLY, 0.01, 10500, true);
//  topWheelCtrl->powerZeroClamp = true;
//  topWheelCtrl->log = false;
//  botWheelCtrl = TBHControllerInit(S_ENC_BOT_FLY, 0.01, 10500, true);
//  botWheelCtrl->log = false;
//  botWheelCtrl->powerZeroClamp = true;
//  vex_printf("puTTy Test");
//
//  //Initialize EPIDControllers
//  rightDrive = EPidInit(kMinJerk,0.01,0,0.01,S_ENC_DRIVE_RIGHT, true);
//  rightDrive->log = false;
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
/*
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
	  bool shotsReady = true;
  	  //Run feeds continuously
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
  		  //Enable flywheels for  seconds on SHORT
  		  if (timeGap < 11000 && step == 0)
  		  {
  			  tbhEnableWithGain(topWheelCtrl, FLY_MID_SPEED, 0.035);
  			  tbhEnableWithGain(botWheelCtrl, FLY_MID_SPEED, 0.035);
  			  step++;
  		  }
  		  //Rotate x degrees right
  		  if (timeGap >= 12000  && timeGap < 13500 && step == 1) // && timeGap < 22500
  		  {
  			  EPidEnable(rightDrive, 1500, -400);
  			  EPidEnable(leftDrive, 1500, 400);
  			  step++;
  		  }
  		  //Drive forward across the field
  		  if (timeGap >= 13500 && timeGap < 17500 && step == 2)
  		  {
  			  shotsReady = false;
  			  EPidEnable(rightDrive, 4000, 2100);			// If shooting from short, 3050
  			  EPidEnable(leftDrive, 4000, 2100);
  			  step++;
  		  }
  		  //Rotate x degrees right
  		  if (timeGap >= 17500 && timeGap < 19000 && step == 3)
  		  {
  			  EPidEnable(rightDrive, 1500, 390);
  		 	  EPidEnable(leftDrive, 1500, -390);
  		 	  step++;
  		  }
  		  //Backup into position

  		  if (timeGap >= 19000 && timeGap < 21500 && step == 4)
  		  {
  			  EPidEnable(rightDrive, 2500, 1100);
  		 	  EPidEnable(leftDrive, 2500, 1100);
  		 	  step++;
  		  }
  		  //Adjust
  		  if (timeGap >= 21500 && timeGap < 22500 && step == 5)
  		  {
  		  	 EPidEnable(rightDrive, 1000, +105);
  		  	 EPidEnable(leftDrive, 1000, -105);
  		  	 step++;
  		  }
 		  if (timeGap >= 22500 && step == 6)
  		  {
  			  shotsReady = true;
  		  }
  		  //Shoot for 22 second
  		  //Rotate ~90 degrees right
  		  if (timeGap >= 44500 && timeGap < 46500 && step == 6)
  		  {
  			  EPidEnable(rightDrive, 2000, +320);
  			  EPidEnable(leftDrive, 2000, -320);
  			  step++;
  		  }

  		  //Move forward ~a foot
  		  if (timeGap >= 46500 && timeGap < 47500 && step == 7)
  		  {
  			  EPidEnable(rightDrive, 1000, -200);
  			  EPidEnable(leftDrive, 1000, -200);
  			  step++;

  		  }
  		  //Turn 45 degrees
  		  if (timeGap >= 47500 && timeGap < 49000 && step == 8)
  		  {
  			  EPidEnable(rightDrive, 1500, +170);
  			  EPidEnable(leftDrive, 1500, -170);
  			  step++;
  		  }
  		  //Slam the wall
  		  if (timeGap >= 49000 && timeGap < 50500 && step == 9)
  		  {
  			  EPidEnable(rightDrive, 1500, -400);
  			  EPidEnable(leftDrive, 1500, -400);
  			  step++;
  		  }
  		  // Get the ramp to open and shut the flywheels
  		  if(timeGap >= 50500 && timeGap < 52500 && step == 10)
  		  {
  			  tbhDisable(topWheelCtrl);
  			  tbhDisable(botWheelCtrl);
  			  //EPidDisable(rightDrive);
  			  //EPidDisable(leftDrive);
  			  shotsReady = false;
  			  vexMotorSet(M_FEED_FRONT, 0);
  			  vexMotorSet(M_FEED_SHOOT, 0);
  			  vexDigitalPinSet(P_PISTON, 1);
  			  step++;
  		  }

  		  if(timeGap >= 52500 && step == 11)
  		  {
  		  	  vexDigitalPinSet(P_PISTON, 0);
  		  }


  		  //Drive motors
  		  int16_t motorValL = EPidUpdate(leftDrive);
  		  int16_t motorValR = EPidUpdate(rightDrive);

  		  vexMotorSet(M_DRIVE_BACK_LEFT, motorValR);
  		  vexMotorSet(M_DRIVE_FRONT_LEFT, motorValR);
  		  vexMotorSet(M_DRIVE_FRONT_RIGHT, motorValL);
  		  vexMotorSet(M_DRIVE_BACK_RIGHT, motorValL);

  		  //Set flywheels
  		  vexMotorSet(M_FLY_TOP_WHEEL, tbhUpdate(topWheelCtrl));
  		  vexMotorSet(M_FLY_BOT_WHEEL, tbhUpdate(botWheelCtrl));

  		  //Control shoot feed
  		  if(shotsReady || (!isBallTop() && step < 10))
  		  {
  			  vexMotorSet(M_FEED_SHOOT, 100);
  		  } else {
  			vexMotorSet(M_FEED_SHOOT, 0);
  		  }

  		  // Don't hog cpu
  		  vexSleep( 10 );
  	  }
    }
  else {
	  int TURN1,TURN2,TURN3,TURN4,TURN5;
	  TURN1 = 170;
	  TURN2 = 155;
	  TURN3 = 600;
	  TURN4 = 565;
	  TURN5 = 450;
	  if(!color)
	  {
		  TURN1 *= -1;
		  TURN2 *= -1;
		  TURN3 *= -1;
		  TURN4 *= -1;
		  TURN5 = -400;
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

		  ///
		  //Prepare flywheels
//		  if(step == 0 && timeGap < 3000)
//		  {
//			  tbhEnableWithGain(topWheelCtrl, FLY_START_SPEED, 0.04);
//			  tbhEnableWithGain(botWheelCtrl, FLY_START_SPEED, 0.04);
//
//		  }
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
			  tbhEnableWithGain(topWheelCtrl, FLY_MID_SPEED, 0.035);
			  tbhEnableWithGain(botWheelCtrl, FLY_MID_SPEED, 0.035);
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
			  tbhEnableWithGain(topWheelCtrl, FLY_MID_SPEED, 0.035);
			  tbhEnableWithGain(botWheelCtrl, FLY_MID_SPEED, 0.035);
			  shotsReady = true;
			  step++;
		  }

		  //Turn ~180
		  if((timeGap >= 23700 && timeGap < 26200) && step == 9)
		  {
			  shotsReady = false;
			  feeding = false;
			  EPidEnable(rightDrive, 2500, TURN3);
			  EPidEnable(leftDrive, 2500, -TURN3);
			  step++;
		  }
		  //Trample pile
		  if((timeGap >= 26200 && timeGap < 28700) && step == 10)
		  {
			  feeding = true;
			  EPidEnable(rightDrive, 2500, 1200);
			  EPidEnable(leftDrive, 2500, 1200);
			  step++;
		  }
		  //Align and shoot
		  if((timeGap >= 28700 && timeGap < 30700) && step == 11)
		  {
			  feeding = true;
			  tbhEnableWithGain(topWheelCtrl, FLY_MID_SPEED-200, 0.05);
			  tbhEnableWithGain(botWheelCtrl, FLY_MID_SPEED-200, 0.05);
			  EPidEnable(rightDrive, 2000, -TURN4);
			  EPidEnable(leftDrive, 2000, TURN4);
			  step++;
		  }
		  if((timeGap >= 30700 && timeGap < 32700) && step == 12)
		  {
			  shotsReady = true;
			  step++;
		  }
		  //Back 500 ticks
		  if((timeGap >= 33200 && timeGap < 34700) && step == 13)
		  {
			  shotsReady = false;
			  EPidEnable(rightDrive, 1500, -700);
			  EPidEnable(leftDrive, 1500, -700);
			  step++;
		  }
		  //Turn to be square to wall
		  if((timeGap >= 34700 && timeGap < 36200) && step == 14)
		  {
			 EPidEnable(rightDrive, 1500, TURN5);
			 EPidEnable(leftDrive, 1500, -TURN5);
			 step++;
		  }
		  //Back up, get balls
		  if((timeGap >= 36200 && timeGap < 38200) && step == 15)
		  {
			  EPidEnable(rightDrive, 2000, 600);
			  EPidEnable(leftDrive, 2000, 600);
			  step++;
			  //runShootFeed = true;
		  }
		  //Move back and forth
		  if((timeGap >= 38200 && timeGap < 39700) && step == 16)
		  {
			  EPidEnable(rightDrive, 1000, -150);
			  EPidEnable(leftDrive, 1000, -150);
			  step++;
			  //runShootFeed = true;
		  }

		  if((timeGap >= 39700 && timeGap < 40700) && step == 17)
		  {
			  EPidEnable(rightDrive, 1000, 150);
			  EPidEnable(leftDrive, 1000, 150);
			  step++;
			  //runShootFeed = true;
		  }
		  //Back up
		  if((timeGap >= 40700 && timeGap < 44500) && step == 18)
		  {
			  EPidEnable(rightDrive, 3800, -1600);
			  EPidEnable(leftDrive, 3800, -1600);
			  step++;
			  //runShootFeed = true;
		  }


		  //Drive motors
		  int16_t motorValL = EPidUpdate(leftDrive);
		  int16_t motorValR = EPidUpdate(rightDrive);
		  vexMotorSet(M_DRIVE_BACK_LEFT, motorValR);
		  vexMotorSet(M_DRIVE_FRONT_LEFT, motorValR);
		  vexMotorSet(M_DRIVE_FRONT_RIGHT, motorValL);
		  vexMotorSet(M_DRIVE_BACK_RIGHT, motorValL);

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
  */
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
  Speedometer *spdm = SpeedometerInit(S_ENC_FLY);

  //Run until asked to terminate
  while(!chThdShouldTerminate())
  {
	  //  if(vexControllerGet(Btn8U)) {
		 // driveMotors();
	  //} else {
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

    //vexMotorSet(M_FLY_A, vexControllerGet(J_DRIVE));
    //vexMotorSet(M_FLY_B, vexControllerGet(J_DRIVE));
    //vexMotorSet(M_FLY_C, vexControllerGet(J_DRIVE));
    //double speed = SpeedometerUpdate(spdm)*-1;
    //if(speed > maxSpeed) {
    //    maxSpeed = speed;
    //}

    vex_printf("maxSpeed=%f\n", maxSpeed);
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
      tbhEnableWithGain(topWheelCtrl, FLY_START_SPEED, 0.04);
//      tbhEnableWithGain(botWheelCtrl, FLY_START_SPEED, 0.04);
    }
    //3/4 court shot
    if(vexControllerGet(J_SHOOT_MID)) {
    	tbhEnableWithGain(topWheelCtrl, FLY_MID_SPEED,0.035);
//    	tbhEnableWithGain(botWheelCtrl, FLY_MID_SPEED,0.035);
    }
    //Full court shot
    if(vexControllerGet(J_SHOOT_CORNER)) {
    	tbhEnableWithGain(topWheelCtrl, FLY_CORNER_SPEED,0.0125);
//    	tbhEnableWithGain(botWheelCtrl, FLY_CORNER_SPEED,0.0125);
    }
    if(vexControllerGet(J_SHOOT_LESS)) {
       	tbhEnableWithGain(topWheelCtrl, FLY_LESS_SPEED,0.04);
//       	tbhEnableWithGain(botWheelCtrl, FLY_LESS_SPEED,0.04);
       }

    //Turn off flywheels
    if(vexControllerGet(J_SHOOT_STOP)) {
      tbhDisable(topWheelCtrl);
//      tbhDisable(botWheelCtrl);
    }
    //Activate/deactivate flywheel motors
    //vexMotorSet(M_FLY_TOP_WHEEL, tbhUpdate(topWheelCtrl));
    //vexMotorSet(M_FLY_BOT_WHEEL, tbhUpdate(botWheelCtrl));

    if(isBallBot())
    {
    	botSensorTime = chTimeNow();
    }
    // Front Feed Controls
//    if( vexControllerGet(J_FEED_FRONT_U) || vexControllerGet(J_FEED_SHOOT_U) || motorRunning) {
//       vexMotorSet(M_FEED_FRONT, 63);
//    } else if(vexControllerGet(J_FEED_FRONT_D) || vexControllerGet(J_FEED_SHOOT_D)) {
//       vexMotorSet(M_FEED_FRONT, -63);
//    } else {
//       vexMotorSet(M_FEED_FRONT, 0);
//    }


    // Shoot Feed

    //if 6U then retract solenoid else deploy
    if(vexControllerGet(J_FEED_SOLE_U)) {
    	   vexMotorSet(M_FEED_SHOOT, 77);
    	   vexDigitalPinSet(P_SOLENOID, 0);
    }
    //5U Feed In, 5D Feed Out
    else if (vexControllerGet(J_FEED_FRONT_D)){
    	vexMotorSet(M_FEED_SHOOT, -77);
    }
    else if (vexControllerGet(J_FEED_FRONT_U)){
    	vexMotorSet(M_FEED_SHOOT, 77);
    }
    else{
    	vexDigitalPinSet(P_SOLENOID, 1);
    	vexMotorSet(M_FEED_SHOOT, 0);
    }
//    else if(vexControllerGet(J_FEED_SHOOT_D)) {
//       vexMotorSet(M_FEED_SHOOT, -77);
//       vexDigitalPinSet(P_SOLENOID, 0);
//    }

//    else if(!isBallTop() && (sensorTimeGap < 250)) {
//       vexMotorSet(M_FEED_SHOOT, 77);
//    } else {
//       vexMotorSet(M_FEED_SHOOT, 0);
//    }

    //Don't hog CPU
    vexSleep( 10 );
  }

  return (msg_t)0;
  return (msg_t)0;
  return (msg_t)0;
}
