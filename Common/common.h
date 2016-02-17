#ifndef __COMMON_H__
#define __COMMON_H__

#include "ch.h"
#include "hal.h"
#include "vex.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define ABS(v) (((v) < 0)?(-(v)):(v))
#define SIGN(v) ((v)<0?-1:1)
#define CLAMP(v, lower, upper) ( ((v) < (lower)) ? (lower) : ( ((v) > (upper)) ? (upper) : (v) ) )
#define VALLEY(v, low, high) ((ABS(v) < (low))?0:(MIN(ABS(v), (high))*SIGN(v)))

typedef struct _Speedometer {
	tVexSensors sensor;
  systime_t lastTime;
  int32_t lastValue;
	double speed;
} Speedometer;

#define MAX_SPEEDOMETERS 10

Speedometer *SpeedometerInit(tVexSensors sensor);
double SpeedometerUpdate(Speedometer *spm);

#endif
