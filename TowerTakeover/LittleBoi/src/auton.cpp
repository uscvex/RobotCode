////////////////////////////////////////////////
// LITTLE BOT AUTON
//

#include "main.h"

using namespace pros;
using namespace std;

#define LOW_TOWER_POS_A 1900
#define LOW_TOWER_ACCEPT 1700
#define MID_TOWER_POS_A 2250
#define MID_TOWER_ACCEPT 2000
#define HIGH_TOWER_POS_A 3600
#define HIGH_TOWER_ACCEPT 3400
#define CLAW_OPEN_POS_A 1
#define CLAW_CLOSE_POS_A 600
#define CLOSE CLAW_CLOSE_POS_A
#define OPEN CLAW_OPEN_POS_A
#define LIFT_GRAB_POS 650

// Auton commands
#define TURN 1          // TURN, <FACEDIR>, <TIMEOUT>
#define DRIVE 2         // DRIVE, <DRIVESPEED>, <FACEDIR>, <TIMEOUT>
#define DRIVEDIST 3     // DRIVEDIST, <DRIVESPEED>, <FACEDIR>, <DISTANCE>, <TIMEOUT>
#define LIFTPOS 4       // LIFTPOS, <POSITION>
#define CLAWPOS 5       // CLASPOS, <POSITION>
#define END 7           // END
#define PAUSE 8         // PAUSE, <TIMEOUT>
#define WAIT 9          // WAIT, <CONDITION>, <PARAMETER>, <TIMEOUT>
#define DRIVEUNTIL 10   // DRIVEUNTIL, <DRIVESPEED>, <FACEDIR>, <TYPE>, <TIMEOUT>
#define SETDIR 11       // SETDIR, <ANGLE>

#define BOTH_W 1
#define BOTH_B 2
#define RIGHT_W 3
#define RIGHT_B 4
#define LEFT_W 5
#define LEFT_B 6
#define CUBE_PRESENT 7
#define HIT_TOWER 8

// Wait conditions
#define LIFTBELOW 1
#define LIFTABOVE 2
#define TIME 3

#define WHITE_THRESHOLD_R 2000
#define WHITE_THRESHOLD_L 2000


int autonSelect = PROGRAMMING_SKILLS;
int numAutons = 3;

double redAuton[] = {
    180,
    DRIVEDIST,-80,180,12,3,             // PUSH PRELOAD IN
    DRIVEDIST,70,180,3,3,               // DRIVE AWAY
    END
};

double blueAuton[] = {
    180,
    DRIVEDIST,-80,180,12,3,             // PUSH PRELOAD IN
    DRIVEDIST,70,180,3,3,               // DRIVE AWAY
    END
};

double programmingSkills[] = {
    270,

    DRIVEDIST,80,270,8,2,
    CLAWPOS,CLOSE,                              // FIRST CUBE
    PAUSE,0.125,
    TURN,180,1,
    LIFTPOS,MID_TOWER_POS_A,
    DRIVEDIST,60,180,4,2,
    WAIT,LIFTABOVE,MID_TOWER_ACCEPT,2,
    DRIVEDIST,50,180,12,2,
    PAUSE,0.375,
    CLAWPOS,OPEN,                               // FIRST TOWER
    PAUSE,0.25,
    DRIVEDIST,-40,180,8,2,
    LIFTPOS,-1,
    PAUSE,0.5,
    DRIVEDIST,-60,180,5,2,

    TURN,270,1,

    DRIVEDIST,50,270,12,2,
    CLAWPOS,CLOSE,                              // GRAB 1ST STACK OF FOUR
    DRIVEDIST,70,270,4,0.5,
    PAUSE,0.5,

    TURN,300,0.2,                               // SLOW TURN
    TURN,330,0.2,
    TURN,0,0.2,
    TURN,25,0.2,
    TURN,30,0.2,

    DRIVEDIST,60,30,20,5,
    LIFTPOS,LIFT_GRAB_POS,
    DRIVEDIST,60,55,36,5,
    TURN,45,0.25,
    DRIVEDIST,60,45,2,1,
    LIFTPOS,1,
    PAUSE,0.25,
    CLAWPOS,OPEN,                               // PLACE 1ST STACK OF FOUR
    PAUSE,0.5,
    DRIVEDIST,-60,45,15,5,

    TURN,180,1,
    DRIVEDIST,-60,180,25,2.5,                   // DRIVE TO HIT WALL
    DRIVE,-60,180,0.25,
    SETDIR,180,
    DRIVEDIST,60,180,26.5,2,

    TURN,270,1,

    DRIVEDIST,60,270,30,3,
    CLAWPOS,CLOSE,                              // SECOND CUBE
    PAUSE,0.25,
    LIFTPOS,LOW_TOWER_POS_A,
    DRIVEDIST,-30,270,3,2,
    WAIT,LIFTABOVE,LOW_TOWER_ACCEPT,2,
    DRIVEDIST,40,270,11,2,
    PAUSE,0.25,
    CLAWPOS,OPEN,                               // SECOND TOWER
    PAUSE,0.5,

    DRIVEDIST,-40,270,4,2,
    LIFTPOS,-1,
    PAUSE,0.75,

    //DRIVEDIST,-80,270,1,2,


    TURN,180,1,
    DRIVEDIST,-80,180,24,4,                     // DRIVE TO HIT WALL
    DRIVE,-80,180,0.25,
    PAUSE,0.25,
    SETDIR,180,
    
//    ///////////////
//    // START HERE
//    180,
//    ///////////////
    
    DRIVE,-80,180,0.75,
    PAUSE,0.25,
    SETDIR,180,
    DRIVEDIST,70,180,2,1,
    
    TURN,90,1,
    DRIVEUNTIL,-70,90,LEFT_W,2,
    DRIVEDIST,-70,90,17.5,4,
    TURN,180,1,
    
    DRIVEDIST,40,180,16,4,
    CLAWPOS,CLOSE,                              // GRAB 2ND STACK OF FOUR
    DRIVEDIST,70,180,5,0.5,
    PAUSE,0.5,
    
    
    TURN,210,0.2,                               // SLOW TURN
    TURN,240,0.2,
    TURN,270,0.2,
    TURN,300,0.2,
    TURN,330,0.2,
    
    DRIVEDIST,70,325,10,5,
    LIFTPOS,LIFT_GRAB_POS,
    DRIVEDIST,70,315,32,2,
    TURN,310,0.25,
    DRIVEDIST,70,310,13,2,
    LIFTPOS,1,
    PAUSE,0.25,
    CLAWPOS,OPEN,                               // PLACE 2ND STACK OF FOUR
    PAUSE,0.25,
    DRIVEDIST,-70,315,12,2,
    
    TURN,270,1,
    DRIVEUNTIL,70,270,RIGHT_W,2,
    DRIVEDIST,-60,270,2,0.5,

    TURN,180,1,
    PAUSE,0.25,
    DRIVEUNTIL,40,180,CUBE_PRESENT,2,
    CLAWPOS,CLOSE,                              // THIRD CUBE
    DRIVEDIST,70,180,5,1,
    PAUSE,0.25,
    
    TURN,280,1,
    DRIVEUNTIL,-70,280,RIGHT_W,4,
    DRIVEDIST,-60,280,2.5,1,
    DRIVEDIST,60,280,0.1,0.5,
    TURN,180,1,
    
    DRIVEDIST,-60,180,10,0.5,
    DRIVEDIST,60,180,2,0.5,
    
    LIFTPOS,HIGH_TOWER_POS_A,
    DRIVEDIST,60,180,1,0.5,
    WAIT,LIFTABOVE,HIGH_TOWER_ACCEPT,3,
    PAUSE,0.25,
    
    DRIVEDIST,40,180,11,2,
    //DRIVEUNTIL,40,180,HIT_TOWER,2,
    //PAUSE,0.225,
    CLAWPOS,OPEN,                               // THIRD TOWER
    PAUSE,0.25,
    
    DRIVEDIST,-60,180,3,2,
    LIFTPOS,-1,
    PAUSE,0.5,
    DRIVE,60,180,0.25,
    DRIVEDIST,-60,180,1,0.25,
    PAUSE,1,
    
    DRIVEDIST,-70,180,10,1,
    TURN,300,1,
    DRIVEDIST,70,300,10,1,
    DRIVEUNTIL,70,300,LEFT_W,2,
    DRIVEDIST,70,300,16,3,
    DRIVEDIST,-60,300,0.1,1,    // BREAK
    
    TURN,180,1,
    PAUSE,0.5,
    DRIVEUNTIL,40,180,CUBE_PRESENT,4,
    CLAWPOS,CLOSE,                              // FOURTH CUBE
    PAUSE,0.125,
    DRIVEDIST,-60,180,2,1,
    LIFTPOS,MID_TOWER_POS_A,
    WAIT,LIFTABOVE,MID_TOWER_ACCEPT,2,
    DRIVEDIST,50,180,10,1.25,
    PAUSE,0.25,
    CLAWPOS,OPEN,                               // FOURTH TOWER
    PAUSE,0.25,
    CLAWPOS,-1,
    
    END,
};


bool nextCommand = true;
double* nextEntry = NULL;

double autonTime = 0;

double processEntry() {
    double result = *nextEntry;
    nextEntry++;
    return result;
}

// Auton will be called at the start of autonomous mode
void autonomous() {
    
    cout << "Auton Starting\n";
    
    double autonStartTime = millis();       // Start time of auton mode
    double commandStartTime = millis();     // Start time of each command
    double commandTimeOut = -1;             // Time-out limit of each command
    
    double driveDist = 0;                   // How far in inches to drive
    
    bool commandTimedOut = false;           // Flag for if current command reached time-out
    
    int waitCondition = -1;                 // Condition type to wait until
    double waitParameter = -1;              // Parameter to decide when wait is done
    
    // Point to correct auton routine
    if (autonSelect == RED_AUTON) nextEntry = &redAuton[0];
    if (autonSelect == BLUE_AUTON) nextEntry = &blueAuton[0];
    if (autonSelect == PROGRAMMING_SKILLS) nextEntry = &programmingSkills[0];
    
    // Set robot current direction
    setDirection(processEntry());
    
    nextCommand = true;
    
    while (true) {
        
        if (controller.get_digital(DIGITAL_UP)) {
            return;
        }
        
        // If we are ready for the next command
        if (nextCommand) {
            
            autonTime = (millis() - autonStartTime) / 1000;
            controller.print(0,0,"T: %f", autonTime);
            
            nextCommand = false;            // We are not ready for next command any more
            commandStartTime = millis();    // Record the time the command was started
            commandTimeOut = -1;            // Default to no time-out limit
            commandTimedOut = false;        // This command has not yet timed out
            
            cout << ((millis() - autonStartTime) / 1000) << ": ";
            
            // Figure out which command we are supposed to perform
            switch ((int)processEntry()) {
                    
                case SETDIR:    // Set gyro direction
                    cout << "SETDIR" << endl;
                    setDirection(processEntry());
                    nextCommand = true;
                    break;
                    
                case PAUSE:     // Pause for a time (given in seconds)
                    cout << "PAUSE" << endl;
                    commandTimeOut = processEntry() * 1000;
                    break;
                    
                case WAIT:      // Wait until condition is met
                    cout << "WAIT" << endl;
                    waitCondition = (int)processEntry();
                    waitParameter = processEntry();
                    commandTimeOut = processEntry() * 1000;
                    break;
                    
                case TURN:      // Turn to face angle
                    cout << "TURN" << endl;
                    driveMode = TURN;
                    driveSpeed = 0;
                    faceDir = processEntry();
                    commandTimeOut = processEntry() * 1000;
                    break;
                    
                case DRIVE: // Drive for a time
                    cout << "DRIVETIME" << endl;
                    driveMode = DRIVE_TIME;
                    driveSpeed = processEntry();
                    faceDir = processEntry();
                    commandTimeOut = processEntry() * 1000;
                    break;
                    
                case DRIVEUNTIL: // Drive until sensors see white/black
                    cout << "DRIVEUNTIL" << endl;
                    driveMode = DRIVE_WHITE;
                    driveSpeed = processEntry();
                    faceDir = processEntry();
                    driveDist = processEntry();
                    commandTimeOut = processEntry() * 1000;
                    break;
                    
                case DRIVEDIST: // Drive for a distance
                    cout << "DRIVEDIST" << endl;
                    driveMode = DRIVE_DIST;
                    driveSpeed = processEntry();
                    faceDir = processEntry();
                    driveDist = processEntry() * TICKS_PER_INCH;
                    
                    // If we are going back, change drive distance
                    if (driveSpeed < 0)
                        driveDist *= -1;
                    
                    targetDrivePos = getDriveEncoderAvg() + driveDist;
                    
                    commandTimeOut = processEntry() * 1000;
                    break;
                    
                case LIFTPOS:   // Move lift to specified position
                    cout << "LIFTPOS" << endl;
                    liftSeek = processEntry();
                    nextCommand = true;
                    break;
                    
                case CLAWPOS:   // Move claw to specified position
                    cout << "CLAWPOS" << endl;
                    clawSeek = processEntry();
                    nextCommand = true;
                    break;
                    
                case END:       // Do nothing, we are finished
                    cout << "END" << endl;
                    break;
                    
                default:        // Command not recognised
                    cout << "BAD COMMAND" << endl;
                    break;
                    
            }
        }
        
        // Check if the command we were doing has a time-out limit set
        if (commandTimeOut > 0) {
            // Check if this limit has been reached
            if (millis() - commandStartTime > commandTimeOut) {
                commandTimeOut = -1;        // Reset time-out limit
                commandTimedOut = true;     // Set flag to say we timed out
                nextCommand = true;         // Ready for next command
                cout << "Timed out\n";
            }
        }
        
        // If we are doing wait command
        if (waitCondition > 0) {
            bool finishedWait = false;
            
            // Check what we are waiting for
            switch (waitCondition) {
                    
                case LIFTABOVE:     // Lift to be above position
                    if (liftPos > waitParameter) {
                        finishedWait = true;
                        cout << "Lift above done\n";
                    }
                    break;
                    
                case LIFTBELOW:     // Lift to be below position
                    if (liftPos < waitParameter) {
                        finishedWait = true;
                        cout << "Lift below done\n";
                    }
                    break;
                    
                case TIME:          // Time since auton start to be above time
                    if (millis() - autonStartTime > 1000 * waitParameter) {
                        finishedWait = true;
                        cout << "Wait time done\n";
                    }
                    break;
                    
            }
            
            // If condition is met the we are done
            if (finishedWait) {
                waitCondition = -1;     // Stop waiting
                waitParameter = -1;     // Reset parameter
                nextCommand = true;     // Ready for next command
            }
        }
        
        // Check if we've driven far enough
        if (driveMode == DRIVE_DIST) {
            bool finishedDrive = false;
            
            if (driveSpeed > 0) {
                if (getDriveEncoderAvg() > targetDrivePos)
                    finishedDrive = true;
            }
            else {
                if (getDriveEncoderAvg() < targetDrivePos)
                    finishedDrive = true;
            }
            
            if (finishedDrive) {
                faceDir = -1;
                driveSpeed = 0;
                driveMode = USER;
                nextCommand = true;
            }
        }
        
        
        // Check if we've hit the white/black area
        if (driveMode == DRIVE_WHITE) {
            bool finishedDrive = false;
            
            bool whiteR = (whiteLineR.get_value() < WHITE_THRESHOLD_R);
            bool whiteL = (whiteLineL.get_value() < WHITE_THRESHOLD_L);
            
            switch ((int)driveDist) {
                case BOTH_W:
                    if (whiteL && whiteR) {
                        finishedDrive = true;
                    }
                    break;
                case BOTH_B:
                    if (!whiteL && !whiteR) {
                        finishedDrive = true;
                    }
                    break;
                case RIGHT_W:
                    if (whiteR) {
                        finishedDrive = true;
                    }
                    break;
                case RIGHT_B:
                    if (!whiteR) {
                        finishedDrive = true;
                    }
                    break;
                case LEFT_W:
                    if (whiteL) {
                        finishedDrive = true;
                    }
                    break;
                case LEFT_B:
                    if (!whiteL) {
                        finishedDrive = true;
                    }
                    break;
                case CUBE_PRESENT:
                    if (clawLimit.get_value() == 1) {
                        finishedDrive = true;
                    }
                    break;
                    
                case HIT_TOWER:
                    if (towerLimit.get_value() == 1) {
                        finishedDrive = true;
                    }
                    break;
            }
            
            if (finishedDrive) {
                faceDir = -1;
                driveSpeed = 0;
                driveDist = 0;
                driveMode = USER;
                nextCommand = true;
            }
        }
        
        // If we timed out, make sure to stop whatever it was we were doing
        if (commandTimedOut) {
            
            // If we were waiting for a condition, we are not anymore
            if (waitCondition != -1) {
                waitCondition = -1;
                waitParameter = -1;
                cout << "Wait timed out\n";
            }
            
            // If we were driving
            if (driveMode != USER) {
                faceDir = -1;
                driveSpeed = 0;
                driveMode = USER;
            }
            
            
        }
        
        delay(20);
    }
}
