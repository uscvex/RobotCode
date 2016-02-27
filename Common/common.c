#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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

void serialLog(char *first, ...) {
  char buf[100];
  va_list argp;
  va_start(argp, first);
  systime_t time = chTimeNow();
  double value;
  char *label = first;
  while(label) {
    value = va_arg(argp, double);
    vex_sprintf(buf, "%d,%s,%f", time, label, value);
    int checkSum = 0;
    char *p = buf;
    while(*p) {
      checkSum ^= *p;
      p++;
    }
    vex_printf("###%d#%s###\n", checkSum, buf);
    label = va_arg(argp, char*);
  }
  va_end(argp);
}
