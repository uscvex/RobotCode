#ifndef __EASING_H__
#define __EASING_H__

#include "pidlib.h"
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"

typedef enum {
	kLinear = 0,
	kMinJerk
} tEasingFunc;

typedef struct _EasingConfig {
	tEasingFunc func;
	uint32_t duration;

	int32_t value;
	uint32_t start_time;
	int32_t  start;
	int32_t  target;
	bool enabled;
} EasingConfig;

#define MAX_EASING_CONFIGS 10

EasingConfig *easingInit(tEasingFunc func);
void enableEasing(EasingConfig *conf, int32_t duration, int32_t start, int32_t target);
void disableEasing(EasingConfig *conf);
int32_t updateEasing(EasingConfig *conf);

#endif
