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

// Auton commands
#define TURN 1          // TURN, <FACEDIR>, <TIMEOUT>
#define DRIVE 2         // DRIVE, <DRIVESPEED>, <FACEDIR>, <TIMEOUT>
#define DRIVEDIST 3     // DRIVE, <DRIVESPEED>, <FACEDIR>, <DISTANCE>, <TIMEOUT>
#define LIFTPOS 4       // LIFTPOS, <POSITION>
#define CLAWPOS 5       // CLASPOS, <POSITION>
#define END 7           // END
#define PAUSE 8         // PAUSE, <TIMEOUT>
#define WAIT 9          // WAIT, <CONDITION>, <PARAMETER>, <TIMEOUT>


// Wait conditions
#define LIFTBELOW 1
#define LIFTABOVE 2
#define TIME 3

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
    DRIVEDIST,127,270,48,5,
    DRIVE,-60,270,0.05,
    
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
