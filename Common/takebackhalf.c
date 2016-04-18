#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "common.h"
#include "takebackhalf.h"
#include <string.h>


static int16_t nextTBHController = 0;
static TBHController controllers[MAX_TBH_CONTROLLERS];

TBHController *TBHControllerInit(tVexSensors sensor, double gain, int32_t maxSpeed, bool sensorReverse) {
	TBHController *ctrl = &controllers[nextTBHController++];

	ctrl->sensor = sensor;

	ctrl->tbh = 0;
	ctrl->power = 0;
	ctrl->motorPower = 0;
	ctrl->maxSpeed = maxSpeed;
	ctrl->gain = gain;
	ctrl->enabled = false;
	memset(ctrl->lastErrors, 0, sizeof(double)*SIZEOF_LAST_ERRORS);
	ctrl->lastErrPtr = 0;
	/* memset(ctrl->lastValues, 0, sizeof(double)*SIZEOF_LAST_VALUES); */
	/* ctrl->lastValPtr = 0; */
    ctrl->lastAvgValue = 0;
	ctrl->lastTime = 0;
	ctrl->powerZeroClamp = false;
	ctrl->sensorReverse = sensorReverse;
	ctrl->log = false;

	return ctrl;
}

void tbhEnable(TBHController *ctrl, int32_t targetSpeed) {
	tbhEnableWithGain(ctrl, targetSpeed, ctrl->gain);
}

void tbhEnableWithGainTBH(TBHController *ctrl, int32_t targetSpeed, float gain, double tbh) {
	if(ctrl->enabled && ctrl->targetSpeed == targetSpeed) {
		return;
	}
	ctrl->gain = gain;
	ctrl->targetSpeed = targetSpeed;
	ctrl->enabled = true;

	memset(ctrl->lastErrors, 0, sizeof(double)*SIZEOF_LAST_ERRORS);
	ctrl->lastErrors[0] = 0;
	ctrl->lastErrPtr = 1;

	/* memset(ctrl->lastValues, 0, sizeof(double)*SIZEOF_LAST_VALUES); */
	/* ctrl->lastValues[0] = vexSensorValueGet(ctrl->sensor); */
	/* if(ctrl->sensorReverse) { */
	/* 	ctrl->lastValues[0] = -ctrl->lastValues[0]; */
	/* } */
	/* ctrl->lastValPtr = 1; */
    ctrl->lastAvgValue = 0;

	ctrl->lastTime = chTimeNow();
	ctrl->tbh = tbh;
	ctrl->power = ctrl->tbh;
	ctrl->motorPower = ctrl->power * 127;
}

void tbhEnableWithGain(TBHController *ctrl, int32_t targetSpeed, float gain) {
    tbhEnableWithGainTBH(ctrl, targetSpeed, gain, (targetSpeed/((double)ctrl->maxSpeed)));
}

void tbhDisable(TBHController *ctrl) {
	ctrl->enabled = false;
}

bool tbhIsStable(TBHController* ctrl){
	double avgError = 0;
	unsigned int i;
	for(i = 0; i < SIZEOF_LAST_ERRORS; i++)
	{
		avgError += ctrl->lastErrors[i];
	}
	avgError /= SIZEOF_LAST_ERRORS;
	return (avgError <= 0.05);
}

/* int32_t getAverageSensorValue(TBHController *ctrl) { */
/*     int i; */
/*     double avgSensorValue = 0; */
/*     for(i = 0;i < SIZEOF_LAST_VALUES;i++) { */
/*         avgSensorValue += ctrl->lastValues[i]; */
/*     } */
/*     avgSensorValue /= SIZEOF_LAST_VALUES; */
/*     return (int32_t)(avgSensorValue); */
/* } */

int16_t tbhUpdate(TBHController *ctrl) {
	double error;
	int32_t value;
	systime_t currTime = chTimeNow();

	//Set motor power to 0 if disable
	if(!ctrl->enabled) {
		ctrl->motorPower = 0;
	} else if(currTime != ctrl->lastTime) {
        double lastError;
        if(ctrl->lastErrPtr == 0) {
            lastError = ctrl->lastErrors[SIZEOF_LAST_ERRORS-1];   
        }
        else {
            lastError = ctrl->lastErrors[ctrl->lastErrPtr-1];   
        }

		//Get number of ticks from encoder
		value = vexSensorValueGet(ctrl->sensor);
		if(ctrl->sensorReverse) {
			value = -value;
		}
		/* ctrl->lastValues[ctrl->lastValPtr] = value; */
		/* ctrl->lastValPtr = (ctrl->lastValPtr+1) % SIZEOF_LAST_VALUES; */
        /* value = getAverageSensorValue(ctrl); */

		//Calculate speed 
		double speed = (value - ctrl->lastAvgValue)/((double)(currTime - ctrl->lastTime));
		error = (ctrl->targetSpeed/1000.0) - speed;

		/*
		 * If there is a error, then mid point is calculated.
		 */
		if(SIGN(error) != SIGN(lastError)) {
			ctrl->power = 0.5 * (ctrl->power + ctrl->tbh);
			ctrl->tbh = ctrl->power;
		} else {
			if(ctrl->powerZeroClamp) {
				ctrl->power = CLAMP(ctrl->power + ctrl->gain*error, 0, 1);
			} else {
				ctrl->power = CLAMP(ctrl->power + ctrl->gain*error, -1, 1);
			}
		}

		ctrl->lastErrors[ctrl->lastErrPtr] = error;
		ctrl->lastErrPtr = (ctrl->lastErrPtr+1) % SIZEOF_LAST_ERRORS;

        ctrl->lastAvgValue = value;
		ctrl->lastTime = currTime;
		ctrl->motorPower = ctrl->power * 127;

		if(ctrl->log) {
			//vex_printf("speed=%f error=%f motor_power = %d\n", speed, error, ctrl->motorPower);
			/* vex_printf("%d,%d,%f,%f,%f,%f\n", value, ctrl->lastAvgValue, speed, error, ctrl->tbh, ctrl->power); */
			serialLog("speed", (double)speed,
			          "error", (double)error,
                      "tbh", (double)ctrl->tbh,
                      "power", (double)ctrl->power, NULL);
			//vex_printf(, value);
		}
	}
	return ctrl->motorPower;
}
