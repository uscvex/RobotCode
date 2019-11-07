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



/////////////////////////////
////////	STAR MODE /////////
/////////////////////////////


float BIGRIGHTFRONT[]={	//GOOD				//MODE 1 == GET STARS AT BACK, SHOOT OVER FENCE, THEN IF TIME GET STARS IN CORNER
	0,
	DRIVE,270,0,0.75,
	DRIVE,270,0,LEFTWHITE,
	DRIVE,270,0,0.15,
	DRIVE,90,0,0.2,
	ARMPOS,100,
	PAUSE,0.35,
	ARMPOS,1,
	PAUSE,0.5,
	CLAWPOS,OPEN,
	DRIVE,0,0,1,
	CLAWPOS,CLOSE,
	DRIVE,0,0,0.25,
	PAUSE,0.75,
	DRIVE,290,110,0.5,
	THROW,
	DRIVE,290,110,2.25,

	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	DRIVE,80,90,1.75,
	CLAWPOS,CLOSE,
	PAUSE,0.5,
	DRIVE,270,90,1,
	THROW,
	DRIVE,270,90,1.5,
	PAUSE,0.5,
	DRIVE,270,90,1,
	CLAWPOS,OPEN,
	END                                //End of table
};

float BIGRIGHTBACK[]={
	180,
	DRIVE,270,180,0.75,
	DRIVE,270,180,LEFTWHITE,
	DRIVE,270,180,0.15,
	DRIVE,90,180,0.2,
	ARMPOS,100,
	PAUSE,0.35,
	ARMPOS,1,
	PAUSE,0.5,
	CLAWPOS,OPEN,
	DRIVE,180,180,1,
	CLAWPOS,CLOSE,
	DRIVE,180,180,0.25,
	PAUSE,0.75,
	DRIVE,250,40,0.5,
	THROW,
	DRIVE,250,40,2.25,

	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	DRIVE,100,90,1.75,
	CLAWPOS,CLOSE,
	PAUSE,0.5,
	DRIVE,270,90,1,
	THROW,
	DRIVE,270,90,1.5,
	PAUSE,0.5,
	DRIVE,270,90,1,
	CLAWPOS,OPEN,
	END                                //End of table
};

float BIGLEFTBACK[]={
	180,
	DRIVE,90,180,0.75,
	DRIVE,90,180,LEFTWHITE,
	DRIVE,90,180,0.15,
	DRIVE,270,180,0.2,
	ARMPOS,100,
	PAUSE,0.35,
	ARMPOS,1,
	PAUSE,0.5,
	CLAWPOS,OPEN,
	DRIVE,180,180,1,
	CLAWPOS,CLOSE,
	DRIVE,180,180,0.25,
	PAUSE,0.75,
	DRIVE,110,320,0.5,
	THROW,
	DRIVE,110,320,2.25,

	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	DRIVE,260,270,1.75,
	CLAWPOS,CLOSE,
	PAUSE,0.5,
	DRIVE,90,270,1,
	THROW,
	DRIVE,90,270,1.5,
	PAUSE,0.5,
	DRIVE,90,270,1,
	CLAWPOS,OPEN,
	END                                //End of table
};

float BIGLEFTFRONT[]={
	0,
	DRIVE,90,0,0.75,
	DRIVE,90,0,LEFTWHITE,
	DRIVE,90,0,0.15,
	DRIVE,270,0,0.2,
	ARMPOS,100,
	PAUSE,0.35,
	ARMPOS,1,
	PAUSE,0.5,
	CLAWPOS,OPEN,
	DRIVE,0,0,1,
	CLAWPOS,CLOSE,
	DRIVE,0,0,0.25,
	PAUSE,0.75,
	DRIVE,70,220,0.5,
	THROW,
	DRIVE,70,220,2.25,

	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	DRIVE,280,270,1.75,
	CLAWPOS,CLOSE,
	PAUSE,0.5,
	DRIVE,90,270,1,
	THROW,
	DRIVE,90,270,1.5,
	PAUSE,0.5,
	DRIVE,90,270,1,
	CLAWPOS,OPEN,
	END
};


////////////////
// SMALL MODE //
////////////////

float SMALLRIGHTFRONT[] = {	//GOOD
	90,
	DRIVE,235,90,0.5,
	DRIVE,270,90,0.75,
	ARMPOS,100,
	PAUSE,0.35,
	ARMPOS,1,
	CLAWPOS,300,
	PAUSE,0.15,
	//DEPLOYED
	DRIVE,90,90,RIGHTWHITE,
	CLAWPOS,CLOSE,
	PAUSE,1.25,
	THROW,
	DRIVE,305,90,0.5,
	DRIVE,270,90,1.5,
	PAUSE,1,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,
	CLAWPOS,CLOSE,
	PAUSE,0.5,
	THROW,
	END
};

float SMALLRIGHTBACK[] = {
	90,
	DRIVE,305,90,0.5,
	DRIVE,270,90,0.75,
	ARMPOS,100,
	PAUSE,0.35,
	ARMPOS,1,
	CLAWPOS,300,
	PAUSE,0.15,
	//DEPLOYED
	DRIVE,90,90,RIGHTWHITE,
	CLAWPOS,CLOSE,
	PAUSE,1.25,
	THROW,
	DRIVE,235,90,0.5,
	DRIVE,270,90,1.5,
	PAUSE,1,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,
	CLAWPOS,CLOSE,
	PAUSE,0.5,
	THROW,
	END
};

float SMALLLEFTBACK[] = {
		270,
	DRIVE,55,270,0.5,
	DRIVE,90,270,0.75,
	ARMPOS,100,
	PAUSE,0.35,
	ARMPOS,1,
	CLAWPOS,300,
	PAUSE,0.15,
	//DEPLOYED
	DRIVE,270,270,RIGHTWHITE,
	CLAWPOS,CLOSE,
	PAUSE,1.25,
	THROW,
	DRIVE,125,270,0.5,
	DRIVE,90,270,1.5,
	PAUSE,1,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,
	CLAWPOS,CLOSE,
	PAUSE,0.5,
	THROW,
	END
};

float SMALLLEFTFRONT[] = {
	270,
	DRIVE,125,270,0.5,
	DRIVE,90,270,0.75,
	ARMPOS,100,
	PAUSE,0.35,
	ARMPOS,1,
	CLAWPOS,300,
	PAUSE,0.15,
	//DEPLOYED
	DRIVE,270,270,RIGHTWHITE,
	CLAWPOS,CLOSE,
	PAUSE,1.25,
	THROW,
	DRIVE,55,270,0.5,
	DRIVE,90,270,1.5,
	PAUSE,1,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,
	CLAWPOS,CLOSE,
	PAUSE,0.5,
	THROW,
	END
};

//////////
// HANG //
//////////

float HANGLEFT[] = {
	0,
	DRIVE,180,0,1,
	ARMPOS,100,
	PAUSE,0.35,
	ARMPOS,1,
	CLAWPOS,2,
	PAUSE,0.15,
	TURN,335,1,
	DRIVE,270,0,0.1,
	DRIVE,355,315,1.3,
	TURN,160,1,
	ARMPOS,100,
	PAUSE,0.5,
	DRIVE,0,90,LEFTULTRA,16,3,
	DRIVE,90,135,0.25,
	TURN,160,1,
	DRIVE,270,135,1,
	HANG,
	END
};

float HANGRIGHT[] = {
	0,
	DRIVE,180,0,1,
	ARMPOS,100,
	PAUSE,0.35,
	ARMPOS,1,
	CLAWPOS,2,
	PAUSE,0.15,
	TURN,25,1,
	DRIVE,90,0,0.1,
	DRIVE,5,45,1.3,
	TURN,200,1,
	ARMPOS,100,
	PAUSE,0.5,
	DRIVE,0,270,RIGHTULTRA,16,3,
	DRIVE,270,225,0.25,
	TURN,200,1,
	DRIVE,90,225,1,
	HANG,
	END
};


float DESTROYLEFT[]={
	180,
	DRIVE,90,180,0.75,
	DRIVE,90,180,LEFTWHITE,
	DRIVE,270,180,0.15,
	ARMPOS,100,
	PAUSE,0.35,
	ARMPOS,1,
	CLAWPOS,(OPEN-50),
	PAUSE,0.15,
	DRIVE,180,180,1,
	CLAWPOS,CLOSE,
	PAUSE,1,
	ARMPOS,60,
	DRIVE,0,180,1.25,
	ARMPOS,60,
	TURN,280,1.5,
	THROW,
	PAUSE,0.5,
	DRIVE,90,270,2,
	END                                //End of table
};

float DESTROYRIGHT[]={	//GOOD
	180,
	DRIVE,270,180,0.75,
	DRIVE,270,180,LEFTWHITE,
	DRIVE,90,180,0.15,
	ARMPOS,100,
	PAUSE,0.35,
	ARMPOS,1,
	CLAWPOS,(OPEN-50),
	PAUSE,0.15,
	DRIVE,180,180,1,
	CLAWPOS,CLOSE,
	PAUSE,1,
	ARMPOS,60,
	DRIVE,0,180,1.25,
	ARMPOS,60,
	TURN,80,1.5,
	THROW,
	PAUSE,0.5,
	DRIVE,270,90,2,
	END                                //End of table
};



////////////
// SKILLS //
////////////

float SKILLS2 [] = {
	90,
	DRIVE,270,90,0.75,
	ARMPOS,100,
	PAUSE,0.35,
	ARMPOS,1,
	CLAWPOS,OPEN,
	PAUSE,0.15,
	//DEPLOYED
	DRIVE,90,90,0.5,
	CLAWPOS,CLOSE,
	PAUSE,0.5,
	DRIVE,270,90,1.5,
	THROW,
	DRIVE,270,90,0.25,
	PAUSE,2.25,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,
	//FOUR LOAD-STARS
	DRIVE,90,90,1.75,
	CLAWPOS,CLOSE,
	PAUSE,1.5,
	DRIVE,270,90,1.75,
	THROW,
	DRIVE,270,90,0.25,
	PAUSE,2.25,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,
	//DOUBLE LOAD-CUBE


	//START NAVIGATION
	CLAWPOS,CLOSE,
	DRIVE,90,0,0.5,
	CLAWPOS,OPEN,
	DRIVE,90,0,RIGHTWHITE,
	TURN,0,1,
	DRIVE,0,0,1.5,
	CLAWPOS,CLOSE,
	PAUSE,0.5,
	DRIVE,270,90,1.5,
	THROW,
	//MIDDLE CUBE
	DRIVE,270,90,0.25,
	PAUSE,2.25,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,

	DRIVE,45,180,0.5,
	CLAWPOS,CLOSE,
	DRIVE,45,180,LEFTWHITE,
	TURN,180,1,
	LINEFOLLOW,0,180,4,
	PAUSE,0.125,
	SETGYRO,180,
	PAUSE,0.125,

	DRIVE,225,90,1,
	CLAWPOS,300,
	DRIVE,270,90,0.5,
	DRIVE,90,90,1.75,
	CLAWPOS,CLOSE,
	PAUSE,0.5,
	DRIVE,270,90,1.5,
	THROW,
	DRIVE,270,90,0.25,
	PAUSE,2.25,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,

	DRIVE,90,180,1,
	DRIVE,0,180,1,
	DRIVE,90,225,RIGHTULTRA,50,
	DRIVE,45,225,2,
	HANG,
	END
};

float SKILLS3 [] = {
	90,
	DRIVE,235,90,0.5,
	DRIVE,270,90,0.75,
	ARMPOS,100,
	PAUSE,0.35,
	ARMPOS,1,
	CLAWPOS,300,
	PAUSE,0.15,
	//DEPLOYED
	DRIVE,90,90,RIGHTWHITE,
	CLAWPOS,CLOSE,
	PAUSE,0.25,																					//0.5
	DRIVE,305,90,0.5,
	DRIVE,270,90,0.5,
	DRIVE,270,90,RIGHTWHITE,
	THROW,///////////////////////////////////////
	DRIVE,270,90,0.125,
	PAUSE,2.25,																					//2.75
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,200,
	PAUSE,0.5,
	//FOUR LOAD-STARS
	DRIVE,115,90,0.75,
	DRIVE,90,90,RIGHTWHITE,
	CLAWPOS,CLOSE,
	PAUSE,0.25,
	DRIVE,270,90,1.5,
	DRIVE,270,90,RIGHTWHITE,
	THROW,/////////////////////////////////////
	DRIVE,270,90,0.25,
	PAUSE,2.25,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,
	//DOUBLE LOAD-CUBE


	//START NAVIGATION
	CLAWPOS,CLOSE,
	DRIVE,90,0,0.5,
	CLAWPOS,OPEN,
	DRIVE,90,0,RIGHTWHITE,
	DRIVE,90,0,0.01,
	TURN,0,0.25,
	CLAWPOS,300,
	DRIVE,0,0,0.75,
	CLAWPOS,200,
	DRIVE,0,0,1,
	CLAWPOS,CLOSE,
	PAUSE,0.5,
	DRIVE,270,90,2.25,
	THROW,
	//MIDDLE CUBE
	DRIVE,270,90,0.25,
	PAUSE,2.25,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,

	DRIVE,45,180,0.5,
	CLAWPOS,CLOSE,
	DRIVE,45,180,LEFTWHITE,
	DRIVE,270,180,0.125,


	LINEFOLLOW,0,180,3.5,
	PAUSE,0.125,
	SETGYRO,180,
	PAUSE,0.125,


	DRIVE,225,90,1,
	CLAWPOS,300,
	DRIVE,270,90,0.5,
	DRIVE,90,90,1.5,
	CLAWPOS,CLOSE,
	PAUSE,0.25,
	DRIVE,235,90,0.55,
	DRIVE,270,90,1,
	DRIVE,270,90,RIGHTWHITE,
	THROW,
	DRIVE,270,90,0.25,
	PAUSE,2.25,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,

	CLAWPOS,CLOSE,
	DRIVE,90,90,1,
	DRIVE,90,90,LEFTWHITE,
	CLAWPOS,CLOSE,
	TURN,180,0.75,
	DRIVE,0,180,2.5,
	SETGYRO,180,

	DRIVE,180,180,0.4,		//110->115
	DRIVE,90,180,HITWALL,
	DRIVE,45,225,1.25,
	CLAWPOS,200,
	HANG,

	END
};


float SKILLS4 [] = {
	90,
	DRIVE,235,90,0.5,
	DRIVE,270,90,0.75,
	ARMPOS,100,
	PAUSE,0.35,
	ARMPOS,1,
	CLAWPOS,300,
	PAUSE,0.15,
	//DEPLOYED
	DRIVE,90,90,RIGHTWHITE,
	CLAWPOS,CLOSE,
	PAUSE,0.25,																					//0.5
	DRIVE,305,90,0.25,
	DRIVE,270,90,0.5,
	DRIVE,270,90,RIGHTWHITE,
	THROW,///////////////////////////////////////
	DRIVE,270,90,0.125,
	PAUSE,2.25,																					//2.75
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,200,
	PAUSE,0.5,
	//FOUR LOAD-STARS
	DRIVE,115,90,0.75,
	DRIVE,90,90,RIGHTWHITE,
	CLAWPOS,CLOSE,
	PAUSE,0.25,
	DRIVE,270,90,1.5,
	DRIVE,270,90,RIGHTWHITE,
	THROW,/////////////////////////////////////
	DRIVE,270,90,0.25,
	PAUSE,2.25,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,
	//DOUBLE LOAD-CUBE


	//START NAVIGATION
	CLAWPOS,CLOSE,
	DRIVE,90,0,0.5,
	CLAWPOS,OPEN,
	DRIVE,90,0,RIGHTWHITE,
	DRIVE,90,0,0.2,
	TURN,0,0.25,
	CLAWPOS,300,
	DRIVE,15,0,0.75,
	CLAWPOS,200,
	DRIVE,0,0,1,
	CLAWPOS,CLOSE,
	PAUSE,0.5,
	DRIVE,270,90,2.25,
	THROW,
	//MIDDLE CUBE
	DRIVE,270,90,0.25,
	PAUSE,2.25,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,

	DRIVE,45,180,0.5,
	CLAWPOS,CLOSE,
	DRIVE,45,180,LEFTWHITE,
	DRIVE,270,180,0.125,


	LINEFOLLOW,0,180,3.5,
	PAUSE,0.125,
	SETGYRO,180,
	PAUSE,0.125,


	DRIVE,225,90,1,
	CLAWPOS,300,
	DRIVE,270,90,0.5,
	DRIVE,90,90,1.5,
	CLAWPOS,CLOSE,
	PAUSE,0.25,
	DRIVE,225,90,0.9,
	DRIVE,270,90,1,

	DRIVE,270,90,RIGHTWHITE,
	THROW,
	DRIVE,270,90,0.25,
	PAUSE,2.25,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,

	CLAWPOS,CLOSE,
	DRIVE,90,90,1,
	DRIVE,90,90,LEFTWHITE,
	DRIVE,270,90,0.15,
	CLAWPOS,CLOSE,
	TURN,180,0.75,
	DRIVE,0,180,2.5,
	SETGYRO,180,

	DRIVE,180,180,0.4,		//110->115
	DRIVE,90,180,HITWALL,
	DRIVE,45,225,1.25,
	CLAWPOS,200,
	HANG,

	END
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////
// SKILLS //
////////////

float SKILLS [] = {
	90,
	DRIVE,235,90,0.5,
	DRIVE,270,90,1,
	ARMPOS,120,
	PAUSE,0.4,
	ARMPOS,1,
	CLAWPOS,230,
	PAUSE,0.3,
	//DEPLOYED
	DRIVE,90,90,RIGHTWHITE,
	CLAWPOS,CLOSE,
	PAUSE,0.75,																					//0.5
	DRIVE,305,90,0.05,
	DRIVE,270,90,0.25,
	THROW,///////////////////////////////////////
	DRIVE,270,90,2,
	PAUSE,0.5,																					//2.75
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,200,
	PAUSE,0.5,
	//FOUR LOAD-STARS
	DRIVE,115,90,0.75,
	DRIVE,90,90,RIGHTWHITE,
	CLAWPOS,CLOSE,
	PAUSE,0.25,
	DRIVE,270,90,0.25,
	THROW,/////////////////////////////////////
	DRIVE,270,90,2,
	PAUSE,0.5,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,
	//DOUBLE LOAD-CUBE



	//START NAVIGATION
	CLAWPOS,CLOSE,
	DRIVE,90,0,0.5,
	CLAWPOS,OPEN,
	DRIVE,90,0,RIGHTWHITE,
	DRIVE,90,0,0.2,
	TURN,0,0.25,
	CLAWPOS,300,
	DRIVE,15,0,0.75,
	CLAWPOS,200,
	DRIVE,0,0,1,
	CLAWPOS,CLOSE,
	PAUSE,0.5,
	DRIVE,270,90,0.5,
	THROW,
	//MIDDLE CUBE
	DRIVE,270,90,2,
	PAUSE,0.25,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,

	DRIVE,45,180,0.5,
	CLAWPOS,CLOSE,
	DRIVE,45,180,LEFTWHITE,
	DRIVE,270,180,0.125,


	LINEFOLLOW,0,180,3.5,
	PAUSE,0.125,
	SETGYRO,180,
	PAUSE,0.125,


	DRIVE,225,90,1,
	CLAWPOS,300,
	DRIVE,270,90,0.5,
	DRIVE,90,90,1.5,
	CLAWPOS,CLOSE,
	PAUSE,0.25,
	DRIVE,225,90,0.9,
	DRIVE,270,90,0.25,
	THROW,///////////////////////////////////////
	DRIVE,270,90,2,
	PAUSE,0.25,
	CLAWPOS,CLOSE,
	ARMPOS,1,
	PAUSE,1,
	CLAWPOS,OPEN,
	PAUSE,0.5,

	CLAWPOS,CLOSE,
	DRIVE,90,90,1,
	DRIVE,90,90,LEFTWHITE,
	DRIVE,270,90,0.15,
	CLAWPOS,CLOSE,
	TURN,180,0.75,
	DRIVE,0,180,2.5,
	SETGYRO,180,

	DRIVE,180,180,0.4,		//110->115
	DRIVE,90,180,LEFTULTRA,16,3,
	DRIVE,45,225,1.25,
	CLAWPOS,200,
	HANG,


	END
};
