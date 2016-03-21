#include "val.cpp"

void enable_flywheels(int32_t speed, float gain){
  /* Set the flywheel speed and gain and start running*/
  tbhEnableWithGain(topWheelCtrl, 
                    spped, gain);
  tbhEnableWithGain(botWheelCtrl, 
                    speed, gain);
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
    driveMotors();
    //Test autonomous
    if(vexControllerGet(J_START_AUTON))
    {
    	vexAutonomous(NULL);
    }

    /*
      Enable Flywheels, with the required speed and gain
      PB ~ Point Blank Shot
      Start ~ Start Shot
      Close Shot
      Side Shot

      NOTE: To Change Values, open val.cpp to change vals.
    */
    if(vexControllerGet(J_SHOOT_PB)) {
      enable_flywheels(FLY_PB_SPEED, TOP_FLY_WHEEL_PB_GAIN);
    }
    if(vexControllerGet(J_SHOOT_START)) {
      enable_flywheels(FLY_START_SPEED, TOP_FLY_WHEEL_DEFAULT_GAIN);
    }
    if(vexControllerGet(J_SHOOT_CLOSE)) {
      enable_flywheels(FLY_CLOSE_SPEED, TOP_FLY_WHEEL_DEFAULT_GAIN);
    }
    if(vexControllerGet(J_SHOOT_SIDE)) {
      enable_flywheels(FLY_SIDE_SPEED, TOP_FLY_WHEEL_LOW_GAIN);
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
    } 
    else if((/*vexControllerGet(J_FEED_FRONT_D) ||*/ vexControllerGet(J_FEED_SHOOT_D)) /*&& !isBallTop()*/) {
      vexMotorSet(M_FEED_FRONT, -60);
    
    /* Uncomment the Next Two lines, to Enable Auto Feed */
    //} else if(!isBallTop() && isBallBot()) {
      // vexMotorSet(M_FEED_SHOOT, 100);

    } 
    else {
      vexMotorSet(M_FEED_FRONT, 0);
    }


    // Front Feed
    if(vexControllerGet(J_FEED_SHOOT_U)) {
      vexMotorSet(M_FEED_SHOOT, 90);
    } 
    else if(vexControllerGet(J_FEED_SHOOT_D)) {
       vexMotorSet(M_FEED_SHOOT, -90);

    /* Uncomment the Next Two lines, to Enable Auto Feed */
    //} else if(!isBallTop() && isBallBot()) {
       //vexMotorSet(M_FEED_FRONT, 100);

    } 
    else {
      vexMotorSet(M_FEED_SHOOT, 0);
    }

    vexSleep(10);

  }
  return (msg_t)0;
}
