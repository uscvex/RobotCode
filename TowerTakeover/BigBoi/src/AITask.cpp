////////////////////////////////////////////////
// BIG BOT AI CHALLENGE CODE
//

#include "main.h"
#include <set>
using namespace std;

// Are we in AI mode?
bool AI_ACTIVE = false;

///////////////////////////////////////
// Targets
//
#define NO_TARGET -1
#define UNPROTECTED_GOAL 1          // Small goal zone
#define PROTECTED_GOAL_INNER 2      // Deep of big zone
#define PROTECTED_GOAL_OUTER 3      // Shallow big zone

///////////////////////////////////////
// Objectives
//
#define NO_OBJECTIVE -1

// Navigate to paramters
#define NAVIGATE_TO 1       // Objective is to move to an (x,y,theta) on the field
int nav_Target_X = 0;
int nav_Target_Y = 0;
int nav_Target_Theta = 0;

#define GET_CUBE 2          // Go to and intake cube, AI_Target will be int id of cube
#define PLACE_STACK 2       // Put stack into goal, AI_Target will be int representing which goal
int AI_Target = NO_TARGET;

int AI_Objective = NO_OBJECTIVE;    // Objective for the robot

// States for objective completion
#define RUNNING 1
#define SUCESS 2
#define FAILED 3
#define DO_NOT_DISTURB 4
#define STARTED 5
int AI_Status = SUCESS;


// Field Info
#define MAP_RESOLUTION 1000
set<Cube> cubes;            // Set of cube objects read from VEX software
// Field map for navigation
int fieldMap[MAP_RESOLUTION][MAP_RESOLUTION];
int numOurStacks = 0;       // Number of stacks in our goals
int cubesInTray = 0;        // Number of cubes in tray

// Task to generate accurate field maps
void mapField() {

    // Read data from vex software & little bot - put into cubes set
        // For each cube, also calculate its benefit
        // I.e. how much we gain stacking it, how much we gain placing it in tower
    
    // Regenerate map of field based on cubes & little bot position
    
    // Transmit data to little bot
    
}

// Task to monitor robot process & decide strategy
void strategyTask(void* params) {
    AI_ACTIVE = true;
    
    while (true) {
        
        mapField();
        
        // Variables to be set based on best objective
        int bestObjective = NO_OBJECTIVE;
        int bestTargetX = 0;
        int bestTargetY = 0;
        int bestTargetTheta = 0;
        int bestTarget = 0;
        
        
        ////////////////////////////////////////////////////////////////////////////////
        // Strategy Logic
        //
        
        
        ////////////////////////////////////////
        // Example, simple strategy
        // Get nearest cube
        // If tray is full, deposit stack
        //
        bestObjective = GET_CUBE;
        // Loop through cube set to find closest cube
        // Set bestTarget to ID of cube above
        bestTarget = 1;
        //
        // If the tray is full and theres room, go place the stack
        if (cubesInTray >= 12 && numOurStacks < 3) {
            bestObjective = PLACE_STACK;
            bestTarget = PROTECTED_GOAL_OUTER;
        }
        //
        // End of example
        ////////////////////////////////////////
        
        
        // Lastly, set the command
        // Check that we can interrupt
        if (AI_Status != DO_NOT_DISTURB) {
            AI_Status = STARTED;                    // Tell robot new task has been set
            
            AI_Objective = bestObjective;           // Tell robot what to do
            
            // Tell robot specifics of task
            nav_Target_X = bestTargetX;
            nav_Target_Y = bestTargetY;
            nav_Target_Theta = bestTargetTheta;
            AI_Target = bestTarget;
        }
        
        delay(20);
    }
    
}
