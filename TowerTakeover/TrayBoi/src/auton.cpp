////////////////////////////////////////////////
// TRAY BOT AUTON
//

#include "main.h"

using namespace pros;
using namespace std;

#define LIFT_DOWN_POS_A 650
#define LIFT_DOWN_POS_ACCEPT 800
#define LOW_TOWER_POS_A 1800
#define LOW_TOWER_POS_ACCEPT 1700
#define MID_TOWER_POS_A 2250
#define MID_TOWER_POS_ACCEPT 2150

// Auton commands
#define TURN 1          // TURN, <FACEDIR>, <TIMEOUT>
#define DRIVE 2         // DRIVE, <DRIVESPEED>, <FACEDIR>, <TIMEOUT>
#define DRIVEDIST 3     // DRIVE, <DRIVESPEED>, <FACEDIR>, <DISTANCE>, <TIMEOUT>
#define LIFTPOS 4       // LIFTPOS, <POSITION>
#define TRAYPOS 5       // TRAYPOS, <POSITION>
#define END 7           // END
#define PAUSE 8         // PAUSE, <TIMEOUT>
#define WAIT 9          // WAIT, <CONDITION>, <PARAMETER>, <TIMEOUT>
#define DRIVEWHITE 10   // DRIVE, <DRIVESPEED>, <FACEDIR>, <TYPE>, <TIMEOUT>
#define SETDIR 11       // SETDIR, <ANGLE>
#define SETINTAKE 12    // SETINTAKE, <SPEED>
#define DEPLOY 13       // DEPLOY
#define DEPOSIT 14       // DEPOSIT

#define BOTH_W 1
#define BOTH_B 2
#define RIGHT_W 3
#define RIGHT_B 4
#define LEFT_W 5
#define LEFT_B 6

// Wait conditions
#define LIFTBELOW 1
#define LIFTABOVE 2
#define TRAYBELOW 3
#define TRAYABOVE 4
#define TIME 5
#define DEPLOYDONE 6
#define DEPOSITDONE 7

#define WHITE_THRESHOLD_R 1000
#define WHITE_THRESHOLD_L 1000


int autonSelect = PROGRAMMING_SKILLS;
int numAutons = 3;

double redAuton[] = {
    270,
    DEPLOY,                         // DEPLOY
    WAIT,DEPLOYDONE,7,              // WAIT UNTIL DONE
    PAUSE,0.5,
    
    DRIVEDIST,60,270,41,8,          // DRIVE LEVEL WITH 4 STACK
    TURN,180,1,                     // TURN TO FACE STACK
    
    SETINTAKE,-127,                 // RUN INTAKE OUT FOR TRAY
    TRAYPOS,5000,                   // LIFT TRAY TO OPEN INTAKE
    PAUSE,1,
    TRAYPOS,2000,
    SETINTAKE,0,                    // STOP INTAKE
    
    DRIVEDIST,20,180,10,5,          // DRIVE TO FOURSTACK
    SETINTAKE,127,                  // RUN INTAKE IN
    TRAYPOS,1,                      // LOWER TRAY
    DRIVEDIST,127,180,10,3,         // DRIVE INTO FOURSTACK
    PAUSE,3,                        // WAIT TO INTAKE CUBES
    
    
    
    END
};

double blueAuton[] = {
    90,
    DEPLOY,
    END
};

double programmingSkills[] = {
    270,
    DEPLOY,
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
                    
                case TRAYPOS:   // Move claw to specified position
                    cout << "TRAYPOS" << endl;
                    traySeek = processEntry();
                    nextCommand = true;
                    break;
                    
                case SETINTAKE:
                    cout << "SETINTAKE" << endl;
                    runIntake = processEntry();
                    nextCommand = true;
                    break;
                    
                case DEPLOY:
                    cout << "DEPLOY" << endl;
                    deployStep = 1;
                    nextCommand = true;
                    break;
                    
                case DEPOSIT:
                    cout << "DEPOSIT" << endl;
                    depositStep = 1;
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
                    
                case TRAYABOVE:     // Tray to be above position
                    if (trayPos > waitParameter) {
                        finishedWait = true;
                        cout << "Tray above done\n";
                    }
                    break;
                    
                case TRAYBELOW:     // Tray to be below position
                    if (trayPos < waitParameter) {
                        finishedWait = true;
                        cout << "Tray below done\n";
                    }
                    break;
                    
                case DEPLOYDONE:     // Deploy to be done
                    if (deployStep == -1) {
                        finishedWait = true;
                        cout << "Deploy done\n";
                    }
                    break;
                    
                case DEPOSITDONE:     // Deposit to be done
                    if (depositStep == -1) {
                        finishedWait = true;
                        cout << "Deposit done\n";
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
