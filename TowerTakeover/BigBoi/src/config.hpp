////////////////////////////////////////////////
// BIG BOT CONFIG
//


#ifndef _CONFIG_H_
#define _CONFIG_H_ 1

////////////////////////////////////////////////////////////////////////////////////////
// CONFIG FILE FOR ALL GLOBAL VARS, MOTORS, SENSORS, ETC.
// Please put all in here with extern prefix, then also in file they are used in
// All #defines should be in this file too
//
#include <set>

//////////////////////////////////////////
// Controls
//
// Joystick dead zone
#define JOYZONE 10


//////////////////////////////////////////
// Speed Motor Gearsets
//
#define SPEED E_MOTOR_GEARSET_18
#define TORQUE E_MOTOR_GEARSET_36

// Drive modes
#define USER 0
#define TURN 1
#define DRIVE_TIME 2
#define DRIVE_DIST 3
#define DRIVE_DEPOSIT 4
#define DRIVE_DEPLOY 5
#define DRIVE_TO 6
#define DRIVE_PLACE 7

#define RED_AUTON 0
#define BLUE_AUTON 1
#define PROGRAMMING_SKILLS 2

#define INTAKE_ARM_IN_POS 1
#define INTAKE_ARM_OUT_POS 800


// Function prototypes
void runDrive(void* params);
void runLift(void* params);
void trackPosition();
void init();

void setDirection(double dir);
double pythag(double x1, double y1, double x2, double y2);
void setPosition(double x, double y, double d);
void setPosition(double x, double y);


// Declare motors and controller objects
extern Controller controller;

// Drive
extern Motor mFRA;
extern Motor mFRB;
extern Motor mBRA;
extern Motor mBRB;
extern Motor mFLA;
extern Motor mFLB;
extern Motor mBLA;
extern Motor mBLB;

// Lift
extern Motor mLiftR;
extern Motor mLiftL;

// Inner intake
extern Motor mInIntakeR;
extern Motor mInIntakeL;

// Outer intake
extern Motor mOutIntakeR;
extern Motor mOutIntakeL;

// Intake arms
extern Motor mIntakeArmR;
extern Motor mIntakeArmL;

// Position tracking encoders
extern ADIEncoder rightEncoder;
extern ADIEncoder leftEncoder;
extern ADIEncoder backEncoder;

// Vars for auton control of mechanisms
extern double liftSeek;
extern double intakeArmSeekLeft;
extern double intakeArmSeekRight;
extern double runIntake;
extern bool onlyInner;

// Have we initialized yet?
extern bool haveInitialized;

// Vars to save robot global position
extern double globalDirection;
extern double globalX;
extern double globalY;


// Vars to save previous encoder values
extern double lastRightEncoder;
extern double lastLeftEncoder;
extern double lastBackEncoder;

extern int driveMode;
extern double faceDir;
extern double driveDir;
extern double driveSpeed;
extern double startingDrivePosX;
extern double startingDrivePosY;
extern double distanceToDrive;

extern double liftPos;
extern double intakeArmPosLeft;
extern double intakeArmPosRight;

extern int autonSelect;
extern int numAutons;

extern int depositStep;
extern int deployStep;
extern bool doneDeploy;

extern double driveToX;
extern double driveToY;

extern double lastAutonTime;



struct Cube {
    int id;
    double xPos;
    double yPos;
    double theta;
    double zPos;
    char color;     // 'o', 'p', 'g'
    double stackBenefit;
    double towerBenefit;
    Cube(double id_, double xPos_, double yPos_, double theta_,
    	double zPos_, char color_) :
    	id(id_), xPos(xPos_), yPos(yPos_), theta(theta_),
    	zPos(zPos_), color(color_) {}
};

struct Node;

struct Edge {
    Node* next;
    double edge_weight;
    Edge(Node* n, double weight) : next(n), edge_weight(weight) {}
};

struct Node {
	Cube cube;
	std::set<Edge> edges;
	Node(Cube myself) : cube(myself) {}
};



#endif
