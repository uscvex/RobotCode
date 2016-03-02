#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "common.h"
#include "takebackhalf.h"

static int16_t nextTBHController = 0;
static TBHController controllers[MAX_TBH_CONTROLLERS];

//#define Ideal_Speed			00
//#define TBH_Threshold		00
//
//
//// Array of Ten Sample values to calculate max and min
//double Flywheel_Speed_Sample[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
//
//// Global variable to add the values to add values to array
//int ptr = 0;
TBHController *TBHControllerInit(tVexSensors sensor, double gain, int32_t maxSpeed, bool sensorReverse) {
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
	tbhEnableWithGain(ctrl, targetSpeed, ctrl->gain);
}

void tbhEnableWithGain(TBHController *ctrl, int32_t targetSpeed, float gain) {
	if(ctrl->enabled && ctrl->targetSpeed == targetSpeed) {
		return;
	}
	ctrl->gain = gain;
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

/*
 * Sample N values, and then calculate the min and the max.
 */
//bool tbhIsStable(TBHController *ctrl){
//	double max, min;
//	min = max = Flywheel_Sample_speed[0];
//	for (int i=1;i<Flywheel_Speed_Sample.size;i++){
//		if (Flywheel_Speed_Sample[i]>max){
//			max = Flywheel_Speed_Sample[i];
//		}
//		if (Flywheel_Speed_Sample[i]<min){
//			min = Flywheel_Speed_Sample[i];
//		}
//	}
//	return ((max-min)<TBH_Threshold*Ideal_Speed);
//}

int16_t tbhUpdate(TBHController *ctrl) {
	//double 44e;
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
		int32_t speed = (value - ctrl->lastValue)/((double)(currTime - ctrl->lastTime));
		ctrl->acceleration = (speed-ctrl->lastSpeed)/((double)(currTime - ctrl->lastTime));
		ctrl->lastSpeed = speed;
		error = (ctrl->targetSpeed/1000.0) - speed;
		/*
		 * If there is a error, then mid point is calculated.
		 */
		if(SIGN(error) != SIGN(ctrl->lastError)) {
			/*
			 *
			 */
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
			//vex_printf("%f,%f,%f,%f\n", speed, error, ctrl->tbh, ctrl->power);
			serialLog("speed", (double)speed,
			          "error", (double)error,
								"tbh", (double)ctrl->tbh,
								"power", (double)ctrl->power, NULL);
			//vex_printf(, value);
		}
	}
	return ctrl->motorPower;
}
