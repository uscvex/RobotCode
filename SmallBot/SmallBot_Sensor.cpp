/*
* Sensor Code:
		
*/


bool isBlue(void) {
  return (vexAdcGet(S_COLOR_SELECTOR) < 2000);
}

bool isLineOn(int sensor) {
  return (vexAdcGet(sensor) < lfolThresholds[(sensor - S_LINE_FOLLOWER_L2)]);
}

bool isAllLineOn(void) {
  int i = 0;
  for(i = 0;i < 5;i++) {
    if(vexAdcGet(i+S_LINE_FOLLOWER_L2) >= lfolThresholds[i]) {
      return false;
    }
  }
  return true;
}

bool isEndsOn(void) {
  return ((isLineOn(S_LINE_FOLLOWER_L2) || isLineOn(S_LINE_FOLLOWER_L1)) &&
          (isLineOn(S_LINE_FOLLOWER_R2) || isLineOn(S_LINE_FOLLOWER_R1)));
}

bool isBallTop(void) {
  return (vexAdcGet(S_BALL_TOP) < 2200);
}

bool isBallBot(void) {
  return (vexAdcGet(S_BALL_BOT) < 2880);
}
