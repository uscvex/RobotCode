#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "common.h"

static Speedometer speedometers[MAX_SPEEDOMETERS];
static int16_t nextSpeedometer = 0;

Speedometer *SpeedometerInit(tVexSensors sensor) {
  Speedometer *spdmtr = &speedometers[nextSpeedometer++];
  spdmtr->sensor = sensor;
  spdmtr->lastTime = chTimeNow();
  spdmtr->lastValue = vexSensorValueGet(sensor);
  return spdmtr;
}

double SpeedometerUpdate(Speedometer *spdmtr) {
  int32_t value = vexSensorValueGet(spdmtr->sensor);
  systime_t currTime = chTimeNow();
  spdmtr->speed = (value - spdmtr->lastValue)/((double)(currTime - spdmtr->lastTime));
  spdmtr->lastValue = value;
  spdmtr->lastTime = currTime;
  return spdmtr->speed;
}
