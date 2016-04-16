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
    vex_sprintf(buf, "Time: %d,label: %s,value: %f", time, label, value);
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
bool xDriveMotors(int forward, int strafe, int turn,
                  int16_t mfr, int16_t mbr, int16_t mfl, int16_t mbl,
                  int iMin, int iMax, int oMin, int oMax) {
  short fld, frd, brd, bld;

  forward = VALLEY(forward, iMin, iMax);
  strafe = VALLEY(strafe, iMin, iMax);
  turn = VALLEY(turn, iMin, iMax);

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
