////////////////////////////////////////////////
// BIG BOT AUTON
//

#include "main.h"

using namespace pros;
using namespace std;

#define LOW_TOWER_POS_A 1800
#define MID_TOWER_POS_A 2250
#define HIGH_TOWER_POS_A 3600
#define CLAW_OPEN_POS_A 1
#define CLAW_CLOSE_POS_A 180


                            // FIRST LINE: <X>, <Y>, <DIRECTION>
// Auton commands
#define TURN 1              // TURN, <FACEDIR>, <TIMEOUT>
#define DRIVE 2             // DRIVE, <SPEED>, <DRIVEDIR>, <FACEDIR>, <TIMEOUT>
#define DRIVEDIST 3         // DRIVE, <SPEED>, <DRIVEDIR>, <FACEDIR>, <DISTANCE> <TIMEOUT>
#define LIFTPOS 4           // LIFTPOS, <POSITION>
#define INTAKEPOS 5         // INTAKEPOS, <POSITION>
#define END 7               // END
#define PAUSE 8             // PAUSE, <TIMEOUT>
#define WAIT 9              // WAIT, <CONDITION>, <PARAMETER>, <TIMEOUT>
#define INTAKESPEED 10      // INTAKESPEED, <SPEED>
#define INTAKEPOSLEFT 11    // INTAKEPOSLEFT, <POSITION>
#define INTAKEPOSRIGHT 12   // INTAKEPOSRIGHT, <POSITION>
#define DEPOSIT 13          // DEPOSIT
#define DEPLOY 14           // DEPLOY
#define DRIVETO 15          // DRIVE TO, <SPEED>, <FACEDIR>, <X>, <Y>, <TIMOUT>

// Wait conditions
#define LIFTBELOW 1
#define LIFTABOVE 2
#define INTAKEARMLEFTBELOW 3
#define INTAKEARMLEFTABOVE 4
#define INTAKEARMRIGHTBELOW 5
#define INTAKEARMRIGHTABOVE 6
#define TIME 7
#define DEPOSITDONE 8
#define DEPLOYDONE 9

int autonSelect = 2;
int numAutons = 3;
double lastAutonTime = 0;

double redAuton[] = {
    180,
    
    END
};

double blueAuton[] = {
    180,
    
    END
};

double programmingSkills[] = {
    -60,-45,270,
    
    DEPLOY,
    WAIT,DEPLOYDONE,15,
    WAIT,LIFTBELOW,12000,5,
    DRIVETO,40,270,-60,-45,1,
    
    INTAKESPEED,127,
    INTAKEPOS,INTAKE_ARM_OUT_POS,
    WAIT,INTAKEARMLEFTABOVE,600,1,
    DRIVETO,40,270,-30,-45,9,
    DRIVETO,40,280,-26,-40,2,
    DRIVETO,40,270,-30,-45,2,
    
    DRIVETO,50,245,0,-54,3,
    DRIVETO,50,280,12,-50,4,
    DRIVETO,50,280,44,-42,9,
    INTAKEPOS,INTAKE_ARM_IN_POS,
    DRIVETO,70,225,58,-58,5,
    
    DEPOSIT,
    WAIT,DEPOSITDONE,10,
    
    DRIVETO,50,20,
    
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
    
    bool commandTimedOut = false;           // Flag for if current command reached time-out
    
    int waitCondition = -1;                 // Condition type to wait until
    double waitParameter = -1;              // Parameter to decide when wait is done
    
    // Point to correct auton routine
    if (autonSelect == RED_AUTON) nextEntry = &redAuton[0];
    if (autonSelect == BLUE_AUTON) nextEntry = &blueAuton[0];
    if (autonSelect == PROGRAMMING_SKILLS) nextEntry = &programmingSkills[0];
    
    // Set robot current position
    setPosition(processEntry(),processEntry(),processEntry());
    
    nextCommand = true;
    
    while (true) {
        
        // If we are ready for the next command
        if (nextCommand) {
            
            lastAutonTime = (millis() - autonStartTime) / 1000; // Record time
            
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
                    if (waitCondition != DEPOSITDONE && waitCondition != DEPLOYDONE)
                        waitParameter = processEntry();
                    commandTimeOut = processEntry() * 1000;
                    break;
                    
                case TURN:      // Turn to face angle
                    cout << "TURN" << endl;
                    driveMode = TURN;
                    driveSpeed = 0;
                    faceDir = processEntry();
                    driveDir = 0;
                    commandTimeOut = processEntry() * 1000;
                    break;
                    
                case DRIVE: // Drive for a time
                    cout << "DRIVETIME" << endl;
                    driveMode = DRIVE_TIME;
                    driveSpeed = processEntry();
                    driveDir = processEntry();
                    faceDir = processEntry();
                    commandTimeOut = processEntry() * 1000;
                    break;
                    
                case DRIVEDIST: // Drive for a distance
                    cout << "DRIVEDIST" << endl;
                    driveMode = DRIVE_DIST;
                    driveSpeed = processEntry();
                    driveDir = processEntry();
                    faceDir = processEntry();
                    distanceToDrive = processEntry();
                
                    // Record starting position
                    startingDrivePosX = globalX;
                    startingDrivePosY = globalY;
                    
                    commandTimeOut = processEntry() * 1000;
                    break;
                    
                case DRIVETO: // Drive to a point
                    cout << "DRIVETO" << endl;
                    driveMode = DRIVE_TO;
                    driveSpeed = processEntry();
                    driveDir = -1;
                    
                    faceDir = processEntry();
                    driveToX = processEntry();
                    driveToY = processEntry();
                    
                    commandTimeOut = processEntry() * 1000;
                    break;
                    
                case LIFTPOS:   // Move lift to specified position
                    cout << "LIFTPOS" << endl;
                    liftSeek = processEntry();
                    nextCommand = true;
                    break;
                    
                case INTAKEPOS:   // Move intake arms to specified position
                    cout << "INTAKEPOS" << endl;
                    intakeArmSeekLeft = processEntry();
                    intakeArmSeekRight = intakeArmSeekLeft;
                    nextCommand = true;
                    break;
                    
                case INTAKEPOSLEFT:   // Move left intake arm to specified position
                    cout << "INTAKEPOS" << endl;
                    intakeArmSeekLeft = processEntry();
                    nextCommand = true;
                    break;
                    
                case INTAKEPOSRIGHT:   // Move right intake arm to specified position
                    cout << "INTAKEPOSRIGHT" << endl;
                    intakeArmSeekRight = processEntry();
                    nextCommand = true;
                    break;
                    
                case INTAKESPEED:       // Set intake to run at speed
                    cout << "INTAKESPEED";
                    runIntake = processEntry();
                    nextCommand = true;
                    break;
                    
                case DEPOSIT:           // Tell robot to deposit stack
                    cout << "DEPOSIT";
                    depositStep = 1;
                    nextCommand = true;
                    break;
                    
                case DEPLOY:           // Tell robot to deploy
                    cout << "DEPLOY";
                    deployStep = 1;
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
                    
                case INTAKEARMLEFTABOVE:
                    if (intakeArmPosLeft > waitParameter) {
                        finishedWait = true;
                        cout << "Intake arm left above done\n";
                    }
                    break;
                    
                case INTAKEARMLEFTBELOW:
                    if (intakeArmPosLeft < waitParameter) {
                        finishedWait = true;
                        cout << "Intake arm left below done\n";
                    }
                    break;
                    
                case INTAKEARMRIGHTABOVE:
                    if (intakeArmPosRight > waitParameter) {
                        finishedWait = true;
                        cout << "Intake arm right above done\n";
                    }
                    break;
                    
                case INTAKEARMRIGHTBELOW:
                    if (intakeArmPosRight < waitParameter) {
                        finishedWait = true;
                        cout << "Intake arm right below done\n";
                    }
                    break;
                    
                case TIME:          // Time since auton start to be above time
                    if (millis() - autonStartTime > 1000 * waitParameter) {
                        finishedWait = true;
                        cout << "Wait time done\n";
                    }
                    break;
                    
                case DEPOSITDONE:          // Auto deposit stack is finished
                    if (depositStep <= -1) {
                        finishedWait = true;
                        cout << "Deposit done\n";
                    }
                    break;
                    
                case DEPLOYDONE:          // Auto deploy is finished
                    if (deployStep <= -1) {
                        finishedWait = true;
                        cout << "Deploy done\n";
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
            
            // If we've moved at least as far as we wanted to
            if (distanceToDrive < pythag(startingDrivePosX, startingDrivePosY, globalX, globalY)) {
                finishedDrive = true;
            }
            
            if (finishedDrive) {
                faceDir = -1;
                driveSpeed = 0;
                driveMode = USER;
                nextCommand = true;
            }
        }
        
        if (driveMode == DRIVE_TO) {
            bool finishedDrive = false;
            
            // If we've moved at least as far as we wanted to
            if (pythag(driveToX, driveToY, globalX, globalY) <= 1) {     // Drive within 1"
                finishedDrive = true;
            }
            
            if (finishedDrive) {
                faceDir = -1;
                driveSpeed = 0;
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
                driveDir = -1;
                driveSpeed = 0;
                driveMode = USER;
            }
            
            // If we were depositing stack
            if (depositStep != -1) {
                depositStep = -1;
            }
            // If we were deploying
            if (deployStep != -1) {
                deployStep = -1;
            }
            
            
        }
        
        delay(20);
    }
}
