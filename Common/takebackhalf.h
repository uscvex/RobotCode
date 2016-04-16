#ifndef __TAKEBACKHALF_H__
#define __TAKEBACKHALF_H__

#define SIZEOFERRORARRAY 5

#include "ch.h"
#include "hal.h"
#include "vex.h"

typedef struct _TBHController {
	tVexSensors sensor;

	double tbh;
	double power;
	int16_t motorPower;

	int32_t targetSpeed; // target speed in ticks per second
	int32_t maxSpeed;

	double lastSpeed;
	double acceleration;

	double gain;
	bool enabled;
	int32_t lastValue;
	double lastError;
	double errorArr[SIZEOFERRORARRAY];
	int counter;
	systime_t lastTime;
	bool powerZeroClamp;
	bool sensorReverse;
	bool log;
} TBHController;

TBHController *TBHControllerInit(tVexSensors sensor,
								 double gain,
								 int32_t maxSpeed,
								 bool sensorReverse);
void tbhEnable(TBHController *ctrl, int32_t targetSpeed);
void tbhEnableWithGainTBH(TBHController *ctrl, int32_t targetSpeed, float gain, double tbh);
void tbhEnableWithGain(TBHController *ctrl, int32_t targetSpeed, float gain);
void tbhDisable(TBHController *ctrl);
int16_t tbhUpdate(TBHController *ctrl);
bool tbhIsStable(TBHController *ctrl);

#define MAX_TBH_CONTROLLERS 10

#endif
