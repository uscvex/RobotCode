#ifndef __EASING_H__
#define __EASING_H__

#include "pidlib.h"
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"

typedef enum {
	kMotor = 0,
	kPID
} tEasingOutput;

typedef enum {
	kLinear = 0,
	kMinJerk
} tEasingFunc;

typedef struct _EasingConfig {
	tEasingOutput output;
	pidController *pidc;
	int16_t motorIndex;

	tEasingFunc func;
	uint32_t duration;

	uint32_t start_time;
	int32_t  start;
	int32_t  target;
	bool enabled;
} EasingConfig;

#define MAX_EASING_CONFIGS 10

EasingConfig *configMotorEasing(int16_t motorIndex, tEasingFunc func, int32_t duration);
EasingConfig *configPidEasing(pidController *pidc, tEasingFunc func, int32_t duration);
void startEasing(EasingConfig *conf, int32_t target);
void setEasingEnabled(EasingConfig *conf, bool enabled);
msg_t easingTask(void *arg);

#endif