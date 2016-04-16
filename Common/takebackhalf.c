#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "common.h"
#include "takebackhalf.h"
#include <string.h>


static int16_t nextTBHController = 0;
static TBHController controllers[MAX_TBH_CONTROLLERS];

//#define Ideal_Speed			00
//#define TBH_Threshold		00
//
//
//// Array of Ten Sample values to calculate max and min
double values_sensor[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
//
//// Global variable to add the values to add values to array
int32_t ptr = 0;
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

void tbhEnableWithGainTBH(TBHController *ctrl, int32_t targetSpeed, float gain, double tbh) {
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
	memset(ctrl->errorArr, 0, sizeof(double)*SIZEOFERRORARRAY);
	ctrl->lastTime = chTimeNow();
	ctrl->lastError = 0;
	ctrl->lastSpeed = 0;
	ctrl->acceleration = 0;
	ctrl->tbh = tbh;
	ctrl->power = ctrl->tbh;
	ctrl->motorPower = ctrl->power * 127;
	ctrl->counter = 0;
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
	for(i = 0; i < SIZEOFERRORARRAY; i++)
	{
		avgError += ctrl->errorArr[i];
	}
	avgError /= sizeof(ctrl->errorArr);
	return (avgError <= 0.05);
}

int32_t getAverage(int32_t val){
	values_sensor[ptr%10] = val;
	ptr++;
	int32_t avg = 0, i=0;
	while (i<10){
			avg+=values_sensor[i];
			i++;
	}
	return (avg/10);

}

int16_t tbhUpdate(TBHController *ctrl) {
	double error;
	int32_t value;
	systime_t currTime = chTimeNow();

	//Set motor power to 0 if disable
	if(!ctrl->enabled) {
		ctrl->motorPower = 0;
	} else if(currTime != ctrl->lastTime) {
		//Get number of ticks from encoder
		value = getAverage(vexSensorValueGet(ctrl->sensor));
		if(ctrl->sensorReverse) {
			value = -value;
		}
		//Calculate speed as ticks per second
		int32_t speed = (value - ctrl->lastValue)/((double)(currTime - ctrl->lastTime));
		ctrl->acceleration = (speed-ctrl->lastSpeed)/((double)(currTime - ctrl->lastTime));
		ctrl->lastSpeed = speed;
		if(ctrl->counter == 5) {ctrl->counter = 0;}
		error = (ctrl->targetSpeed/1000.0) - speed;
		ctrl->errorArr[ctrl->counter] = error;
		ctrl->counter++;

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
