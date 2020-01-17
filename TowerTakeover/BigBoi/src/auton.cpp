////////////////////////////////////////////////
// BIG BOT AUTON
//

#include "main.h"

using namespace pros;
using namespace std;

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
#define SETPOS 16           // SETPOS, <X>, <Y>

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

int autonSelect = RED_AUTON;
int numAutons = 3;
double lastAutonTime = 0;

double redAuton[] = {                   // ONE STACK OF 13 CUBES
    -60,-45,270,                        // START POS (X,Y,THETA)

    DEPLOY,                             // DEPLOY
    WAIT,DEPLOYDONE,15,                 // WAIT TILL DEPLOYED
    WAIT,LIFTBELOW,12000,5,             // WAIT UNTIL LIFT DOWN
    PAUSE,1,                            // WAIT A BIT LONGER
    
    INTAKEPOS,INTAKE_ARM_OUT_POS,       // PUT ARMS OUT
    WAIT,INTAKEARMLEFTABOVE,600,1,      // WAIT UNTIL THEY ARE OUT
    DRIVETO,120,270,-60,-45,0.5,        // DRIVE BACK TO START POS
    INTAKESPEED,127,                    // RUN INTAKE
    
    DRIVETO,50,270,-28,-45,4,           // DRIVE TO GET FIRST ROW               ROW OF FOUR
    PAUSE,0.5,                          // PAUSE FOR LAST CUBE
    DRIVETO,70,270,-36,-45,4,           // DRIVE BACK FROM ROW
    
    DRIVETO,70,270,-36,-37,3,           // SLIDE TO LINE UP FOR TOWER CUBE
    DRIVETO,60,270,-26,-37,3,           // DRIVE TO GET CUBE
    PAUSE,0.5,                          // PAUSE TO INTAKE                      FIRST TOWER CUBE
    DRIVETO,80,270,-59,-37,3,           // DRIVE BACK TO WALL
    
    DRIVETO,80,0,-49,-37,3,             // DRIVE AND TURN TO LINE UP FOR CUBE
    DRIVETO,50,0,-49,-25,3,             // DRIVE TO GET CUBE                    SECOND TOWER CUBE
    PAUSE,0.5,                          // PAUSE TO INTAKE
    DRIVETO,70,0,-49,-31,3,             // DRIVE BACK FROM CUBE
    DRIVETO,70,0,-58,-31,3,             // SLIDE TO WALL TO MAKE ROOM FOR TURN
    
    DRIVETO,70,270,-59,-21,3,           // DRIVE AND TURN TO LINE UP FOR 3
    DRIVETO,40,270,-24,-21,3,           // DRIVE TO GET 3                       ROW OF THREE
    DRIVETO,70,270,-29,-22,3,           // DRIVE BACK FROM CUBE
    
    TURN,0,1,                           // TURN READY TOWER
    DRIVETO,50,0,-29,-14,3,             // DRIVE TO GET CUBE                    TOWER OF THREE
    DRIVETO,70,0,-29,-20,3,             // DRIVE BACK
    DRIVETO,70,0,-22,-20,2,             // SLIDE TO LINE UP FOR NEXT
    DRIVETO,50,0,-21,30,5,              // DRIVE IINTO PILE TO GET SOME MORE    BONUS CUBES
    
    DRIVETO,90,0,-22,-30,5,             // DIVE BACK FROM PILE
    
    INTAKEPOS,INTAKE_ARM_IN_POS,        // PUT ARMS IN
    DRIVETO,90,135,-54,-64,3,           // DRIVE TO CORNER

    DEPOSIT,                            // DROP OFF STACK
    WAIT,DEPOSITDONE,13,                // WAIT FOR DEPOSIT DONE
    
    END                                 // END OF ROUTINE
};


double blueAuton[] = {                  // ONE STACK OF 13 CUBES
    60,-45,90,                          // START POS (X,Y,THETA)
    
    DEPLOY,                             // DEPLOY
    WAIT,DEPLOYDONE,15,                 // WAIT TILL DEPLOYED
    WAIT,LIFTBELOW,12000,5,             // WAIT UNTIL LIFT DOWN
    PAUSE,1,                            // WAIT A BIT LONGER
    
    INTAKEPOS,INTAKE_ARM_OUT_POS,       // PUT ARMS OUT
    WAIT,INTAKEARMLEFTABOVE,600,1,      // WAIT UNTIL THEY ARE OUT
    DRIVETO,120,90,60,-45,0.5,          // DRIVE BACK TO START POS
    INTAKESPEED,127,                    // RUN INTAKE
    
    DRIVETO,50,90,28,-45,4,             // DRIVE TO GET FIRST ROW               ROW OF FOUR
    PAUSE,0.5,                          // PAUSE FOR LAST CUBE
    DRIVETO,70,90,36,-45,4,             // DRIVE BACK FROM ROW
    
    DRIVETO,70,90,36,-37,3,             // SLIDE TO LINE UP FOR TOWER CUBE
    DRIVETO,60,90,26,-37,3,             // DRIVE TO GET CUBE
    PAUSE,0.5,                          // PAUSE TO INTAKE                      FIRST TOWER CUBE
    DRIVETO,80,90,59,-37,3,             // DRIVE BACK TO WALL
    
    DRIVETO,80,0,49,-37,3,              // DRIVE AND TURN TO LINE UP FOR CUBE
    DRIVETO,50,0,49,-25,3,              // DRIVE TO GET CUBE                    SECOND TOWER CUBE
    PAUSE,0.5,                          // PAUSE TO INTAKE
    DRIVETO,70,0,49,-31,3,              // DRIVE BACK FROM CUBE
    DRIVETO,70,0,58,-31,3,              // SLIDE TO WALL TO MAKE ROOM FOR TURN
    
    DRIVETO,70,90,59,-21,3,             // DRIVE AND TURN TO LINE UP FOR 3
    DRIVETO,40,90,24,-21,3,             // DRIVE TO GET 3                       ROW OF THREE
    DRIVETO,70,90,29,-22,3,             // DRIVE BACK FROM CUBE
    
    TURN,0,1,                           // TURN READY TOWER
    DRIVETO,50,0,29,-14,3,              // DRIVE TO GET CUBE                    TOWER OF THREE
    DRIVETO,70,0,29,-20,3,              // DRIVE BACK
    DRIVETO,70,0,22,-20,2,              // SLIDE TO LINE UP FOR NEXT
    DRIVETO,50,0,21,30,5,               // DRIVE IINTO PILE TO GET SOME MORE    BONUS CUBES
    
    DRIVETO,90,0,22,-30,5,              // DIVE BACK FROM PILE
    
    INTAKEPOS,INTAKE_ARM_IN_POS,        // PUT ARMS IN
    DRIVETO,90,225,54,-64,3,            // DRIVE TO CORNER
    
    DEPOSIT,                            // DROP OFF STACK
    WAIT,DEPOSITDONE,13,                // WAIT FOR DEPOSIT DONE
    
    END                                 // END OF ROUTINE
};

double programmingSkills[] = {          // STACK OF 11, STACK OF 8
    -60,-45,270,                        // START POS (X,Y,THETA)
    
//    DEPLOY,                             // DEPLOY
//    WAIT,DEPLOYDONE,15,                 // WAIT TILL DEPLOYED
//    WAIT,LIFTBELOW,12000,5,             // WAIT UNTIL LIFT DOWN
//    PAUSE,1,                            // WAIT A BIT LONGER
    
    INTAKEPOS,INTAKE_ARM_OUT_POS,       // PUT ARMS OUT
    WAIT,INTAKEARMLEFTABOVE,600,1,      // WAIT UNTIL THEY ARE OUT
    DRIVETO,120,270,-60,-45,0.5,        // DRIVE BACK TO START POS
    INTAKESPEED,127,                    // RUN INTAKE
    
    DRIVETO,50,270,-28,-45,4,           // DRIVE TO GET FIRST ROW               ROW OF FOUR
    PAUSE,0.5,                          // PAUSE FOR LAST CUBE
    DRIVETO,70,270,-36,-45,4,           // DRIVE BACK FROM ROW
    
    DRIVETO,70,270,-36,-58,2,           // SLIDE TO LINE UP FOR SIDE CUBES
    DRIVETO,50,270,-10,-58,2,           // DRIVE TO GET SIDE CUBES              SQUARE OF FOUR
    PAUSE,0.5,                          // PAUSE TO INTAKE
    DRIVETO,70,270,-36,-58,2,           // DRIVE BACK
    
    END                                 // END OF ROUTINE
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
            controller.print(0,0,"T: %f", lastAutonTime);
            
            nextCommand = false;            // We are not ready for next command any more
            commandStartTime = millis();    // Record the time the command was started
            commandTimeOut = -1;            // Default to no time-out limit
            commandTimedOut = false;        // This command has not yet timed out
            
            cout << ((millis() - autonStartTime) / 1000) << ": ";
            
            // Figure out which command we are supposed to perform
            switch ((int)processEntry()) {
                    
                case SETPOS:    // Sets X, Y position of robot
                    setPosition(processEntry(),processEntry());
                    nextCommand = true;
                    break;
                    
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
                    controller.print(0,0,"E");
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
