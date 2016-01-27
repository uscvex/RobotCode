#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "common.h"
#include "takebackhalf.h"

static int16_t nextTBHController = 0;
static TBHController controllers[MAX_TBH_CONTROLLERS];

TBHController *TBHControllerInit(tVexSensors sensor,
								 double gain,
								 int32_t maxSpeed,
								 bool sensorReverse) {
	TBHController *ctrl = &controllers[nextTBHController++];

	ctrl->sensor = sensor;

	ctrl->tbh = 0;
	ctrl->power = 0;
	ctrl->motorPower = 0;

	ctrl->maxSpeed = maxSpeed;

	ctrl->gain = gain;
	ctrl->enabled = false;
	ctrl->lastValue = 0;
	ctrl->lastError = 0;
	ctrl->lastTime = 0;
	ctrl->lastSpeed = 0;
	ctrl->powerZeroClamp = false;
	ctrl->sensorReverse = sensorReverse;
	ctrl->log = false;

	return ctrl;
}

void tbhEnable(TBHController *ctrl, int32_t targetSpeed) {
	if(ctrl->enabled && ctrl->targetSpeed == targetSpeed) {
		return;
	}
	ctrl->targetSpeed = targetSpeed;
	ctrl->enabled = true;
	ctrl->lastValue = vexSensorValueGet(ctrl->sensor);
	if(ctrl->sensorReverse) {
		ctrl->lastValue = -ctrl->lastValue;
	}
	ctrl->lastTime = chTimeNow();
	ctrl->lastError = 0;
	ctrl->lastSpeed = 0;
	ctrl->acceleration = 0;
	ctrl->tbh = (ctrl->targetSpeed/((double)ctrl->maxSpeed));
	ctrl->power = ctrl->tbh;
	//vex_printf("tbh = %f\n", ctrl->tbh);
	ctrl->motorPower = ctrl->power * 127;
}

void tbhDisable(TBHController *ctrl) {
	ctrl->enabled = false;
}

int16_t tbhUpdate(TBHController *ctrl) {
	double speed;
	double error;
	int32_t value;
	systime_t currTime = chTimeNow();

	if(!ctrl->enabled) {
		ctrl->motorPower = 0;
	} else if(currTime != ctrl->lastTime) {
		value = vexSensorValueGet(ctrl->sensor);
		if(ctrl->sensorReverse) {
			value = -value;
		}
		speed = (value - ctrl->lastValue)/((double)(currTime - ctrl->lastTime));
		ctrl->acceleration = (speed-ctrl->lastSpeed)/((double)(currTime - ctrl->lastTime));
		ctrl->lastSpeed = speed;
		error = (ctrl->targetSpeed/1000.0) - speed;
		if(SIGN(error) != SIGN(ctrl->lastError)) {
			ctrl->power = 0.5 * (ctrl->power + ctrl->tbh);
			ctrl->tbh = ctrl->power;
		} else {
			if(ctrl->powerZeroClamp) {
				ctrl->power = CLAMP(ctrl->power + ctrl->gain*error, 0, 1);
			} else {
				ctrl->power = CLAMP(ctrl->power + ctrl->gain*error, -1, 1);
			}
		}

		ctrl->lastError = error;
		ctrl->lastTime = currTime;
		ctrl->lastValue = value;
		ctrl->motorPower = ctrl->power * 127;
		if(ctrl->log) {
			//vex_printf("speed=%f error=%f motor_power = %d\n", speed, error, ctrl->motorPower);
			vex_printf("%f,%f,%f,%f\n", speed, error, ctrl->tbh, ctrl->power);
		}
	}
	return ctrl->motorPower;
}
