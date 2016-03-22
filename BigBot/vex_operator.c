void enable_flywheels(int32_t speed, float gain){
  /* Set the flywheel speed and gain and start running*/
  tbhEnableWithGain(topWheelCtrl, 
                    speed, gain);
  tbhEnableWithGain(botWheelCtrl, 
                    speed, gain);
}

msg_t
//User control settings
vexOperator( void *arg )
{
  (void)arg;
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
  	currentTime = chTimeNow();
  	sensorTimeGap = currentTime - botSensorTime;
  	pneumaticTimeGap = currentTime - pneumaticPressed;

    //Test autonomous
  	if(vexControllerGet(J_START_AUTON))
  	{
  		vexAutonomous(NULL);
  	}

  	//Stop timer for piston if the button is pressed
  	if(!vexControllerGet(J_PISTON))
  	{
  		pneumaticPressed = currentTime;
  	}

	
    bool motorRunning = driveMotors();

    /*Wait for .25 sec of button being pressed to deploy the piston*/
    if(pneumaticTimeGap >= 250) {
      vexDigitalPinSet(P_PISTON, 1);
    } 
    else {
      vexDigitalPinSet(P_PISTON, 0);
    }
    
    //Short range shot
    //if(vexControllerGet(J_SHOOT_SHORT)) {
    //	tbhEnableWithGain(topWheelCtrl, FLY_SHORT_SPEED,0.01);
    //	tbhEnableWithGain(botWheelCtrl, FLY_SHORT_SPEED,0.01);
    //}

    //Start position shot
    if(vexControllerGet(J_SHOOT_START)) {
      enable_flywheels(FLY_START_SPEED, 0.04);
    }

    //3/4 court shot
    if(vexControllerGet(J_SHOOT_MID)) {
      enable_flywheels(FLY_MID_SPEED, 0.035);
    }

    //Full court shot
    if(vexControllerGet(J_SHOOT_CORNER)) {
      enable_flywheels(FLY_CORNER_SPEED, 0.0125);
    }

    if(vexControllerGet(J_SHOOT_LESS)) {
      enable_flywheels(FLY_LESS_SPEED, 0.04);
    }

    //Turn off flywheels
    if(vexControllerGet(J_SHOOT_STOP)) {
      tbhDisable(topWheelCtrl);
      tbhDisable(botWheelCtrl);
    }

    //Activate/deactivate flywheel motors
    vexMotorSet(M_FLY_TOP_WHEEL, tbhUpdate(topWheelCtrl));
    vexMotorSet(M_FLY_BOT_WHEEL, tbhUpdate(botWheelCtrl));

    if(isBallBot()){
    	botSensorTime = chTimeNow();
    }

    // Front Feed Controls
    if( vexControllerGet(J_FEED_FRONT_U) || vexControllerGet(J_FEED_SHOOT_U) || motorRunning) {
       vexMotorSet(M_FEED_FRONT, 63);
    } 
    else if(vexControllerGet(J_FEED_FRONT_D) || vexControllerGet(J_FEED_SHOOT_D)) {
       vexMotorSet(M_FEED_FRONT, -63);
    } 
    else {
       vexMotorSet(M_FEED_FRONT, 0);
    }


    // Shoot Feed
    if(vexControllerGet(J_FEED_SHOOT_U)) {
    	   vexMotorSet(M_FEED_SHOOT, 77);
    } 
    else if(vexControllerGet(J_FEED_SHOOT_D)) {
       vexMotorSet(M_FEED_SHOOT, -77);
    } 
    else if(!isBallTop() && (sensorTimeGap < 250)) {
       vexMotorSet(M_FEED_SHOOT, 77);
    } 
    else {
       vexMotorSet(M_FEED_SHOOT, 0);
    }

    //Don't hog CPU
    vexSleep( 10 );
  }

  return (msg_t)0;
  return (msg_t)0;
  return (msg_t)0;
}
