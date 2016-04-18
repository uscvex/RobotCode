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
  static char buf[100];
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

/* Top down view of robot:

    MotorFL, Port4    MotorFR, Port3
   +++      ###    F    ###      +--
          ###             ###
        ###                 ###


		   L 					             R


        ###                 ###
          ###             ###
   +-+      ###    B    ###      ++-
    MotorBL, Port7    MotorBR, Port8
*/
bool xDriveMotors(int16_t forward, int16_t strafe, int16_t turn,
                  int16_t mfr, int16_t mbr, int16_t mfl, int16_t mbl,
                  int oMin, int oMax) {
  short fld, frd, brd, bld;

  //Four drives
  fld = VALLEY(forward + strafe + turn, oMin, oMax);
  frd = VALLEY(forward - strafe - turn, oMin, oMax);
  bld = VALLEY(forward - strafe + turn, oMin, oMax);
  brd = VALLEY(forward + strafe - turn, oMin, oMax);

  vexMotorSet(mfr, frd);
  vexMotorSet(mbr, brd);
  vexMotorSet(mbl, bld);
  vexMotorSet(mfl, fld);

  return (fld != 0 || frd != 0 || bld != 0 || brd != 0);
}

/**
 * initialize debouncer
 * button - button to be debounced
 * debounceTime - debounce check delay
 */
void debounceInit(Debouncer *dbnc, tCtlIndex button, systime_t debounceTime) {
    dbnc->button = button;
    dbnc->debounceTime = debounceTime;
    dbnc->lastValue = 0;
    dbnc->lastDebouncedValue = 0;
    dbnc->lastTime = 0;
    dbnc->lastPress = 0;
}

/**
 * returns debounced value
 */
int16_t debounceGet(Debouncer *dbnc) {
    int16_t value = vexControllerGet(dbnc->button);
    systime_t currTime = chTimeNow();
    if(dbnc->lastValue != value) {
        dbnc->lastTime = currTime;
    }
    dbnc->lastValue = value;

    if((currTime - dbnc->lastTime) > dbnc->debounceTime) {
        dbnc->lastDebouncedValue = value;
        return value;
    }
    else {
        return dbnc->lastDebouncedValue;
    }
}

/**
 * returns debbounced key down values
 */
bool debounceKeyDown(Debouncer *dbnc) {
    int16_t value = debounceGet(dbnc);
    bool keyDown = (value != dbnc->lastPress && value);
    dbnc->lastPress = value;
    return keyDown;
}
