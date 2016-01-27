#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "easing.h"
#include "common.h"

static EasingConfig configs[MAX_EASING_CONFIGS];
static int16_t nextEasingConfigPtr = 0;

EasingConfig *easingInit(tEasingFunc func) {
	EasingConfig *conf = &configs[nextEasingConfigPtr++];
	conf->func = func;
	conf->duration = 0;
	conf->value = 0;
	conf->start_time = 0;
	conf->start = 0;
	conf->target = 0;
	conf->enabled = false;
	return conf;
}

void enableEasing(EasingConfig *conf, int32_t duration, int32_t start, int32_t target) {
	conf->duration = duration;
	conf->start = start;
	conf->target = target;
	conf->start_time = chTimeNow();
	conf->enabled = true;
}

void disableEasing(EasingConfig *conf) {
	conf->enabled = false;
}

int32_t updateEasing(EasingConfig *conf) {
	if(!conf->enabled) {
		conf->value = 0;
		return conf->value;
	}
	double t = MIN(1.0, (chTimeNow() - conf->start_time)/((double)conf->duration));
	double v = 0;
	switch(conf->func) {
		case kLinear:
			v = t;
			break;
		case kMinJerk:
			v = 10*(t*t*t) - 15*(t*t*t*t) + 6*(t*t*t*t*t);
			break;
	}
	conf->value = conf->start + (conf->target-conf->start)*v;
	return conf->value;
}
