#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "easing.h"
#include "common.h"

static EasingConfig configs[MAX_EASING_CONFIGS];
static int16_t nextEasingConfigPtr = 0;

static EPidController epidControllers[MAX_EPID_CONTROLLERS];
static int16_t nextEPidControllerPtr = 0;

EasingConfig *EasingInit(tEasingFunc func) {
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

void EasingEnable(EasingConfig *conf, int32_t duration, int32_t start, int32_t target) {
	conf->duration = duration;
	conf->start = start;
	conf->target = target;
	conf->start_time = chTimeNow();
	conf->enabled = true;
}

void EasingDisable(EasingConfig *conf) {
	conf->enabled = false;
}

int32_t EasingUpdate(EasingConfig *conf) {
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

EPidController *EPidInit(tEasingFunc func, float Kp, float Ki, float Kd, tVexSensors port, int16_t sensor_reverse) {
	EPidController *epidc = &epidControllers[nextEPidControllerPtr++];
	epidc->pidc = PidControllerInit(Kp, Ki, Kd, port, sensor_reverse);
	epidc->easing = EasingInit(func);
	return epidc;
}

void EPidEnable(EPidController *epid, int32_t duration, int32_t target) {
	EasingEnable(epid->easing, duration, vexControllerGet(epid->pidc->sensor_port), target);
}

void EPidDisable(EPidController *epid) {
	EasingDisable(epid->easing);
}

int16_t EPidUpdate(EPidController *epid) {
	epid->pidc->target_value = EasingUpdate(epid->easing);
	return PidControllerUpdate(epid->pidc);
}
