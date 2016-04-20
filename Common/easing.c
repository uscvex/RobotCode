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
		case kFlat:
			v = 1;
			break;
		case kLinear:
			v = t;
			break;
		case kMinJerk:
			/*
			 * v(t) = 10t^3 - 15t^4 + 6t^5
			 * This is equation for the curve that "reduces" the jerk.
			 */
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
	epidc->spdmtr = SpeedometerInit(port);
	epidc->log = false;
	return epidc;
}

void EPidEnableWithValue(EPidController *epid, int32_t duration, int32_t target, int32_t sensorValue) {
    vex_printf("Enabling %d, %d, %d\n", duration, target, sensorValue);
	epid->pidc->enabled = 1;
	EasingEnable(epid->easing, duration, sensorValue, sensorValue+target);
    vex_printf("Easing start=%d, target=%d\n", epid->easing->start, epid->easing->target);
}

void EPidDisable(EPidController *epid) {
	EasingDisable(epid->easing);
	epid->pidc->enabled = 0;
}

int16_t EPidUpdateWithValue(EPidController *epid, int32_t value) {
	SpeedometerUpdate(epid->spdmtr);
	epid->pidc->target_value = EasingUpdate(epid->easing);
    epid->pidc->error = (epid->pidc->target_value) - value;
	int16_t motorPower = PidControllerUpdate(epid->pidc);
	if(epid->log) {
        vex_printf("v = %d t = %d e = %f cmd=%d\n", value, epid->pidc->target_value, epid->pidc->error, epid->pidc->drive_cmd);
		/* float distance = epid->easing->target - epid->easing->start; */
		/* float normTarget = (epid->easing->value - epid->easing->start)/distance; */
		/* float normError = epid->pidc->error/distance; */
		/* vex_printf("%f,%f,%f,%f\n", normTarget, normError, epid->pidc->drive, epid->spdmtr->speed); */
	}
	return motorPower;
}

int16_t EPidUpdate(EPidController *epid) {
    return EPidUpdateWithValue(epid, 0);
}
