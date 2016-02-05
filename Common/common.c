#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "common.h"

void InitSpeedometer(Speedometer *spm, tVexSensors sensor) {
  spm->sensor = sensor;
  spm->lastTime = chTimeNow();
  spm->lastValue = vexSensorValueGet(sensor);
}

double UpdateSpeedometer(Speedometer *spm) {
  int32_t value = vexSensorValueGet(spm->sensor);
  systime_t currTime = chTimeNow();
  double speed = (value - spm->lastValue)/((double)(currTime - spm->lastTime));
  spm->lastValue = value;
  spm->lastTime = currTime;
  return speed;
}
