#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "easing.h"
#include "common.h"

static EasingConfig configs[MAX_EASING_CONFIGS];
static int16_t nextEasingConfigPtr = 0;
//static Mutex configMutex;

EasingConfig *configEasing(tEasingOutput output, pidController *pidc, int16_t motorIndex, tEasingFunc func, int32_t duration, bool continuous) {
	EasingConfig *conf = &configs[nextEasingConfigPtr++];
	conf->output = output;
	conf->pidc = pidc;
	conf->motorIndex = motorIndex;
	conf->func = func;
	conf->duration = duration;
	conf->start_time = 0;
	conf->start = 0;
	conf->target = 0;

	conf->continuous = continuous;
	conf->lastValue = 0;
	conf->enabled = false;
	conf->valleyOutput = false;
	return conf;
}

EasingConfig *configMotorEasing(int16_t motorIndex, tEasingFunc func, int32_t duration, bool continuous) {
	return configEasing(kMotor, NULL, motorIndex, func, duration, continuous);
}

EasingConfig *configPidEasing(pidController *pidc, tEasingFunc func, int32_t duration, bool continuous) {
	return configEasing(kPID, pidc, -1, func, duration, continuous);
}

void setOutputValley(EasingConfig *conf, int32_t valleyLow, int32_t valleyHigh) {
	conf->valleyOutput = true;
	conf->valleyLow = valleyLow;
	conf->valleyHigh = valleyHigh;
}

void startEasing(EasingConfig *conf, int32_t target) {
	//chMtxLock(&configMutex);
	if(conf->continuous) {
		conf->start = conf->lastValue;
	} else {
		if(conf->output == kMotor) {
			conf->start = vexMotorGet(conf->motorIndex);
		} else {
			conf->start = conf->pidc->target_value;
		}
	}
	conf->target = target;
	conf->start_time = chTimeNow();
	conf->enabled = true;
	//chMtxUnlock();
}

void setEasingEnabled(EasingConfig *conf, bool enabled) {
	//chMtxLock(&configMutex);
	conf->enabled = enabled;
	//chMtxUnlock();
}

msg_t easingTask(void *arg) {
	(void)arg;

	vexTaskRegister("easingTask");
	//chMtxInit(&configMutex);

    while(!chThdShouldTerminate()) {
    	//if(chMtxTryLock(&configMutex)) {
    		int i;
	    	for(i = 0;i < nextEasingConfigPtr;i++) {
	    		EasingConfig *conf = &configs[i];
	    		if(!conf->enabled) {
	    			continue;
	    		}
	    		float t = MIN(1.0, (chTimeNow() - conf->start_time)/((float)conf->duration));
	    		float v = 0;
	    		switch(conf->func) {
	    			case kLinear:
	    				v = t;
	    				break;
	    			case kMinJerk:
	    				v = 10*(t*t*t) - 15*(t*t*t*t) + 6*(t*t*t*t*t);
	    				break;
	    		}
	    		int32_t value = conf->start + (conf->target-conf->start)*v;
	    		vex_printf("value = %d ", value);
	    		if(conf->valleyOutput) {
	    			value = VALLEY(value, conf->valleyLow, conf->valleyHigh);
	    		}
	    		vex_printf("newvalue = %d\n", value);
	    		conf->lastValue = value;

	    		if(conf->output == kMotor) {
	    			vexMotorSet(conf->motorIndex, value);
	    		} else {
	    			conf->pidc->target_value = value;
	    		}
	    	}
	    	//chMtxUnlock();
    	//}
    	vexSleep(10);
    }

    return (msg_t)0;
}