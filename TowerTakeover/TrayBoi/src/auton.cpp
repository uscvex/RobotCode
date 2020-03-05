////////////////////////////////////////////////
// TRAY BOT AUTON
//

#include "main.h"

using namespace pros;
using namespace std;

#define LIFT_DOWN_POS_A 650
#define LIFT_DOWN_POS_ACCEPT 800
#define LOW_TOWER_POS_A 2800
#define LOW_TOWER_POS_ACCEPT 2600
#define MID_TOWER_POS_A 3400
#define MID_TOWER_POS_ACCEPT 3200
#define TRAY_ARM_POS_A 600

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
#define DEPOSIT 14      // DEPOSIT
#define SLOW_TURN 15    // TURN, <FACEDIR>, <TIMEOUT>
#define LOCKPOS 16      // LOCKPOS, <POSITION>

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
#define LOCKBELOW 8
#define LOCKABOVE 9

#define WHITE_THRESHOLD_R 1000
#define WHITE_THRESHOLD_L 1000


int autonSelect = PROGRAMMING_SKILLS;
int numAutons = 3;


double programmingSkills[] = {
    270,
    //    DEPLOY,
    TRAYPOS,1,
    LIFTPOS,1,
    LOCKPOS,1,
    SETINTAKE,127,                          // FIRST ROW OF FOUR
    DRIVEDIST,40,270,55,10,
    SETINTAKE,127,
    DRIVEDIST,40,270,10,10,
    SETINTAKE,127,
    DRIVEDIST,40,270,76,10,                 // SECOND ROW OF FOUR
    PAUSE,0.25,
    DRIVEDIST,-40,270,3,1,
    
    SLOW_TURN,225,1,
    DRIVEDIST,40,225,15.5,2,
    
    SETINTAKE,0,
    DEPOSIT,                                    // FIRST STACK
    WAIT,DEPOSITDONE,15,15,
    SETINTAKE,0,
    
    DRIVEDIST,-50,225,6,1,
    TRAYPOS,1,
    LIFTPOS,400,
    TURN,270,1,
    LIFTPOS,1,
    LOCKPOS,250,
    PAUSE,0.5,
    SETINTAKE,127,
    DRIVEDIST,50,270,12,1,                      // FIRST CUBE
    PAUSE,0.5,
    DRIVEDIST,-50,270,22,2,
    
    SETINTAKE,0,
    TURN,180,1,
    
    TRAYPOS,TRAY_ARM_POS_A,
    LIFTPOS,LOW_TOWER_POS_A,
    WAIT,LIFTABOVE,LOW_TOWER_POS_ACCEPT,2,
    PAUSE,1,
    DRIVEDIST,60,180,15,1.5,
    DRIVEDIST,-40,180,1,1.5,
    LIFTPOS,2200,
    SETINTAKE,-40,
    PAUSE,0.25,
    DRIVEDIST,-40,180,26,3,
    LIFTPOS,600,
    TRAYPOS,1,
    
    SETINTAKE,0,
    TURN,90,1,
    LIFTPOS,1,
    SETINTAKE,127,
    DRIVEDIST,50,90,20,2,
    PAUSE,0.5,
    DRIVEDIST,-50,90,12,2,
    
    SETINTAKE,0,
    TRAYPOS,TRAY_ARM_POS_A,
    LIFTPOS,LOW_TOWER_POS_A,
    WAIT,LIFTABOVE,LOW_TOWER_POS_ACCEPT,2,
    PAUSE,1,
    DRIVEDIST,50,90,23,2,
    DRIVEDIST,-40,90,1,2,
    LIFTPOS,2200,
    SETINTAKE,-70,
    PAUSE,0.25,
    DRIVEDIST,-40,90,12,3,
    LIFTPOS,600,
    TRAYPOS,1,
    DRIVEDIST,-40,90,20,3,
    
    TURN,135,1,
    DRIVEDIST,-60,135,24,1,
    TURN,90,1,
    DRIVEDIST,-60,90,10,1,
    LIFTPOS,1,
    SETINTAKE,127,
    LOCKPOS,1,
    
    DRIVEDIST,40,90,140,12,
    DRIVEDIST,-40,90,10,2,
    
    SLOW_TURN,180,1,
    DRIVEDIST,50,180,30,3,
    SLOW_TURN,135,1,
    
    DRIVEDIST,50,135,20,3,
    
    SETINTAKE,-100,
    PAUSE,0.25,
    SETINTAKE,0,
    
    DEPOSIT,                                    // SECOND STACK
    WAIT,DEPOSITDONE,15,15,
    SETINTAKE,0,
    
    DRIVEDIST,-40,135,6,1,
    LIFTPOS,600,
    TURN,0,1,
    LIFTPOS,1,
    TRAYPOS,1,
    LOCKPOS,250,
    
    DRIVEDIST,60,0,20,2,
    SETINTAKE,127,
    DRIVEDIST,60,0,25,2,
    PAUSE,0.5,                                  // LAST CUBE
    DRIVEDIST,-60,0,10,2,
    TURN,45,0.75,
    DRIVEDIST,-60,45,25,2,
    
    TURN,180,1,
    SETINTAKE,0,
    TRAYPOS,TRAY_ARM_POS_A,
    LIFTPOS,LOW_TOWER_POS_A,
    WAIT,LIFTABOVE,LOW_TOWER_POS_ACCEPT,2,
    DRIVEDIST,60,180,25,2,
    
    DRIVEDIST,-40,180,1,1.5,
    LIFTPOS,2200,
    SETINTAKE,-40,
    PAUSE,0.25,
    DRIVEDIST,-40,180,26,3,
    
    END,
};









double redAuton[] = {
    270,
    DEPLOY,                         // DEPLOY
    WAIT,DEPLOYDONE,1,7,              // WAIT UNTIL DONE
    PAUSE,1,
    SETINTAKE,-127,
    DRIVEDIST,60,270,56,12,          // DRIVE LEVEL WITH 4 STACK
    // DRIVEDIST,-60,270,4,3,          // DRIVE LEVEL WITH 4 STACK
    DRIVE,-30,270,0.1,               // DRIVE TO BREAK
    TURN,180,2,                     // TURN TO FACE STACK
    
    SETINTAKE,-127,                 // RUN INTAKE OUT FOR TRAY
    TRAYPOS,5000,                   // LIFT TRAY TO OPEN INTAKE
    PAUSE,1,
    TRAYPOS,2000,
    SETINTAKE,0,                    // STOP INTAKE
    
    DRIVEDIST,20,180,11,5,          // DRIVE TO FOURSTACK
    SETINTAKE,127,                  // RUN INTAKE IN
    TRAYPOS,1,                      // LOWER TRAY
    DRIVEDIST,120,180,9,3,          // DRIVE INTO FOURSTACK
    PAUSE,0.5,                        // WAIT TO INTAKE CUBES

    DRIVEDIST,-20,180,5,5,         // BACK FROM FOUR
    TURN,90,2,
    DRIVEDIST,60,90,45,4,          // DRIVE TO GET CUBE
    
    DRIVEDIST,-60,90,10.5,5,
    TURN,180,2,
    
    DRIVEDIST,20,180,20,3,         // GET TOWER CUBE
    
    DRIVEDIST,-50,180,30,5,         // BACK UP TO ALIGN FOR NEXT
    
    SLOW_TURN,270,2,
    DRIVEDIST,60,270,35,5,         // GET NEXT CUBE
    PAUSE,0.25,
    DRIVEDIST,-60,270,25,5,         // DRIVE BACK
    SLOW_TURN,0,2,
    SETINTAKE,0,
    DRIVEDIST,60,0,50,3,            // DRIVE INTO WALL
    SETINTAKE,0,
    DRIVEDIST,-30,0,2,5,            // DRIVE AWAY FROM WALL
    SLOW_TURN,80,2,
    DRIVEDIST,60,80,30,4,
    DRIVEDIST,-60,80,1,0.5,
    SETINTAKE,-100,                  // GET CUBES TO CORRECT POS
    PAUSE,0.1,
    SETINTAKE,0,                  // STOP INTAKE
    
    DEPOSIT,
    DRIVE,40,80,1,
    WAIT,DEPOSITDONE,15,15,
    
    
    
    END
};

double blueAuton[] = {
    90,
    DEPLOY,                         // DEPLOY
    WAIT,DEPLOYDONE,1,7,              // WAIT UNTIL DONE
    PAUSE,1,
    SETINTAKE,-127,
    DRIVEDIST,60,90,54,12,          // DRIVE LEVEL WITH 4 STACK
    // DRIVEDIST,-60,270,4,3,          // DRIVE LEVEL WITH 4 STACK
    DRIVE,-30,90,0.1,               // DRIVE TO BREAK
    TURN,180,2,                     // TURN TO FACE STACK
    
    SETINTAKE,-127,                 // RUN INTAKE OUT FOR TRAY
    TRAYPOS,5000,                   // LIFT TRAY TO OPEN INTAKE
    PAUSE,1,
    TRAYPOS,2000,
    SETINTAKE,0,                    // STOP INTAKE
    
    DRIVEDIST,20,180,11,5,          // DRIVE TO FOURSTACK
    SETINTAKE,127,                  // RUN INTAKE IN
    TRAYPOS,1,                      // LOWER TRAY
    DRIVEDIST,120,180,9,3,          // DRIVE INTO FOURSTACK
    PAUSE,0.5,                        // WAIT TO INTAKE CUBES
    
    DRIVEDIST,-20,180,7,5,         // BACK FROM FOUR
    TURN,270,2,
    DRIVEDIST,60,270,45,4,          // DRIVE TO GET CUBE
    
    DRIVEDIST,-60,270,8,5,
    TURN,180,2,
    
    DRIVEDIST,20,180,20,3,         // GET TOWER CUBE
    
    DRIVEDIST,-50,180,23,5,         // BACK UP TO ALIGN FOR NEXT
    
    SLOW_TURN,90,2,
    DRIVEDIST,60,90,35,5,         // GET NEXT CUBE
    PAUSE,0.25,
    DRIVEDIST,-60,90,25,5,         // DRIVE BACK
    SLOW_TURN,0,2,
    SETINTAKE,0,
    DRIVEDIST,60,0,50,3,            // DRIVE INTO WALL
    SETINTAKE,0,
    DRIVEDIST,-30,0,2,5,            // DRIVE AWAY FROM WALL
    SLOW_TURN,290,2,
    DRIVEDIST,60,290,30,4,
    DRIVEDIST,-60,290,1,0.5,
    SETINTAKE,-100,                  // GET CUBES TO CORRECT POS
    PAUSE,0.1,
    SETINTAKE,0,                  // STOP INTAKE
    
    DEPOSIT,
    DRIVE,40,290,1,
    WAIT,DEPOSITDONE,15,15,
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
    int numCommands = 0;
    while (true) {
        
        if (controller.get_digital(DIGITAL_UP)) {
            return;
        }
        
        // If we are ready for the next command
        if (nextCommand) {
            numCommands++;
            autonTime = (millis() - autonStartTime) / 1000;
            controller.print(0,0,"T: %f, %d", autonTime, numCommands);
            
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
                    slowTurn = false;
                    driveMode = TURN;
                    driveSpeed = 0;
                    faceDir = processEntry();
                    commandTimeOut = processEntry() * 1000;
                    break;
                    
                case SLOW_TURN:
                    cout << "SLOW_TURN" << endl;
                    slowTurn = true;
                    driveMode = TURN;
                    driveSpeed = 0;
                    faceDir = processEntry();
                    commandTimeOut = processEntry() * 1000;
                    break;
                    
                case DRIVE: // Drive for a time
                    slowTurn = false;
                    cout << "DRIVETIME" << endl;
                    driveMode = DRIVE_TIME;
                    driveSpeed = processEntry();
                    faceDir = processEntry();
                    commandTimeOut = processEntry() * 1000;
                    break;
                    
                case DRIVEWHITE: // Drive until sensors see white/black
                    slowTurn = false;
                    cout << "DRIVEWHITE" << endl;
                    driveMode = DRIVE_WHITE;
                    driveSpeed = processEntry();
                    faceDir = processEntry();
                    driveDist = processEntry();
                    commandTimeOut = processEntry() * 1000;
                    break;
                    
                case DRIVEDIST: // Drive for a distance
                    slowTurn = false;
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
                    
                case TRAYPOS:   // Move tray to specified position
                    cout << "TRAYPOS" << endl;
                    traySeek = processEntry();
                    nextCommand = true;
                    break;
                    
                case LOCKPOS:   // Move lock to specified position
                    cout << "LOCKPOS" << endl;
                    lockSeek = processEntry();
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
                    controller.print(0,0,"DINGUS");
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
                    
                case LOCKBELOW:
                    if (lockPos < waitParameter) {
                        finishedWait = true;
                        cout << "Lock below done\n";
                    }
                    break;
                    
                case LOCKABOVE:
                    if (lockPos >= waitParameter) {
                        finishedWait = true;
                        cout << "Lock above done\n";
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
