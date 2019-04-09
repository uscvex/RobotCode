#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "linefollower.h"
#include "common.h"

static int16_t nextLineFollower = 0;
static LineFollower lineFollowers[MAX_LINE_FOLLOWERS];

/**
 * Initialize a new LineFollower.
 * sensorCount - the number of line sensors used
 * firstSensor - the port index of the first sensor
 * maxDriveSpeed - the maximum forward speed
 * maxTurnSpeed - the maximum turning speed
 * thresholds - an array of threshold values for each sensor
 * drive - an array of drive values (-1 to 1) that each sensor will contribute
 *         to forward movement
 * turn - an array of turn values (-1 to 1) that each sensor will contribute
 *        to a turn
 **/
LineFollower *LineFollowerInit(int16_t sensorCount,
                               int16_t firstSensor,
                               int16_t maxSpeed,
                               int16_t const *thresholds,
                               float const *drive)
{
  LineFollower *lfol = &lineFollowers[nextLineFollower++];
  lfol->sensorCount= sensorCount;
  lfol->firstSensor = firstSensor;
  lfol->maxSpeed = maxSpeed;
  lfol->thresholds = thresholds;
  lfol->drive = drive;
  lfol->lastTurn = 0;
  lfol->log = false;
  lfol->enabled = false;
  //lfol->lastTurn = 0;
  return lfol;
}

void LineFollowerEnable(LineFollower *lfol) {
  lfol->enabled = true;
}

void LineFollowerDisable(LineFollower *lfol) {
  lfol->enabled = false;
}

/**
 * Update line follower and compute drive outputs
 * lfol - the line follower instance
 * leftDriveOut - pointer to integer where left motor output will be stored
 * rightDriveOut - pointer to integer where right motor output will be stored
 */
void LineFollowerUpdate(LineFollower *lfol) {
  float leftDrive = 0;
  float rightDrive = 0;
  int enableCount = 0;
  int i;

  if(!lfol->enabled) {
    lfol->leftDrive = 0;
    lfol->rightDrive = 0;
    return;
  }

  // find sum of drive and turn values for all enabled sensors
  for(i = 0;i < lfol->sensorCount;i++) {
    bool enabled = vexAdcGet(lfol->firstSensor + i) < lfol->thresholds[i];
    if(lfol->log) {
      char label[15];
      vex_sprintf(label, "sensor_%d", i);
      serialLog(label, (double)enabled, NULL);
    }
    if(!enabled) {
      continue;
    }
    rightDrive += lfol->drive[i];
    leftDrive += lfol->drive[lfol->sensorCount - 1 - i];
    enableCount++;
  }

  if(enableCount) {
    // find average of drive and turn values for enabled sensors
    lfol->leftDrive = (int16_t)((leftDrive/enableCount) * lfol->maxSpeed);
    lfol->rightDrive = (int16_t)((rightDrive/enableCount) * lfol->maxSpeed);

    // update last turn values
    if(lfol->leftDrive > lfol->rightDrive) {
      lfol->lastTurn = 1;
    } else if(lfol->rightDrive > lfol->leftDrive) {
      lfol->lastTurn = -1;
    } else {
      lfol->lastTurn = 0;
    }
  } else {
    // if no sensors were detected in place rotate in
    // a direction opposite to last turn
    if(lfol->lastTurn != 0) {
      rightDrive = lfol->lastTurn * lfol->drive[lfol->sensorCount/2];
      leftDrive = -lfol->lastTurn * lfol->drive[lfol->sensorCount/2];
    } else {
      rightDrive = lfol->drive[lfol->sensorCount/2];
      leftDrive =  lfol->drive[lfol->sensorCount/2];
    }

    lfol->leftDrive = (int16_t)(leftDrive * lfol->maxSpeed);
    lfol->rightDrive = (int16_t)(rightDrive * lfol->maxSpeed);
  }


  if(lfol->log) {
    serialLog("leftDrive", (double)(lfol->leftDrive/127.0),
              "rightDrive", (double)(lfol->rightDrive/127.0),
              "lastTurn", (double)(lfol->lastTurn), NULL);
  }
}
