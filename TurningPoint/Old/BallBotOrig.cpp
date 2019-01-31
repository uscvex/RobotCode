#include "BallBotConfig.h"
#include "drive.h"
#include "posmech.h"
#include "speedmech.h"
#include "BallBotAutons.cpp"

vex::competition Competition;

// Declare objects as globals
Drive drive = Drive();
SpeedMachine flyWheel = SpeedMachine();
PositionMachine arm = PositionMachine();
PositionMachine wrist = PositionMachine();
PositionMachine flipper = PositionMachine();

#define FLYWHEEL 1
#define ARM 2

#define HIGH 1
#define LOW 2

// #defines for controller buttons
#define BTN_FIRE_LOW
#define BTN_FIRE_HIGH
#define BTN_FIRE_BOTH
#define BTN_FIRE_PRESET
#define BTN_INTAKE_IN
#define BTN_INTAKE_OUT
#define BTN_INTAKE_TOGGLE
#define BTN_ABORT

#define BTN_TOGGLE_MODE

#define BTN_ARM_HIGH
#define BTN_ARM_LOW
#define BTN_ARM_DOWN
#define BTN_WRIST_VERTICAL
#define BTN_WRIST_FORWARD
#define BTN_WRIST_BACK
#define BTN_FLIP

// #defines for arm positions
#define FLIP_POS1
#define FLIP_POS2
#define WRIST_BACK_POS
#define WRIST_FORWARD_POS
#define WRIST_VERTICAL_POS
#define ARM_POS_HIGH
#define ARM_POS_LOW
#define ARM_POS_DOWN

// #defines for tuning
#define FLYWHEEL_AIM_RANGE 5    // fire ball when within x degrees of flag

// Declare and initialize any global flags we need:
// Control mode
int controlMode = FLYWHEEL;

// Auton Control
double* autonCommand = &defaultAuton[0];    // default auto routine

// For Flywheel
int autoFireState = -1;         // -1 for neutral, 1 for 'aim&spin&fire', 2 for 'spin & fire', 3 for 'fire!'
int targetFlag = 1;             // 1 for low, 2 for high, 3 for high then low

//For Intake
bool forceIntake = false;
double intakeSpeedOuter = 0;    // speed for outer intake
double intakeSpeedInner = 0;    // speed for inner intake
double flyWheelDefaultSpeed = 0;    // set speed for fixed-dist fireing
int runTillBall = 0;            // 0 = nothing, 1 = run till 1 ball in, 2 = run for two balls

// For cap mechanisms
double armSeek = -1;
double wristSeek = -1;
double flipperSeek = -1;
int stackTarget = -1;
int stackStep = -1;

// Array for flywheel speed lookup'
// Distance (tiles), low flag speed (rpm), high flag speed (rpm)
// For each distance we record flywheel speeds needed for hitting high/low flags
double[][] flywheelSpeeds = {
    {-100, 0, 0},   // to catch errors
    {0, 25, 50},
    {0.5, 100, 125},
};
int flyWheelSpeedsDefinition = 3;   // number of entries


double processEntry() {
    autonCommand++;
    return *autonCommand;
}



void setGyro(double dir) {
    // Set sensor value for gyro
    
    
}



int runArm() {
    bool justFlipped = false;
    
    while (true) {
        
        // If we want to stack something, follow the steps
        switch (stackStep) {
            case 1:
                
                break;
            case 2:
                
                break;
            default:
                stackStep = -1;
                break;
        }
        
        // Check controller inputs
        if (controlMode == ARM) {
            if (BTN_FLIP) {
                stackStep = -1;
                if (!justFlipped) {
                    if (flipper.getPosition() > (FLIP_POS1 + FLIP_POS2)/2) {
                        flipperSeek = FLIP_POS1;
                    }
                    else {
                        flipperSeek = FLIP_POS2;
                    }
                }
                justFlipped = true;
            }
            else {
                justFlipped = false;
            }
            if (BTN_WRIST_BACK) {
                stackStep = -1;
                wristSeek = WRIST_BACK_POS;
            }
            if (BTN_WRIST_FORWARD) {
                stackStep = -1;
                wristSeek = WRIST_FORWARD_POS;
            }
            if (BTN_WRIST_VERTICAL) {
                stackStep = -1;
                wristSeek = WRIST_VERTICAL_POS;
            }
            if (BTN_ARM_HIGH) {
                stackStep = -1;
                armSeek = ARM_POS_HIGH;
            }
            if (BTN_ARM_HIGH) {
                stackStep = -1;
                armSeek = ARM_POS_LOW;
            }
            if (BTN_ARM_DOWN) {
                stackStep = -1;
                armSeek = ARM_POS_DOWN;
            }
        }
        if (BTN_ABORT) {
            wristSeek = -1;
            armSeek = -1;
            flipperSeek = -1;
            stackStep = -1;
        }
        
        // If we need to seek, then tell the arm, wrist, and flipper
        if (armSeek > 0) {
            arm.hold(armSeek);
        }
        else {
            arm.stop();
        }
        if (wristSeek > 0) {
            wrist.hold(wristSeek);
        }
        else {
            wrist.stop();
        }
        if (flipperSeek > 0) {
            flipper.hold(flipperSeek);
        }
        else {
            flipper.stop();
        }
        
        arm.run();
        wrist.run();
        flipper.run();
        
        vex::task::sleep(20);   // don't hog cpu
    }
}


int runFlywheel() {
    
    // Declare any local variables
    bool ballIsIn = false;
    bool ballWasIn = false;
    bool justToggledAutoBall = false;
    
    while (true) {
        
        // Set intake motor speeds to 0
        if (!forceIntake) {
            intakeSpeedInner = 0;
            intakeSpeedOuter = 0;
        }

        // keep flywheel at default speed
        flywheel.runAtSpeed(flyWheelDefaultSpeed);
        
        if (autoFireState != -1) {  // Auto fire
            
            // Check vision sensor to determine necessary turn
            double relativeAngle = 0;
            if (autoFireState <= 1) {
                // Read sensor and find relative angle
                
                
            }
            
            // Check lidar / ultrasonic for distance
            double distance = 0;
            if (autoFireState <= 2) {
                // Read sensor and find distance
                
                
            }
            
            // Lookup distance in flywheelSpeeds table, & interpolate to find speed
            double targetSpeed = flyWheelDefaultSpeed;
            if (autoFireState <= 2) {
                int index = -1;
                for (int i = 1; i < flyWheelSpeedsDefinition; i++) {
                    // Look for speed too high
                    if (flyWheelSpeeds[i][0] >= distance) {
                        index = i;
                        break;
                    }
                }
                if (index == -1) {
                    // Further than furthest in table
                    if (targetFlag == 1) targetSpeed = flyWheelSpeeds[i][1];
                    else targetSpeed = flyWheelSpeeds[i][2];
                }
                else {
                    // Interpolate for correct speed
                    // find how similar distance is to each value
                    double distDiff = (distance - flyWheelSpeeds[index-1][0]);
                    distDiff /= (flyWheelSpeeds[index][0] - flyWheelSpeeds[index-1][0]);
                    double speedDiff;
                    // Find how similar speed should be to each value & set target speed
                    if (targetFlag == 1) {
                        speedDiff = (flyWheelSpeeds[index][1] - flyWheelSpeeds[index-1][1]);
                        targetSpeed = speedDiff*distDiff + flyWheelSpeeds[index-1][1];
                    }
                    else {
                        speedDiff = (flyWheelSpeeds[index][2] - flyWheelSpeeds[index-1][2]);
                        targetSpeed = speedDiff*distDiff + flyWheelSpeeds[index-1][2];
                    }
                }
            }
            
            // Read flags for & set flywheel object target speed
            flyWheel.runAtSpeed(targetSpeed);
            
            // Read vision sensor & ask drive to turn appropriately
            drive.turnRelative(relativeAngle);
            
            bool fireBall = false;
            // Check current speed of flywheel & if aimed
            if ((flywheel.isWithinRange() && abs(relativeAngle) < FLYWHEEL_AIM_RANGE)) {
                // Set flag for fireing ball
                fireBall = true;
            }
            
            // Check if ball is in ready position
            ballIsIn = false;
            // read sensors to check if ball is in
            
            
            
            if (!ballIsIn && !ballWasIn) {    // move ball into position if not already
                intakeSpeedOuter = 100;
                intakeSpeedInner = 25;
            }
            
            if (fireBall) {         // aimed and running correct speed
                // Run intake motor
                intakeSpeedInner = 100;
                
                if (ballWasIn && !ballIsIn) {   // ball has left
                    // Clear flags
                    fireBall = false;
                    if (targetFlag == 3) {  // wanted to shoot high then low
                        targetFlag = 1;
                    }
                    else {
                        autoFireState = -1;
                    }
                }
            }
            
            
        }   // end of auto-fire
        
        
        
        // Check controller buttons...
        // Set flags for preset flywheel speeds & auto-aim-fire
        // If manual intake buttons pressed, override intake speeds
        if (controlMode == FLYWHEEL) {
            if (BTN_FIRE_LOW) { // auto fire low
                autoFireState = 1;
                targetFlag = 1;
            }
            if (BTN_FIRE_HIGH) { // auto fire high
                autoFireState = 1;
                targetFlag = 2;
            }
            if (BTN_FIRE_BOTH) { // auto fire both
                autoFireState = 1;
                targetFlag = 3;
            }
            if (BTN_FIRE_PRESET) { // auto fire preset
                autoFireState = 3;
            }
            
            if (BTN_INTAKE_IN) { // manual run intake in
                intakeSpeedInner = 25;
                intakeSpeedOuter = 100;
                runTillBall = 0;
                forceIntake = false;
            }
            if (BTN_INTAKE_OUT) { // manual run intake out
                intakeSpeedInner = -25;
                intakeSpeedOuter = -100;
                runTillBall = 0;
                forceIntake = false;
            }
            if (BTN_INTAKE_TOGGLE) { // toggle auto ball intake
                if (!justToggledAutoBall) {
                    if (runTillBall) runTillBall = 0; else runTillBall = 2;
                }
                justToggledAutoBall = true;
            }
            else {
                justToggledAutoBall = false;
            }
        }
        if (BTN_ABORT) {     // cancel auto functions
            autoFireState = -1;
            runTillBall = 0;
            forceIntake = false;
        }
        
        if (runTillBall) {
            if (/*read inner sensor*/) {     // ball is not all the way in
                intakeSpeedOuter = 100;
                intakeSpeedInner = 25;
            }
            else if (/*read outer sensor*/ && (runTillBall == 2)) {   // 1 ball is in, but not 2
                intakeSpeedOuter = 100;
            }
        }
        
        
        // Send speeds to intake motors
        intakeInner.spin(intakeSpeedInner);
        intakeOuter.spin(intakeSpeedOuter);
        
        // Run the flywheel
        flyWheel.run();
        
        // Remember ball info for fireing
        ballWasIn = ballIsIn;
        
        
        vex::task::sleep(20);   // don't hog cpu
    }
    return 0;
}


void autonomous( void ) {
    // Start flywheel task
    
    
    bool nextCommand = true;
    int driveMode = 0;
    double pauseTime = 0;
    // Set pointer to chosen auton routine
    if (autonSelect == 1) autonCommand = &auton1[0];
    
    
    // First entry is always direction,
    setGyro(*autonCommand);
    
    
    while (true) {
        
        // Auton table decipherer - switch statement
        // Commands will set flags / call object funtions
        // Need commands for:
        // DRIVE (Time, distance, whiteline)
        // TURN (Abs & relative)
        // PAUSE (Time, till ball shot)
        // SETGYRO
        // FIRE (Auto aim, high & low)
        // INTAKE (Time / Until ball enters)
        if (nextCommand) {
            switch ((int)processEntry()) {
                case END:
                    // Do nothing
                    break;
                case PAUSE:
                    pauseTime = processEntry();
                    if (pauseTime > 0) pauseTime = pauseTime * 1000 + driveBrain->timer(vex::timeUnits::msec);
                    break;
                case DRIVE:
                    double ds = processEntry();
                    double dd = processEntry();
                    double dt = processEntry();
                    if (dt < 0) {
                        if (dt == DISTANCE) {
                            driveMode = dt;
                            drive.driveDist(ds,dd,processEntry(),processEntry());
                        }
                        else {
                            driveMode = dt;
                            drive.driveCustom(ds,dd,processEntry());
                        }
                    }
                    else {
                        driveMode = 0;
                        drive.driveTime(ds,dd,dt);
                    }
                    break;
                case TURN:
                    drive.turnTo(processEntry(),processEntry());
                    break;
                case TURN_REL:
                    drive.turnRelative(processEntry(),processEntry());
                    break;
                case TURN_ENC:
                    drive.turnRelativeEncoder(processEntry(),processEntry());
                    break;
                case FIRE_PRESET:
                    autoFireState = 3;
                    nextCommand = true;
                    break;
                case FIRE_AIM:
                    autoFireState = 1;
                    targetFlag = processEntry();
                    nextCommand = true;
                    break;
                case INTAKE_ON:
                    runTillBall = 2;
                    nextCommand = true;
                    break;
                case INTAKE_OFF:
                    runTillBall = 0;
                    nextCommand = true;
                    break;
                case ARMSEEK:
                    armSeek = processEntry();
                    nextCommand = true;
                    break
                case WRISTSEEK:
                    wristSeek = processEntry();
                    nextCommand = true;
                    break;
                case FLIPPERSEEK:
                    flipperSeek = processEntry();
                    nextCommand = true;
                    break;
                case FLIP:
                    if (flipper.getPosition() > (FLIP_POS1 + FLIP_POS2)/2) {
                        flipperSeek = FLIP_POS1;
                    }
                    else {
                        flipperSeek = FLIP_POS2;
                    }
                    nextCommand = true;
                    break;
                case STACK_LOW:
                    stackTarget = LOW;
                    stackStep = 1;
                    break;
                case STACK_HIGH:
                    stackTarget = HIGH;
                    stackStep = 1;
                    break;
                case STACK_LOW_FROM:
                    stackTarget = LOW;
                    stackStep = processEntry();
                    break;
                case STACK_HIGH_FROM:
                    stackTarget = HIGH;
                    stackStep = processEntry();
                    break;
                default:
                    break;
            }
        }
        
        
        
        // Read any sensors and set global variables
        // eg. gyroDirection, etc.
        
        
        
        // Auton command termination code
        // Decide if we should move to the next command
        // eg. checking timers for pause, flags for shooting balls, etc.
        bool terminateDrive = false;
        if (driveMode == WHITELINE) {
            if (/* read white line sensor */) {
                terminateDrive = true;
            }
        }
        
        if (stackTarget != 0 && stackStep == -1) {
            stackTarget = 0;
            nextCommand = true;
        }
        
        if (pauseTime > 0) {
            if (driveBrain->timer(vex::timeUnits::msec) > pauseTime) {
                pauseTime = 0;
                nextCommand = true;
            }
        }
        else {
            if (pauseTime == FIRED && autoFireState == -1) {
                nextCommand = true;
            }
            if (pauseTime == GOTBALL)
        }
        
        if (terminateDrive) {
            driveMode = 0;
            drive.finishMove();
            nextCommand = true;
        }
        
        drive.setDirection(gyroDirection);
        drive.run();
        
        vex::task::sleep(20);   // let other tasks use cpu
    }
}

void usercontrol( void ) {
    // Start flywheel task
    while (true) {
        
        // Not much to do here
        
        // Read any sensors and set global variables
        // eg. gyroDirection, etc.
        
        
        // drive.setDirection(gyroDirection);
        drive.run();
        vex::task::sleep(20); //Sleep the task for a short amount of time to prevent wasted resources.
    }
}

// main() runs competition/field interaction code... DO NOT CHANGE!!
int main() {
    
    //Run the pre-autonomous function.
    pre_auton();
    
    //Set up callbacks for autonomous and driver control periods.
    Competition.autonomous( autonomous );
    Competition.drivercontrol( usercontrol );
    
    //Prevent main from exiting with an infinite loop.
    while (true) {
        vex::task::sleep(100);//Sleep the task for a short amount of time to prevent wasted resources.
    }
    
}
