 #pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Motor,  port2,           feedFront,     tmotorVex393, openLoop)
#pragma config(Motor,  port3,           frontLeft,     tmotorVex393, openLoop)
#pragma config(Motor,  port4,           backLeft,      tmotorVex393, openLoop)
#pragma config(Motor,  port5,           topFlywheel,   tmotorVex393HighSpeed, PIDControl, encoder, encoderPort, I2C_1, 1000)
#pragma config(Motor,  port6,           bottomFlywheel, tmotorVex393HighSpeed, PIDControl, encoder, encoderPort, I2C_2, 1000)
#pragma config(Motor,  port7,           backRight,     tmotorVex393, openLoop)
#pragma config(Motor,  port8,           frontRight,    tmotorVex393, openLoop)
#pragma config(Motor,  port9,           feedShoot,     tmotorVex393, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

int X2 = 0, Y1 = 0, X1 = 0, threshold = 25, motormax = 100;
int FR, BR, FL, BL;

void flywheelon(){
	motor[topFlywheel] = 100;
	motor[bottomFlywheel] = 100;
}

void flywheeloff(){
	motor[topFlywheel] = 0;
	motor[bottomFlywheel] = 0;
}

task main(){
	while(true){
		Y1 = abs(vexRT[Ch3]) > threshold ? vexRT[Ch3] : 0;
		X1 = abs(vexRT[Ch4]) > threshold ? vexRT[Ch4] : 0;
		X2 = abs(vexRT[Ch1]) > threshold ? vexRT[Ch1] : 0;

		FR = Y1 - X2 - X1;
		BR =  Y1 - X2 + X1;
		FL = Y1 + X2 + X1;
		BL =  Y1 + X2 - X1;

		motor[frontRight] = abs(FR) > motormax ? sgn(FR)*motormax : FR;
		motor[backRight] =  abs(BR) > motormax ? sgn(BR)*motormax : BR;
		motor[frontLeft] = abs(FL) > motormax ? sgn(FL)*motormax : FL;
		motor[backLeft] =  abs(BL) > motormax ? sgn(BL)*motormax : BL;

		if (vexRT[Btn8U] == 1){
			flywheelon();
		}
		else if (vexRT[Btn8R] == 1){
			flywheeloff();
		}

		motor[feedFront] = (vexRT[Btn6U] == 1) ? 127 : (vexRT[Btn6D] == 1) ? -127 : 0;
		motor[feedShoot] = (vexRT[Btn5U] == 1) ? 127 : (vexRT[Btn5D] == 1) ? -127 : 0;

	} //end while(true)
} //end main