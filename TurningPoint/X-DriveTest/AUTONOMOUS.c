//Commands for autonomous mode
#define DRIVE 1													//Drive command
#define SLOW 2													//Slow drive command
#define PAUSE 4													//Pause
#define SETGYRO 6												//Set the gyro
#define TURN 8													//Rotate
#define ARMPOS 9												//Set arm position
#define CLAWPOS 10											//Set claw position
#define THROW 11
#define LINEFOLLOW 12

#define END 99													//Stop command list

//parameters
#define LEFTWHITE -5     										//White will search for a white line
#define RIGHTWHITE -1
#define RIGHTULTRA -2   										//Search for RIGHTULTRA sound range
#define LEFTULTRA -2
#define BLACK -3   											//Search for black line
#define USER -4   											//Find angle in user code (override)
#define HANG -6
#define HITWALL -11

//float motorSlew[10];										//Table for motor slewing

float auto0[]={0};											//Default starting table

//Command table to nudge opponents at start of autonomous, then navigate to net and fire three balls into the high goal
//Uses the gyro, line tracker, and RIGHTULTRA sound sensors to navigate.

float basicDriveAuton[] = {
	0,
	DRIVE,0,0,1,
	TURN,90,2,
	DRIVE,270,180,1,
	TURN,180,2,
};
