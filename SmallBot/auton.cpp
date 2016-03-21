int autonStep = 0;
int autonShootCount = 0;
int autonTurn = 1;
systime_t autonTime;
systime_t autonLastTime;
systime_t autonWaitTime;

#define STEP(s) (autonStep == (s) && (autonTime-autonLastTime) > autonWaitTime)
#define WAIT(t) do {autonLastTime = autonTime;autonWaitTime = (t);} while(false);
#define TIMEELAPSED(t) ((autonTime-autonLastTime) > (t))

int rotateClockWise(int startStep, int target, float speed) {
  if(STEP(startStep)) {
    vex_printf("rotateClockWise run\n");
    int32_t duration = (ABS(target)/speed)*1000;
    int32_t waitTime = duration * 1.1;
    EPidEnable(leftDrive, duration, target);
    EPidEnable(rightDrive, duration, -target);
    autonStep++;
    WAIT(waitTime);
  }
  return (startStep + 1);
}

int move(int startStep, int target, float speed) {
  if(STEP(startStep)) {
    vex_printf("move run\n");
    int32_t duration = (ABS(target)/speed)*1000;
    int32_t waitTime = duration * 1.5;
    EPidEnable(leftDrive, duration, target);
    EPidEnable(rightDrive, duration, target);
    autonStep++;
    WAIT(waitTime);
  }
  return (startStep + 1);
}

int rotateCWTillLine(int startStep, int motorPower) {
  int sensor;
  if(motorPower > 0) {
    sensor = S_LINE_FOLLOWER_L2;
  } else {
    sensor = S_LINE_FOLLOWER_R2;
  }
  if(STEP(startStep)) {
    vex_printf("rotateCWTillLine run\n");
    EPidDisable(leftDrive);
    EPidDisable(rightDrive);
	  vexMotorSet(M_DRIVE_LEFT1, motorPower);
	  vexMotorSet(M_DRIVE_LEFT2, motorPower);
	  vexMotorSet(M_DRIVE_RIGHT1, -motorPower);
	  vexMotorSet(M_DRIVE_RIGHT2, -motorPower);
    autonStep++;
    WAIT(0);
  }
  if(STEP(startStep+1) && isLineOn(sensor)) {
    vex_printf("rotateCWTillLine lineup\n");
    EPidEnable(leftDrive, 2000, 0);
    EPidEnable(rightDrive, 2000, 0);
    autonStep++;
    WAIT(2500);
  }
  return (startStep + 2);
}


int restDown(int startStep) {
  if(STEP(startStep)) {
    // rest
    vex_printf("rest!!\n");
    EPidDisable(leftDrive);
    EPidDisable(rightDrive);
    LineFollowerDisable(lfol);
    vexMotorSet(M_DRIVE_LEFT1, 0);
    vexMotorSet(M_DRIVE_LEFT2, 0);
    vexMotorSet(M_DRIVE_RIGHT1, 0);
    vexMotorSet(M_DRIVE_RIGHT2, 0);
    autonStep++;
    WAIT(2000);
  }
  return (startStep + 1);
}

int punch(int startStep) {
  if(STEP(startStep)) {
    vex_printf("punch!! punch!!\n");
    EPidDisable(leftDrive);
    EPidDisable(rightDrive);
	  vexMotorSet(M_DRIVE_LEFT1, 127);
	  vexMotorSet(M_DRIVE_LEFT2, 127);
	  vexMotorSet(M_DRIVE_RIGHT1, 127);
	  vexMotorSet(M_DRIVE_RIGHT2, 127);
    autonStep++;
    WAIT(3000);
  }
  return (startStep + 1);
}

#define TILLMIDDLE 0
#define TILLENDS   1
#define SOMETIME 2

int lineFollow(int startStep, int end) {
  bool endCondition = false;
  if(end == TILLMIDDLE) {
    endCondition = isAllLineOn();
  } else if(end == TILLENDS) {
    endCondition = isEndsOn();
  } else if(end == SOMETIME) {
    endCondition = true;
  }

  if(STEP(startStep)) {
    vex_printf("lineFollowTillMiddle start\n");
    EPidDisable(leftDrive);
    EPidDisable(rightDrive);
    LineFollowerEnable(lfol);
    autonStep++;
    if(end == SOMETIME) {
      WAIT(2000);
    } else {
      WAIT(0);
    }
  }
  if(STEP(startStep+1) && endCondition) {
    vex_printf("lineFollowTillMiddle start\n");
    LineFollowerDisable(lfol);
    EPidEnable(leftDrive, 2000, 0);
    EPidEnable(rightDrive, 2000, 0);
    autonStep++;
    WAIT(2500);
  }
  return (startStep + 2);
}

/**
 * 4 step routine that shoots N Balls
 */
int shootNBalls(int startStep, int n, int failSafeStep) {
    failSafeStep = failSafeStep < 0 ? (startStep+5) : failSafeStep;

    if(STEP(startStep)) {
      vex_printf("shootNBalls step Init\n", autonStep);
      autonShootCount = 0;
      vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
      autonStep++;
      WAIT(0);
    }
    if(STEP(startStep+1)) {
      vex_printf("shootNBalls step Feed through\n", autonStep);
      vexMotorSet(M_FEED_FRONT, DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, DEFAULT_FEED_SPEED);
      autonStep++;
      WAIT(0);
    }
    if(autonStep == (startStep+2) && TIMEELAPSED(AUTON_FEED_FAIL_TIME)) {
      vex_printf("shootNBalls Failsafe\n");
      //fail safe
      autonShootCount = 0;
      vexMotorSet(M_FEED_SHOOT, 0);
      vexMotorSet(M_FEED_FRONT, 0);
      autonStep = failSafeStep;
    }
    if(STEP(startStep+2) && isBallTop()) {
      vex_printf("shootNBalls Shoot\n");
      autonStep++;
      WAIT(500);
    }
    if (STEP(startStep+3)) {
      vex_printf("shootNBalls Tiny Backup\n");
      vexMotorSet(M_FEED_FRONT, -DEFAULT_FEED_SPEED);
      vexMotorSet(M_FEED_SHOOT, -DEFAULT_FEED_SPEED);
      autonStep++;
      WAIT(100);
    }
    if (STEP(startStep+4)) {
      vex_printf("shootNBalls Repeat step=%d ballCount=%d\n", autonStep, autonShootCount);
      vexMotorSet(M_FEED_FRONT, 0);
      vexMotorSet(M_FEED_SHOOT, 0);
      autonShootCount++;
      if(autonShootCount == n) {
        autonShootCount = 0;
        autonStep++;
      } 
      else {
        autonStep = startStep+1;
        WAIT(1000);
      }
    }
    return (startStep + 5);
}

msg_t
vexAutonomous( void *arg )
{
  (void)arg;
  //vexTaskRegister("auton");

  vexSensorValueSet(S_ENC_DRIVE_LEFT, 0);
  vexSensorValueSet(S_ENC_DRIVE_RIGHT, 0);

  autonTurn = isBlue()?1:-1;
  autonStep = 0;
  autonShootCount = 0;
  autonTime = chTimeNow();
  autonWaitTime = 0;

  vex_printf("starting autonomous\n");
  tbhEnable(topWheelCtrl, FLY_SIDE_SPEED);
  tbhEnable(botWheelCtrl, FLY_SIDE_SPEED);
  WAIT(2000);

  int nextStep;
  #define RUNSTEP(stepName, ...) nextStep = stepName(nextStep, ##__VA_ARGS__)

  vex_printf("enc=%d,%d\n", vexSensorValueGet(S_ENC_DRIVE_LEFT), vexSensorValueGet(S_ENC_DRIVE_RIGHT));
  while(!chThdShouldTerminate())
  {
    autonTime = chTimeNow();
  	if(vexControllerGet(J_STOP_AUTON)) {
  		break;
  	}

    nextStep = 0;
    RUNSTEP(shootNBalls, 8, -1);
    RUNSTEP(rotateClockWise, 475, 300);
    RUNSTEP(move, -2020, 300);
    RUNSTEP(rotateCWTillLine, -35);
    RUNSTEP(lineFollow, TILLMIDDLE);
    RUNSTEP(move, -100, 100);
    RUNSTEP(lineFollow, TILLENDS);
    RUNSTEP(rotateClockWise, 300, 300);
    RUNSTEP(rotateCWTillLine, 35);
    RUNSTEP(rotateClockWise, 30, 300);
    //RUNSTEP(lineFollow, SOMETIME);
    RUNSTEP(move, 700, 400);
    RUNSTEP(restDown);
    RUNSTEP(punch);
    if(STEP(nextStep)) {
      vex_printf("Exit step %d\n", autonStep);
      break;
    }

    LineFollowerUpdate(lfol);
  	int16_t motorValL = EPidUpdate(leftDrive);
  	int16_t motorValR = EPidUpdate(rightDrive);
    if(leftDrive->pidc->enabled) {
  	  vexMotorSet(M_DRIVE_LEFT1, motorValL);
  	  vexMotorSet(M_DRIVE_LEFT2, motorValL);
    }
    if(rightDrive->pidc->enabled) {
  	  vexMotorSet(M_DRIVE_RIGHT1, motorValR);
  	  vexMotorSet(M_DRIVE_RIGHT2, motorValR);
    }
    if(lfol->enabled) {
  	  vexMotorSet(M_DRIVE_LEFT1, lfol->leftDrive);
  	  vexMotorSet(M_DRIVE_LEFT2, lfol->leftDrive);
  	  vexMotorSet(M_DRIVE_RIGHT1, lfol->rightDrive);
  	  vexMotorSet(M_DRIVE_RIGHT2, lfol->rightDrive);
    }
    vexMotorSet(M_FLY_TOP_WHEEL, tbhUpdate(topWheelCtrl));
    vexMotorSet(M_FLY_BOT_WHEEL, tbhUpdate(botWheelCtrl));
    vexSleep( 10 );
	}


  tbhDisable(topWheelCtrl);
  tbhDisable(botWheelCtrl);
  EPidDisable(leftDrive);
  EPidDisable(rightDrive);
  vexMotorSet(M_FEED_FRONT, 0);
  vexMotorSet(M_FEED_SHOOT, 0);
  vexMotorSet(M_FLY_TOP_WHEEL, 0);
  vexMotorSet(M_FLY_BOT_WHEEL, 0);
  vex_printf("Exit autonomous");
  return (msg_t)0;
}