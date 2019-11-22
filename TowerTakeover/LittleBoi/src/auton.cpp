////////////////////////////////////////////////
// LITTLE BOT AUTON
//

#include "main.h"

using namespace pros;
using namespace std;

#define LOW_TOWER_POS_A 1800
#define MID_TOWER_POS_A 2250
#define HIGH_TOWER_POS_A 3600
#define CLAW_OPEN_POS_A 1
#define CLAW_CLOSE_POS_A 180
#define LIFT_GRAB_POS 650

// Auton commands
#define TURN 1          // TURN, <FACEDIR>, <TIMEOUT>
#define DRIVE 2         // DRIVE, <DRIVESPEED>, <FACEDIR>, <TIMEOUT>
#define DRIVEDIST 3     // DRIVE, <DRIVESPEED>, <FACEDIR>, <DISTANCE>, <TIMEOUT>
#define LIFTPOS 4       // LIFTPOS, <POSITION>
#define CLAWPOS 5       // CLASPOS, <POSITION>
#define END 7           // END
#define PAUSE 8         // PAUSE, <TIMEOUT>
#define WAIT 9          // WAIT, <CONDITION>, <PARAMETER>, <TIMEOUT>
#define DRIVEWHITE 10   // DRIVE, <DRIVESPEED>, <FACEDIR>, <TYPE>, <TIMEOUT>

#define BOTH_W 1
#define BOTH_B 2
#define RIGHT_W 3
#define RIGHT_B 4
#define LEFT_W 5
#define LEFT_B 6

// Wait conditions
#define LIFTBELOW 1
#define LIFTABOVE 2
#define TIME 3

#define WHITE_THRESHOLD_R 300
#define WHITE_THRESHOLD_L 300



int autonSelect = 2;
int numAutons = 3;

double redAuton[] = {
    180,
    
    END
};

double blueAuton[] = {
    180,
    
    END
};

double programmingSkills[] = {
    270,
    CLAWPOS,CLAW_CLOSE_POS_A,
    PAUSE,0.25,
    DRIVEDIST,127,270,1,1,          // DRIVE AWAY FROM WALL
    DRIVE,-60,270,0.05,             // BREAK
    TURN,220,1,                     // TURN TO FACE TOWER
    LIFTPOS,MID_TOWER_POS_A,        // RAISE LIFT
    WAIT,LIFTABOVE,2000,2,          // WAIT TILL UP
    DRIVEDIST,40,220,14,3,          // DRIVE TO TOWER
    //PAUSE,1,                        // WAIT TO STOP SWAY
    CLAWPOS,CLAW_OPEN_POS_A,        // DROP CUBE                    FIRST TOWER
    PAUSE,0.125,                    // WAIT TO DROP
    LIFTPOS,-1,                     // LOWER LIFT

    DRIVEDIST,-60,225,14,2,         // DRIVE BACK
    DRIVEWHITE,-60,180,RIGHT_W,2,   // DRIVE TO RED TILE
    DRIVEDIST,-60,180,1,1,          // DRIVE BACK A LITTLE MORE
    DRIVE,60,180,0.05,              // BREAK

    LIFTPOS,LIFT_GRAB_POS,          // RAISE LIFT
    TURN,270,1,                     // TURN TO FACE NEXT CUBE

    DRIVEDIST,40,270,14,2,          // DRIVE TO NEXT CUBE
    LIFTPOS,1,                      // RAISE LIFT
    PAUSE,0.5,                      // WAIT FOR TIME
    DRIVEDIST,50,270,2,2,           // DRIVE INTO NEXT CUBE
    CLAWPOS,CLAW_CLOSE_POS_A,       // GRAB IT
    PAUSE,0.25,                     // WAIT FOR TIME

    TURN,180,1,                     // AIM FOR NEXT TOWER
    DRIVEDIST,60,180,10,2,          // DRIVE IN BETWEEN CUBES

    TURN,270,1,                     // TURN FOR NEXT MOVE

    CLAWPOS,CLAW_OPEN_POS_A,
    DRIVEDIST,60,270,35,4,          // DRIVE READY FOR TURN
    CLAWPOS,CLAW_CLOSE_POS_A,
    PAUSE,0.5,

    TURN,225,1,                     // TURN TO FACE TOWER

    LIFTPOS,HIGH_TOWER_POS_A,       // RAISE LIFT
    WAIT,LIFTABOVE,3200,2,          // WAIT TILL UP

    DRIVEDIST,45,225,13,3,          // DRIVE TO TOWER

    //PAUSE,1,                      // WAIT FOR STABLE
    CLAWPOS,CLAW_OPEN_POS_A,        // DROP CUBE                            HIGH TOWER
    DRIVEDIST,45,225,2,3,           // DRIVE TO TOWER
    PAUSE,0.5,                      // WAIT FOR CUBE TO FALL

    LIFTPOS,-1,                     // BRING LIFT DOWN
    DRIVEDIST,-40,225,10,2,          // DRIVE BACK FROM TOWER
    WAIT,LIFTBELOW,200,1,           // WAIT TILL LIFT IS DOWN
    PAUSE,0.5,

    TURN,270,1,                     // TURN TO FACE NEXT TOWER

    DRIVEWHITE,60,270,RIGHT_W,2,    // DRIVE TO CENTER LINE
    DRIVEDIST,60,270,2,2,           // DRIVE A BIT FURTHER
    DRIVE,-60,270,0.05,             // BREAK

    LIFTPOS,LIFT_GRAB_POS,          // RAISE LIFT TO GRAB
    TURN,0,1,                       // TURN TO FACE NEXT CUBE

    DRIVEDIST,60,0,4,2,             // DRIVE TO CUBE
    LIFTPOS,1,                      // MOVE LIFT DOWN
    PAUSE,0.25,                     // WAIT WHILE LIFT MOVES
    DRIVEDIST,60,0,1,2,             // DRIVE INTO CUBE MORE
    CLAWPOS,CLAW_CLOSE_POS_A,       // GRAB
    PAUSE,0.5,                      // WAIT FOR GRAB
    DRIVEDIST,-60,0,2,2,            // DRIVE AWAY FROM TOWER
    LIFTPOS,LOW_TOWER_POS_A,        // RAISE LIFT
    WAIT,LIFTABOVE,1600,2,          // WAIT UNTIL UP
    DRIVEDIST,50,0,10,3,            // DRIVE TO TOWER
    CLAWPOS,CLAW_OPEN_POS_A,        // DROP CUBE
    PAUSE,1,                        // LET CUBE DROP

    DRIVEDIST,-50,0,8,2,            // DRIVE BACK
    LIFTPOS,-1,                     // LOWER LIFT
    WAIT,LIFTBELOW,200,2,           // WAIT UNTIL LIFT DOWN

    //0,      //////////// REMOVE THIS LINE
    
    TURN,315,1,                     // TURN TO FACE NEXT TARGET
    DRIVEDIST,70,315,10,2,          // DRIVE CLEAR OF LINE
    DRIVEWHITE,70,315,RIGHT_W,3,    // DRIVE TO LINE
    DRIVE,-40,315,0.05,             // BREAK
    TURN,0,1,                       // TURN TO LINE UP
    
    DRIVEDIST,70,0,2,1,             // DRIVE TO LINE UP
    TURN,270,1,                     // TURN TO FACE CUBE
    DRIVEDIST,-70,270,10,2,         // DRIVE OFF TILE
    DRIVEWHITE,70,270,LEFT_W,2,     // DRIVE ON TILE
    DRIVE,-40,270,0.05,             // BREAK
    LIFTPOS,LIFT_GRAB_POS,          // RAISE LIFT
    CLAWPOS,CLAW_OPEN_POS_A,        // MAKE SURE CLAW IS OPEN
    PAUSE,0.5,                      // SHORT PAUSE
    DRIVEDIST,60,270,18,4,          // DRIVE TO CUBE
    LIFTPOS,1,                      // LOWER LIFT
    DRIVEDIST,70,270,4,2,           // DRIVE FURTHER
    CLAWPOS,CLAW_CLOSE_POS_A,       // CLOSE CLAW
    PAUSE,0.5,                      // WAIT FOR GRAB
    
    TURN,180,1,                     // TURN FOR NEXT TOWER
    CLAWPOS,CLAW_OPEN_POS_A,        // OPEN CLAW
    
    
    DRIVEDIST,80,180,25,5,          // DRIVE TO NEXT TOWER
    CLAWPOS,CLAW_CLOSE_POS_A,       // CLOSE CLAW
    DRIVEDIST,80,180,5,1,           // DRIVE TO NEXT TOWER
    
    LIFTPOS,MID_TOWER_POS_A,        // RAISE LIFT
    WAIT,LIFTABOVE,2000,2,          // WAIT FOR LIFT TO GO UP
    DRIVEDIST,40,180,15,3,          // DRIVE TO TOWER
    
    CLAWPOS,CLAW_OPEN_POS_A,        // DROP CUBE
    PAUSE,0.5,                      // LET CUBE FALL
    DRIVEDIST,-50,180,10,2,         // DRIVE AWAY FROM TOWER
    LIFTPOS,-1,                     // LOWER LIFT
    WAIT,LIFTBELOW,200,2,           // WAIT TILL LIFT DOWN
    

    END
};


bool nextCommand = true;
double* nextEntry = NULL;

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
        
        // If we are ready for the next command
        if (nextCommand) {
            
            nextCommand = false;            // We are not ready for next command any more
            commandStartTime = millis();    // Record the time the command was started
            commandTimeOut = -1;            // Default to no time-out limit
            commandTimedOut = false;        // This command has not yet timed out
            
            cout << ((millis() - autonStartTime) / 1000) << ": ";
            
            // Figure out which command we are supposed to perform
            switch ((int)processEntry()) {
                    
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
                    
                case DRIVEWHITE: // Drive until sensors see white/black
                    cout << "DRIVEWHITE" << endl;
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
