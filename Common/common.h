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
#define FVALLEY(v, low, high) ((ABS(v) < (low))?(low):(MIN(ABS(v), (high))*SIGN(v)))

typedef struct _Speedometer {
	tVexSensors sensor;
  systime_t lastTime;
  int32_t lastValue;
	double speed;
} Speedometer;

#define MAX_SPEEDOMETERS 10

Speedometer *SpeedometerInit(tVexSensors sensor);
double SpeedometerUpdate(Speedometer *spm);
void serialLog(char *first, ...);
bool xDriveMotors(int forward, int strafe, int turn,
                 int16_t mfr, int16_t mbr, int16_t mfl, int16_t mbl,
                 int iMin, int iMax, int oMin, int oMax);

typedef struct _Debouncer {
    tCtlIndex button;
    systime_t debounceTime;
    int16_t lastValue;
    int16_t lastDebouncedValue;
    systime_t lastTime;
    int16_t lastPress;
} Debouncer;

void debounceInit(Debouncer *dbnc, tCtlIndex button, systime_t debounceTime);
int16_t debounceGet(Debouncer *dbnc);
bool debounceKeyDown(Debouncer *dbnc);

#endif
