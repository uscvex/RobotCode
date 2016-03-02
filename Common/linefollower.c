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
                               int16_t maxDriveSpeed,
                               int16_t maxTurnSpeed,
                               int16_t const *thresholds,
                               float const *drive,
                               float const *turn)
{
  LineFollower *lfol = &lineFollowers[nextLineFollower++];
  lfol->sensorCount= sensorCount;
  lfol->firstSensor = firstSensor;
  lfol->maxDriveSpeed = maxDriveSpeed;
  lfol->maxTurnSpeed = maxTurnSpeed;
  lfol->thresholds = thresholds;
  lfol->drive = drive;
  lfol->turn = turn;
  lfol->lastSide = 0;
  lfol->log = false;
  return lfol;
}

/**
 * Update line follower and compute drive outputs
 * lfol - the line follower instance
 * leftDriveOut - pointer to integer where left motor output will be stored
 * rightDriveOut - pointer to integer where right motor output will be stored
 */
void LineFollowerUpdate(LineFollower *lfol) {
  float drive = 0;
  float turn = 0;
  int enableCount = 0;
  int minEnableIndex = lfol->sensorCount;
  int maxEnableIndex = -1;
  int i;

  // find sum of drive and turn values for all enabled sensors
  for(i = 0;i < lfol->sensorCount;i++) {
    //vex_printf("#%d sensor_%d = %d threshold = %d\n", i, (lfol->firstSensor+i), vexAdcGet(lfol->firstSensor + i), lfol->thresholds[i]);
    bool enabled = (vexAdcGet(lfol->firstSensor + i) < lfol->thresholds[i]);
    if(lfol->log) {
      char label[10];
      vex_sprintf(label, "sensor_%d", i);
      serialLog(label, (double)enabled, NULL);
    }
    if(!enabled) {
      continue;
    }
    drive += lfol->drive[i];
    turn += lfol->turn[i];
    if(i < minEnableIndex) {
      minEnableIndex = i;
    }
    if(i > maxEnableIndex) {
      maxEnableIndex = i;
    }
    enableCount++;
  }

  if(enableCount) {
    // find average of drive and turn values for enabled sensors
    drive = drive/enableCount;
    turn = turn/enableCount;

    // update the last turn side
    float mid = lfol->sensorCount/2.0;
    lfol->lastSide = 0;
    if(minEnableIndex < mid && maxEnableIndex < mid) {
      lfol->lastSide = -1;
    }
    if(minEnableIndex > mid && maxEnableIndex > mid) {
      lfol->lastSide = 1;
    }
  } else {
    // if no sensors were enabled then take a move based
    // on the last direction
    if(lfol->lastSide == 1) { // last dir right, so turn left
      drive = 0;
      turn = lfol->turn[0];
    } else if(lfol->lastSide == -1) { // last dir left, so turn right
      drive = 0;
      turn = lfol->turn[lfol->sensorCount - 1];
    } else { // keep moving forward
      drive = lfol->drive[lfol->sensorCount/2];
      turn = 0;
    }
  }

  if(lfol->log) {
    serialLog("drive", (double)drive,
              "turn", (double)turn, NULL);
  }

  drive *= lfol->maxDriveSpeed;
  turn *= lfol->maxTurnSpeed;

  lfol->leftDrive = (int16_t)(drive + turn);
  lfol->rightDrive = (int16_t)(drive - turn);
}
