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

double redAuton[] = {
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
    PAUSE,0.5,                         // PAUSE FOR LAST CUBE
    DRIVETO,70,270,-36,-45,4,           // DRIVE BACK FROM ROW
    
    DRIVETO,70,270,-36,-37,3,            // DRIVE TO LINE UP FOR TOWER CUBE
    DRIVETO,60,270,-26,-37,3,            // DRIVE TO GET CUBE
    PAUSE,0.5,
    
    
    DRIVETO,80,270,-59,-37,3,            // DRIVE BACK TO WALL
    DRIVETO,80,0,-49,-37,3,                 // DRIVE TO LINE UP FOR CUBE
    DRIVETO,50,0,-49,-25,3,                 // DRIVE TO GET CUBE
    PAUSE,0.5,
    DRIVETO,70,0,-49,-31,3,                 // DRIVE BACK FROM CUBE
    DRIVETO,70,0,-58,-31,3,
    
    DRIVETO,70,270,-59,-21,3,            // DRIVE TO LINE UP FOR 3

    DRIVETO,40,270,-24,-21,3,            // DRIVE TO GET 3                      ROW OF THREE
    
    DRIVETO,70,270,-29,-22,3,             // DRIVE TO LINE UP FOR CUBE
    TURN,0,1,
    DRIVETO,50,0,-29,-14,3,             // DRIVE TO GET CUBE
    DRIVETO,70,0,-29,-20,3,             // DRIVE BACK
    DRIVETO,70,0,-22,-20,2,             // DRIVE TO LINE UP FOR NEXT
    DRIVETO,50,0,-21,30,5,             // DRIVE TO GET NEXT CUBE
    
    DRIVETO,90,0,-22,-30,5,             // DRIVE TO GET NEXT CUBE
    
    INTAKEPOS,INTAKE_ARM_IN_POS,       // PUT ARMS IN
    
    DRIVETO,90,135,-54,-64,3,             // DRIVE TO CORNER
//    LIFTPOS,9000,
//    WAIT,LIFTABOVE,8000,5,
//    DRIVEDIST,50,315,135,10,2,

    DEPOSIT,                            // DROP OFF STACK
    WAIT,DEPOSITDONE,13,                // WAIT FOR DEPOSIT DONE
    
    END
};


double blueAuton[] = {
    60,-21,90,                          // START POS (X,Y,THETA)
    
    DEPLOY,                             // DEPLOY
    INTAKESPEED,-30,                    // RUN INTAKE OUT
    WAIT,DEPLOYDONE,15,                 // WAIT TILL DEPLOYED
    INTAKESPEED,-30,                    // RUN INTAKE OUT
    WAIT,LIFTBELOW,12000,5,             // WAIT UNTIL LIFT DOWN
    PAUSE,1,                            // WAIT A BIT LONGER
    INTAKEPOS,INTAKE_ARM_OUT_POS,       // PUT ARMS OUT
    WAIT,INTAKEARMLEFTABOVE,600,1,      // WAIT UNTIL THEY ARE OUT
    DRIVETO,120,90,60,-21,0.5,          // DRIVE BACK TO START POS
    INTAKESPEED,127,                    // RUN INTAKE
    
    DRIVETO,60,90,16,-21,5,             // DRIVE TO GET FIRST ROW               ROW OF THREE
    PAUSE,0.75,                         // PAUSE FOR LAST CUBE
    
    DRIVETO,80,0,29,-23,3,              // DRIVE TO LINE UP FOR PURPLE
    DRIVETO,60,0,29,-13,3,              // DRIVE TO GET PURPLE                  PURPLE CUBE
    PAUSE,0.5,
    DRIVETO,80,0,29,-18,3,              // DRIVE AWAY FROM PURPLE
    
    DRIVETO,80,0,28,-18,3,              // DRIVE TO LINE UP FOR GREEN
    DRIVETO,60,0,24,-8,3,              // DRIVE TO GET GREEN                   GREEN CUBE
    PAUSE,0.5,
    
    DRIVETO,100,25,28,-15,3,            // DRIVE AWAY FROM GREEN        WAS 45
    DRIVETO,60,45,22,-11,3,             // DRIVE TO GET ORANGE                  ORANGE CUBE
    PAUSE,0.5,
    DRIVETO,120,45,24,-20,3,            // DRIVE AWAY FROM ORANGE
    
    DRIVETO,120,90,54,-25,3,            // DRIVE TO LINE UP FOR PURPLE      Y=-32
    TURN,345,1,                         // TURN TO FACE PURPLE
    DRIVETO,60,15,54,-22,3,             // DRIVE TO GET PURPLE                  PURPLE CUBE
    PAUSE,0.5,
    DRIVETO,120,15,56,-28,3,            // DRIVE AWAY FROM PURPLE   Y=-32
    
    INTAKEPOS,INTAKE_ARM_IN_POS,        // MOVE INTAKE ARMS IN
    DRIVETO,80,90,58,-43, 3,            // DRIVE TO LINE UP FOR ROW OF FOUR     Y=-45
    
    INTAKEPOS,INTAKE_ARM_OUT_POS,       // PUT INTAKE ARMS OUT
    DRIVETO,80,90,60,-43,1,             // DRIVE TO LINE UP FOR ROW OF FOUR     Y=-45
    WAIT,INTAKEARMLEFTABOVE,600,1,      // PUT ARMS OUT
    PAUSE,0.25,
    DRIVETO,40,90,32,-43,6,             // DRIVE GET ROW OF FOUR                ROW OF FOUR Y=-45
    DRIVETO,40,70,22,-37,6,             // DRIVE GET ORANGE                     ORANGE CUBE Y=-40
    
    
    DRIVETO,60,70,26,-40,3,             // DRIVE BACK FROM ORANGE   Y=-45
    INTAKEPOS,INTAKE_ARM_IN_POS,        // MOVE INTAKE ARMS IN
    DRIVETO,90,225,55,-48,4,            // DRIVE TO CORNER  Y=-55
    DRIVETO,90,225,65,-52,1,            // DRIVE TO CORNER  Y=-60
    
    DEPOSIT,                            // DROP OFF STACK
    WAIT,DEPOSITDONE,13,                // WAIT FOR DEPOSIT DONE
    
    END
};

double programmingSkills[] = {
    -60,-45,270,

    END,    // REMOVE THIS, YA DINGUS
    
    DEPLOY,
    WAIT,DEPLOYDONE,15,
    WAIT,LIFTBELOW,12000,5,
    PAUSE,1,
    INTAKESPEED,-60,                    // RUN INTAKE OUT
    //DRIVETO,60,270,-60,-45,1,           // DRIVE BACK TO START AFTER DEPLOY

    INTAKEPOS,INTAKE_ARM_OUT_POS,       // For safety
    WAIT,INTAKEARMLEFTABOVE,600,1,
    INTAKESPEED,127,                    // RUN INTAKE

    DRIVETO,60,270,-38,-45,9,           // ROW OF FOUR

    DRIVETO,40,270,-28,-58,3,           // DRIVE READY FOR SQUARE
    PAUSE,0.25,
    DRIVETO,80,270,0,-60,3,             // TURN TO GET FOUR IN A SQUARE
    DRIVETO,40,270,-12,-60,1,           // DRIVE BACK

    DRIVETO,40,290,12,-50,4,            // DRIVE TO START OF ROW
    DRIVETO,40,270,44,-50,9,            // DRIVE THROUGH REST OF ROW
    PAUSE,0.25,

    
    TURN,180,0.5,
    TURN,135,0.5,
    //TURN,225,0.5,
    
    INTAKEPOS,INTAKE_ARM_IN_POS,        // PULL ARMS IN
    DRIVETO,70,225,60,-60,3,            // DRIVE TO CORNER
    
    DRIVE,127,180,225,0.75,               // DRIVE SOUTH HARD
    //DRIVE,127,270,225,0.75,               // DRIVE EAST HARD
    
    //DRIVE,60,45,225,0.05,                // BREAK

    SETPOS,54,-54,                      // SET POSITION

    //END,    //////////////////////////////////////////////////////////////////////////////////// END

    DEPOSIT,                            // DROP OFF STACK
    WAIT,DEPOSITDONE,13,
    
//    54,-54,225,                         /////////////////////// REMOVE WHEN DEPOSIT
//    DRIVEDIST,40,45,225,20,2,           /////////////////////// REMOVE WHEN DEPOSIT
    
    //DRIVETO,60,225,55,-36,3,            // Drive away from stack
    
    
    DRIVETO,90,90,54,-18,3,             // LINE UP FOR 3 ROW
    
    INTAKEPOS,INTAKE_ARM_OUT_POS,       // PUT ARMS OUT
    INTAKESPEED,90,                    // RUN INTAKE SLOW
    WAIT,INTAKEARMLEFTABOVE,600,1,
    
    INTAKESPEED,127,                    // RUN INTAKE FAST
    PAUSE,0.5,
    DRIVETO,80,90,12,-16,6,             // GET 1ST 3 ROW
    
    
    DRIVETO,80,90,0,-14,6,             // LINE UP FOR NEXT ROW

    
    DRIVETO,80,90,-50,-15,6,             // GET 2ND 3 ROW
    
    //TURN,135,1,                         // TURN TO FACE SOUTH
    DRIVETO,80,135,-50,-46,5,           // DRIVE NEAR ZONE
   
    PAUSE,0.5,
    
    DRIVEDIST,80,315,135,10,2,
    DRIVEDIST,80,135,135,20,2,
    DRIVEDIST,80,315,135,15,2,
    INTAKEPOS,INTAKE_ARM_IN_POS,        // PUT ARMS IN
    PAUSE,0.5,
    DRIVEDIST,80,135,135,10,2,
    
    //TURN,90,1,                          // SWEEP CUBE
    
    //TURN,135,1,                         // AIM FOR ZONE
    DRIVEDIST,60,135,135,10,2,
    //DRIVE,60,90,135,0.5,
    
    DRIVE,127,180,135,0.75,              // DRIVE SOUTH HARD
    DRIVE,127,90,135,0.75,               // DRIVE WEST HARD
    
    DEPOSIT,                            // Drop off stack
    WAIT,DEPOSITDONE,13,
    
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
