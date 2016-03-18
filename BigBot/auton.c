#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

bool isBotFlyWheelStable(void) {
  return (botWheelCtrl->acceleration < 0.01);
}

bool isTopFlyWheelStable(void) {
  return (topWheelCtrl->acceleration < 0.01);
}
bool isRed(void) {
  return (vexAdcGet(S_COLOR_SELECTOR) > 2000);
}

int32_t deg (int32_t deg){
	//TODO: Need Calibaration
	return (deg*10/3);  
}

msg_t
vexAutonomous( void *arg )
{
  (void)arg;
  vexTaskRegister("auton");

  vexSensorValueSet(S_ENC_DRIVE_LEFT,0);
  vexSensorValueSet(S_ENC_DRIVE_RIGHT,0);

  //REMEMBER: To change this accordingly for Skills or Competition
  bool skills = true;

  //true for red, false for blue; Using the potentiometer
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

  		  vexMotorSet(M_DRIVE_RIGHT1, motorValR);
  		  vexMotorSet(M_DRIVE_RIGHT2, motorValR);
  		  vexMotorSet(M_DRIVE_LEFT1, motorValL);
  		  vexMotorSet(M_DRIVE_LEFT2, motorValL);

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

		  ///*
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
