////////////////////////////////////////////////
// TRAY BOT LIFT
//

#include "main.h"

using namespace pros;

// Deploy values
#define LIFT_DEPLOY_POS 3200
#define LIFT_DEPLOY_POS_ACCEPT 3300
#define DEPLOY_DONE_POS 750

// Lift values
#define LIFT_DOWN_POS 300
#define LOW_TOWER_POS 2750
#define MID_TOWER_POS 3400
#define LIFT_SEEK_RATE 1
#define LIFT_DOWN_INCREMENT 100
#define BUTTON_HOLD_TIME 250

#define LIFT_DEPOSIT_POS -1

// Intake speeds
#define INTAKE_IN_SPEED 127
#define INTAKE_OUT_SPEED -127

// Tray values
#define TRAY_DOWN_POS 1
#define TRAY_UP_POS 1800
#define TRAY_SEEK_RATE 30
#define TRAY_SLOW_POS 500
#define TRAY_SLOW_SPEED 30
#define TRAY_ARM_POS 600

// Lock values
#define LOCK_OPEN_POS 1
#define LOCK_CLOSE_POS 250
#define LOCK_SEEK_RATE 1


Motor liftR(1,TORQUE,1);
Motor liftL(10,TORQUE,0);

Motor intakeR(2,TORQUE,0);
Motor intakeL(9,TORQUE,1);

Motor trayR(5,TORQUE,0);
Motor trayL(6,TORQUE,1);

Motor cubeLock(19,SPEED,1);


// Values read/writeable by other tasks
double liftPos = 0;
double liftSeek = -1;
double trayPos = 0;
double traySeek = -1;
double runIntake = 0;
double lockPos = 0;
double lockSeek = -1;
int deployStep = -1;
int depositStep = -1;
bool doneDeploy = false;

void runLift(void* params) {
    
    // Toggle bools
    bool justToggleIntake = false;
    bool justLiftedUp = false;
    bool justToggledTray = false;
    bool justLiftedDown = false;
    double currTime = 0;
    bool justDeploy = false;
    bool justLockToggle = false;
    
    // Var to measure how long button held
    double lastReleasedTime = millis();
    
    while (true) {
        
        // Calculate positions
        liftPos = (liftR.get_position() + liftL.get_position()) / 2;
        trayPos = (trayL.get_position() + trayR.get_position()) / 2;
        lockPos = cubeLock.get_position();
        
        // Print to screen
        pros::lcd::print(6, "Lift pos: %f", liftPos);
        pros::lcd::print(7, "Tray pos: %f", trayPos);
        pros::lcd::print(8, "Lock pos: %f", lockPos);
        
        
        // Start speeds at 0 for safety
        double liftSpeed = 0;
        double traySpeed = 0;
        double lockSpeed = 0;
        double intakeSpeed = runIntake;
        
        // If we are asking to seek to position, perform the seek
        if (traySeek >= 0) {
            traySpeed = (traySeek - trayPos) / TRAY_SEEK_RATE;
            if (traySeek == 1) {
                traySpeed = (traySeek - trayPos);
            }
        }
        if (liftSeek >= 0) {
            liftSpeed = (liftSeek - liftPos) / LIFT_SEEK_RATE;
        }
        if (lockSeek >= 0) {
            lockSpeed = (lockSeek - lockPos) / LOCK_SEEK_RATE;
        }
        
        
        // Auto deploy
        switch (deployStep) {
                
            case 1:         // Wait until release
                if (!controller.get_digital(DIGITAL_B))
                    deployStep++;
                break;
                
            case 2:         // Move arms up to push open tray
                liftSeek = -1;
                traySeek = -1;
                liftSpeed = 127;
                currTime = millis();
                if (liftPos > LIFT_DEPLOY_POS)
                    deployStep++;
                break;
                
            case 3:
                //runIntake = -127;
                liftSpeed = 127;
                if (millis() - currTime >= 500) {
                    deployStep++;
                    currTime = millis();
                }
                break;
                
            case 4:
                //runIntake = -127;
                liftSpeed = 0;
                if (millis() - currTime >= 250)
                    deployStep++;
                break;
                
            case 5:
                liftSpeed = -127;
                //runIntake = -127;
                if (liftPos < DEPLOY_DONE_POS)
                    deployStep++;
                break;
                
            case 6:
                liftSpeed = 0;
                //runIntake = 0;
                traySpeed = 127;
                if (trayPos > 1500)
                    deployStep++;
                break;
                
            case 7:
                traySeek = 1;
                intakeSpeed = 0;
                liftSeek = LIFT_DOWN_POS;
                if (trayPos < 500)
                    deployStep++;
                break;
                
            case 8:         // Deploy is done
                intakeSpeed = 0;
                traySeek = TRAY_DOWN_POS;
                deployStep++;
                doneDeploy = true;
                break;
                
            default:
                deployStep = -1;
                break;
        }

        // Auto deposit
        switch (depositStep) {
                
            case 1:         // Wait until release
                if (!controller.get_digital(DIGITAL_B))
                    depositStep++;
                break;
                
            case 2:
                traySeek = -1;
                liftSeek = LIFT_DEPOSIT_POS;
                intakeSpeed = 0;
                currTime = millis();
                
                if (trayPos > TRAY_SLOW_POS) {
                    traySeek = TRAY_UP_POS+200;
                    intakeSpeed = -30;
                }
                else {
                    traySpeed = 127;
                }
                
                if (trayPos > TRAY_UP_POS)
                    depositStep++;
                
                break;
                
            case 3:
                traySpeed = TRAY_SLOW_SPEED;
                driveMode = DRIVE_TIME;   // Drive forward
                driveSpeed = 25;
                faceDir = -1;
                if (millis() - currTime > 1000)
                    depositStep++;
                break;
                
            case 4:
                traySpeed = TRAY_SLOW_SPEED;
                driveMode = DRIVE_TIME;
                if (autonSelect == PROGRAMMING_SKILLS)
                    driveSpeed = -40;      // YEET Backwards
                else
                    driveSpeed = -127;      // YEET Backwards
                faceDir = -1;//direction;
                currTime = millis();
                depositStep++;
                break;
                
            case 5:
                if (millis() - currTime > 500)
                    depositStep++;
                break;

            case 6:
                driveMode = USER;
//                traySeek = TRAY_DOWN_POS;
                depositStep++;
                break;
                
            default:
                depositStep = -1;
                break;
        }
        
        
        
        // Semi auto controls
        
        // Deposit stack
        if (controller.get_digital(DIGITAL_B)) {
            if (!justDeploy) {
                if (!doneDeploy)
                    deployStep = 1;
                else
                    depositStep = 1;
            }
            justDeploy = true;
        }
        else {
            justDeploy = false;
        }
        
//        if (controller.get_digital(DIGITAL_X))
//            traySeek = TRAY_DOWN_POS;
        
        // Move lift up
        if (controller.get_digital(DIGITAL_R1)) {
            traySeek = TRAY_ARM_POS;
            // If this is a new press
            if (!justLiftedUp) {
                // Then move lift up to next position
                if (liftSeek < LOW_TOWER_POS && liftPos < LOW_TOWER_POS)
                    liftSeek = LOW_TOWER_POS;
                else if (liftSeek < MID_TOWER_POS && liftPos < MID_TOWER_POS)
                    liftSeek = MID_TOWER_POS;
            }
            // Don't repeat until released & re-pressed
            justLiftedUp = true;
        }
        else {
            justLiftedUp = false;
        }
        
        // Move lift down
        if (controller.get_digital(DIGITAL_R2)) {
            // If we've been holding the button for time then move all the way down
            if (millis() - lastReleasedTime > BUTTON_HOLD_TIME) {
                liftSeek = -1;
                if (liftPos > LOW_TOWER_POS/2)
                    liftSpeed = -127;
                else {
                    liftSeek = LIFT_DOWN_POS;
                    traySeek = TRAY_DOWN_POS;
                }
            }\
            // Otherwise move down by a set amount
            else if (!justLiftedDown) {
                if (liftSeek > LOW_TOWER_POS - 500) {
                    liftSeek = liftSeek - LIFT_DOWN_INCREMENT;
                    if (liftSeek < 0)
                        liftSeek = -1;
                }
                else {
                    liftSeek = LIFT_DOWN_POS;
                    traySeek = TRAY_DOWN_POS;
                }
            }
            // Don't let us repeat until we've let go
            justLiftedDown = true;
        }
        else {
            justLiftedDown = false;
            lastReleasedTime = millis();
        }
        
        // Toggle cube lock
        if (controller.get_digital(DIGITAL_DOWN)) {
            if (!justLockToggle) {
                if (lockSeek != LOCK_OPEN_POS)
                    lockSeek = LOCK_OPEN_POS;
                else
                    lockSeek = LOCK_CLOSE_POS;
            }
            justLockToggle = true;
        }
        else {
            justLockToggle = false;
        }
        
        // Manual overrides
        // Lift up
        if (controller.get_digital(DIGITAL_A)) {
            liftSpeed = 127;
            liftSeek = -1;
        }
        // Lift down
        if (controller.get_digital(DIGITAL_Y)) {
            liftSpeed = -127;
            liftSeek = -1;
        }
        // Claw close
        if (controller.get_digital(DIGITAL_LEFT)) {
            traySpeed = -127;
            traySeek = -1;
            deployStep = -1;
            depositStep = -1;
        }
        // Claw open
        if (controller.get_digital(DIGITAL_RIGHT)) {
            traySpeed = 127;
            traySeek = -1;
            deployStep = -1;
            depositStep = -1;
        }
        
        // Abort!
        if (controller.get_digital(DIGITAL_UP)) {
            traySeek = -1;
            liftSeek = -1;
            runIntake = 0;
            deployStep = -1;
            lockSeek = -1;
            depositStep = -1;
        }
        
        
        // Intake in/out
        if (controller.get_digital(DIGITAL_L2)) {
            intakeSpeed = INTAKE_OUT_SPEED;
            runIntake = 0;
        }
        if (controller.get_digital(DIGITAL_L1)) {
            intakeSpeed = INTAKE_IN_SPEED;
            runIntake = 0;
        }
        
        
        // Finally set motor voltages based on speed
        liftR.move_voltage(12000 * liftSpeed / 127);
        liftL.move_voltage(12000 * liftSpeed / 127);
        
        trayL.move_voltage(12000 * traySpeed / 127);
        trayR.move_voltage(12000 * traySpeed / 127);
        
        intakeR.move_voltage(12000 * intakeSpeed / 127);
        intakeL.move_voltage(12000 * intakeSpeed / 127);
        
        cubeLock.move_voltage(12000 * lockSpeed / 127);
        
        
        // Let other tasks have a turn
        delay(20);
    }
}

