#ifndef __EASING_H__
#define __EASING_H__

#include "pidlib.h"
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "./common.h"

typedef enum {
	kLinear = 0,
	kMinJerk,
	kFlat
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

typedef struct _EPidController {
	pidController *pidc;
	EasingConfig *easing;
	Speedometer *spdmtr;
	bool log;
} EPidController;

#define MAX_EASING_CONFIGS 10
#define MAX_EPID_CONTROLLERS 10

//Constructor for EasingConfig
EasingConfig *EasingInit(tEasingFunc func);
void EasingEnable(EasingConfig *conf, int32_t duration, int32_t start, int32_t target);
void EasingDisable(EasingConfig *conf);
int32_t EasingUpdate(EasingConfig *conf);

//Constructor for EPidController
EPidController *EPidInit(tEasingFunc func, float Kp, float Ki, float Kd, tVexSensors port, int16_t sensor_reverse);
void EPidEnable(EPidController *epid, int32_t duration, int32_t target);
void EPidDisable(EPidController *epid);
int16_t EPidUpdate(EPidController *epid);


#endif
