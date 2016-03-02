#ifndef __LINE_FOLLOWER_H__
#define __LINE_FOLLOWER_H__

typedef struct _LineFollower {
  int16_t sensorCount;
  int16_t maxDriveSpeed;
  int16_t maxTurnSpeed;
  int16_t firstSensor;
  int16_t const *thresholds;
  int16_t leftDrive;
  int16_t rightDrive;
  float const *drive;
  float const *turn;
  int lastSide;
  bool log;
} LineFollower;

#define MAX_LINE_FOLLOWERS 3

LineFollower *LineFollowerInit(int16_t sensorCount,
                               int16_t firstSensor,
                               int16_t maxDriveSpeed,
                               int16_t maxTurnSpeed,
                               int16_t const *thresholds,
                               float const *drive,
                               float const *turn);

void LineFollowerUpdate(LineFollower *lfol);

#endif
