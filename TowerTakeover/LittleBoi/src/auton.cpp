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
#define SETDIR 11       // SETDIR, <ANGLE>

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
    
    CLAWPOS,CLAW_OPEN_POS_A,            // OPEN CLAW
    LIFTPOS,LIFT_GRAB_POS,              // RAISE LIFT
    DRIVEDIST,100,270,40,5,             // DRIVE FORWARD
    LIFTPOS,1,                          // LOWER LIFT
    DRIVEDIST,100,280,20,3,             // DRIVE FORWARD MORE
    CLAWPOS,CLAW_CLOSE_POS_A,           // GRAB CUBE                        FIRST CUBE

    DRIVEWHITE,100,280,RIGHT_W,2,       // DRIVE TO CENTER LINE
    DRIVE,-60,270,0.05,                 // BREAK
    PAUSE,0.25,                         // PAUSE TO HOLD CUBE
    TURN,180,1,                         // TURN TO FACE TOWER
    DRIVEDIST,-60,180,2,1,              // DRIVE BACK
    LIFTPOS,LOW_TOWER_POS_A,            // RAISE LIFT
    WAIT,LIFTABOVE,1600,2,              // WAIT FOR LIFT TO GO UP

    DRIVEDIST,40,180,15,5,              // DRIVE TO TOWER
    //PAUSE,0.25,                         // PAUSE FOR STABLE
    CLAWPOS,CLAW_OPEN_POS_A,            // DROP CUBE                        FIRST TOWER
    PAUSE,0.5,                          // WAIT FOR CUBE TO FALL

    LIFTPOS,-1,                         // LOWER LIFT
    DRIVEDIST,-60,180,10,3,             // DRIVE AWAY FROM TOWER
    
    DRIVE,-100,180,0.5,                  // DRIVE INTO WALL
    DRIVEDIST,100,180,15,0.5,            // DRIVE AWAY FROM WALL
    DRIVE,-40,180,0.05,                 // BREAK
    
    LIFTPOS,LIFT_GRAB_POS,              // LOWER LIFT HARD
    //PAUSE,0.25,                         // WAIT FOR LIFT TO GO DOWN
    TURN,270,1,                         // TURN FOR NEXT DRIVE
    DRIVEDIST,-127,270,12,1,            // DRIVE AWAY FROM CENTER
    DRIVEWHITE,-100,270,LEFT_W,2,        // DRIVE TO TILE
    DRIVEDIST,-100,270,20,2,            // DRIVE TO LINE UP FOR NEXT CUBE
    DRIVE,60,270,0.05,                  // BREAK
    PAUSE,0.25,                         // PAUSE FOR STABLE

    TURN,180,1,                         // TURN FOR NEXT CUBE
    DRIVEDIST,70,180,26,2,              // DRIVE TO CUBE
    LIFTPOS,1,                          // LOWER LIFT
    PAUSE,0.5,                          // PAUSE TO GET CUBE
    DRIVEDIST,60,180,3,1,               // DRIVE MORE
    CLAWPOS,CLAW_CLOSE_POS_A,           // GRAB CUBE                        SECOND CUBE
    PAUSE,0.75,                         // PAUSE TO GET CUBE
    DRIVEDIST,70,180,3,1,               // DRIVE MORE

    DRIVEDIST,70,180,15,3,              // DRIVE TO TOWER
    DRIVEDIST,-60,180,5,3,              // DRIVE AWAY TOWER
    LIFTPOS,MID_TOWER_POS_A,            // RAISE LIFT
    WAIT,LIFTABOVE,2000,2,              // WAIT FOR LIFT TO GO UP
    DRIVEDIST,60,180,8,3,               // DRIVE TO TOWER
    PAUSE,0.25,                         // PAUSE FOR STABLE
    CLAWPOS,CLAW_OPEN_POS_A,            // DROP CUBE                        SECOND TOWER
    PAUSE,0.25,                         // WAIT FOR CUBE TO FALL

    DRIVEDIST,-40,180,10,3,             // DRIVE AWAY FROM TOWER
    LIFTPOS,LIFT_GRAB_POS,              // LOWER LIFT HARD
    PAUSE,0.25,                         // WAIT FOR LIFT TO GO DOWN

    DRIVEWHITE,-100,180,RIGHT_W,2,      // DRIVE TO TILE
    DRIVEDIST,-100,180,15,2,            // DRIVE TO LINE UP FOR CUBE
    DRIVE,60,180,0.05,                  // BREAK
    TURN,90,1,                          // TURN TO FACE CUBE

    DRIVEDIST,80,90,15,1,               // DRIVE TO NEXT CUBE
    LIFTPOS,1,                          // LOWER LIFT
    PAUSE,0.5,                          // WAIT FOR LIFT
    CLAWPOS,CLAW_CLOSE_POS_A,           // GRAB CUBE                        THIRD CUBE
    PAUSE,0.5,                          // WAIT FOR GRAB
    DRIVEDIST,-100,90,2,2,              // DRIVE AWAY FROM WALL

    TURN,180,1,                         // TURN FOR NEXT DRIVE
    CLAWPOS,CLAW_OPEN_POS_A,            // OPEN CLAW
    
    DRIVEDIST,127,180,40,8,             // DRIVE ACROSS FIELD               LONG DRIVE SOUTH
    CLAWPOS,CLAW_CLOSE_POS_A,           // CLOSE CLAW
    DRIVEDIST,127,185,52,8,             // DRIVE ACROSS FIELD
    DRIVE,-60,180,0.05,                 // BREAK
    
    TURN,270,1,                         // TURN FOR NEXT MOVE
    DRIVE,-100,270,0.5,                 // DRIVE INTO WALL
    
    SETDIR,270,                         // SET GYRO WHILE AGAINST WALL
    
    DRIVEDIST,100,270,29,3,             // DRIVE TO LINE UP FOR NEXT TOWER
    TURN,180,1,                         // TURN TO FACE NEXT TOWER
    
    DRIVEDIST,100,180,5,3,              // DRIVE NEAR TOWER
    LIFTPOS,LOW_TOWER_POS_A,            // RAISE LIFT
    WAIT,LIFTABOVE,1600,2,              // WAIT FOR LIFT
    DRIVEDIST,40,180,20,2.5,              // DRIVE TO TOWER
    CLAWPOS,CLAW_OPEN_POS_A,            // DROP CUBE                        THIRD TOWER
    //PAUSE,0.5,                          // WAIT FOR DROP
    
    DRIVEDIST,-40,180,10,3,             // DRIVE AWAY FROM TOWER
    LIFTPOS,LIFT_GRAB_POS,              // LOWER LIFT
    DRIVEDIST,-40,180,18,3,             // DRIVE TO LINE UP FOR NEXT CUBE
    TURN,270,1,                         // TURN TO FACE NEXT CUBE
    
    DRIVEDIST,70,270,19,3,              // DRIVE TO CUBE
    LIFTPOS,1,                          // MOVE LIFT DOWN
    PAUSE,0.5,                          // WAIT FOR LIFT DOWN
    CLAWPOS,CLAW_CLOSE_POS_A,           // GRAB CUBE                        FOURTH CUBE
    PAUSE,0.5,                          // WAIT FOR GRAB
    DRIVEDIST,-60,270,5,2,              // DRIVE BACK
    LIFTPOS,LOW_TOWER_POS_A,            // RAISE LIFT
    WAIT,LIFTABOVE,1600,2,              // WAIT FOR LIFT
    DRIVEDIST,40,270,14,2,              // DRIVE TO TOWER
    CLAWPOS,CLAW_OPEN_POS_A,            // DROP CUBE                        FOURTH TOWER
    //PAUSE,0.5,                          // WAIT FOR DROP
    DRIVEDIST,-40,270,5,2,              // DRIVE BACK
    LIFTPOS,-1,                         // LOWER LIFT
    DRIVEDIST,-70,270,10,2,             // DRIVE AWAY FROM TOWER
    LIFTPOS,1,                          // LOWER LIFT HARD
    DRIVEDIST,-127,270,51,3,            // DRIVE TO WALL
    
//    TURN,0,0.5,                         // WIGGLE TO DISLODGE CUBE
//    TURN,270,0.5,                       // WIGGLE BACK
//    DRIVE,-100,270,0.5,                 // DRIVE INTO WALL
    
    PAUSE,0.125,                        // SHORT PAUSE
    SETDIR,270,                         // SET GYRO
    
    DRIVEDIST,100,270,3,2,              // DRIVE AWAY FROM WALL
    DRIVE,-60,270,0.05,                 // BREAK
    TURN,0,1,                           // TURN FOR NEXT DRIVE
    DRIVEWHITE,100,0,LEFT_W,5,          // DRIVE TO TILE                    LONG DRIVE NORTH
    DRIVEDIST,100,0,2,1,                // DRIVE A BIT FURTHER
    LIFTPOS,LIFT_GRAB_POS,              // GET LIFT READY
    DRIVE,-60,0,0.05,                   // BREAK
    
    TURN,270,1,                         // TURN FOR NEXT CUBE
    DRIVE,-90,270,0.5,                  // DRIVE INTO WALL
    DRIVEDIST,90,270,40,3,              // DRIVE TO NEXT CUBE
    LIFTPOS,1,                          // MOVE LIFT DOWN
    DRIVEDIST,40,270,10,3,              // DRIVE A LITTLE MORE
    PAUSE,0.25,                         // PAUSE FOR GRAB
    CLAWPOS,CLAW_CLOSE_POS_A,           // GRAB CUBE                        FIFTH CUBE
    PAUSE,0.25,                         // PAUSE FOR GRAB
    TURN,225,1,                         // TURN FOR TOWER
    
    CLAWPOS,CLAW_OPEN_POS_A,            // OPEN CLAW
    DRIVEDIST,60,225,2,1,               // DRIVE FORWARD
    DRIVEDIST,70,225,10,2,              // DRIVE CLOSE TO TOWER
    CLAWPOS,CLAW_CLOSE_POS_A,           // GRAB
    PAUSE,0.5,                          // PAUSE FOR GRAB
    
    LIFTPOS,HIGH_TOWER_POS_A,           // RAISE LIFT
    WAIT,LIFTABOVE,3400,4,              // WAIT FOR LIFT
    DRIVEDIST,50,225,15,4,              // DRIVE TO TOWER
    CLAWPOS,CLAW_OPEN_POS_A,            // DROP CUBE                        FIFTH TOWER
    DRIVEDIST,40,225,1,4,               // DRIVE INTO TOWER
    PAUSE,0.5,                          // PAUSE FOR DROP
    
    DRIVEDIST,-40,225,5,4,              // DRIVE AWAY FROM TOWER
    LIFTPOS,-1,                         // LOWER LIFT
    DRIVEDIST,-40,225,5,4,              // DRIVE AWAY FROM TOWER
    LIFTPOS,LIFT_GRAB_POS,              // LOWER LIFT HARD
    WAIT,LIFTBELOW,400,2,               // WAIT UNTIL LIFT DOWN
    
    TURN,315,1,                         // TURN FOR NEXT CUBE
    
    DRIVEDIST,100,315,30,5,             // DRIVE TO TOWER
    DRIVEDIST,100,225,30,5,             // DRIVE TO TOWER
    LIFTPOS,1,                          // LOWER LIFT
    DRIVEDIST,100,225,8,5,              // DRIVE TO TOWER
    CLAWPOS,CLAW_CLOSE_POS_A,           // GRAB
    DRIVE,-60,270,0.05,                 // BREAK
    PAUSE,0.5,                          // PAUSE FOR GRAB
    
    TURN,225,1,                         // TURN TO FACE TOWER
    LIFTPOS,MID_TOWER_POS_A,            // RAISE LIFT
    WAIT,LIFTABOVE,2000,2,              // WAIT FOR LIFT
    DRIVEDIST,60,225,10,2,              // DRIVE TO TOWER
    CLAWPOS,CLAW_OPEN_POS_A,            // DROP CUBE
    
    END
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
