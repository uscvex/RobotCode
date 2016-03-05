#ifndef __LINE_FOLLOWER_H__
#define __LINE_FOLLOWER_H__

typedef struct _LineFollower {
  int16_t sensorCount;
  int16_t maxSpeed;
  int16_t firstSensor;
  int16_t const *thresholds;
  int16_t sensorDefault;
  int16_t leftDrive;
  int16_t rightDrive;
  int16_t lastTurn;
  float const *drive;
  bool enabled;
  bool log;
} LineFollower;

#define MAX_LINE_FOLLOWERS 3

LineFollower *LineFollowerInit(int16_t sensorCount,
                               int16_t firstSensor,
                               int16_t maxSpeed,
                               int16_t const *thresholds,
                               float const *drive);

void LineFollowerEnable(LineFollower *lfol);
void LineFollowerDisable(LineFollower *lfol);
void LineFollowerUpdate(LineFollower *lfol);

#endif
