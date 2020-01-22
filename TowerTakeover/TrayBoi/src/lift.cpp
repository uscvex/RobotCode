////////////////////////////////////////////////
// TRAY BOT LIFT
//

#include "main.h"

using namespace pros;

// Deploy values
#define LIFT_DEPLOY_POS 1800
#define LIFT_DEPLOY_POS_ACCEPT 1750
#define DEPLOY_DONE_POS 750

// Lift values
#define LIFT_DOWN_POS 650
#define LOW_TOWER_POS 1800
#define MID_TOWER_POS 2250
#define LIFT_SEEK_RATE 1
#define LIFT_DOWN_INCREMENT 100
#define BUTTON_HOLD_TIME 250

// Intake speeds
#define INTAKE_IN_SPEED 127
#define INTAKE_OUT_SPEED -60

// Tray values
#define TRAY_DOWN_POS 1
#define TRAY_UP_POS 700
#define TRAY_SEEK_RATE 0.5


Motor liftR(1,TORQUE,1);
Motor liftL(10,TORQUE,0);

Motor intakeR(2,TORQUE,1);
Motor intakeL(9,TORQUE,0);

Motor trayR(5,TORQUE,1);
Motor trayL(6,TORQUE,0);

// Values read/writeable by other tasks
double liftPos = 0;
double liftSeek = -1;
double trayPos = 0;
double traySeek = -1;
double runIntake = 0;
int deployStep = -1;


void runLift(void* params) {
    
    // Toggle bools
    bool justToggleIntake = false;
    bool justLiftedUp = false;
    bool justToggledTray = false;
    bool justLiftedDown = false;
    
    // Var to measure how long button held
    double lastReleasedTime = millis();
    
    while (true) {
        
        // Calculate positions
        liftPos = (liftR.get_position() + liftL.get_position()) / 2;
        trayPos = (trayL.get_position() + trayR.get_position()) / 2;
        
        // Print to screen
        pros::lcd::print(3, "Lift pos: %f", liftPos);
        pros::lcd::print(4, "Tray pos: %f", trayPos);
        
        
        // Start speeds at 0 for safety
        double liftSpeed = 0;
        double traySpeed = 0;
        double intakeSpeed = 0;
        
        // If we are asking to seek to position, perform the seek
        if (traySeek >= 0) {
            traySpeed = (traySeek - trayPos) / TRAY_SEEK_RATE;
        }
        if (liftSeek >= 0) {
            liftSpeed = (liftSeek - liftPos) / LIFT_SEEK_RATE;
        }
        
        
        switch (deployStep) {
                
            case 1:
                liftSeek = LIFT_DEPLOY_POS;
                if (liftSeek > LIFT_DEPLOY_POS_ACCEPT)
                    deployStep++;
                break;
                
            case 2:
                liftSeek = LIFT_DOWN_POS;
                if (liftSeek < DEPLOY_DONE_POS)
                    deployStep++;
                break;
                
            default:
                deployStep = -1;
                break;
        }

        
        // Semi auto controls
        
        // Toggle intake
        if (controller.get_digital(DIGITAL_B)) {
            
            if (!justToggleIntake) {
                if (runIntake != INTAKE_IN_SPEED)
                    runIntake = INTAKE_IN_SPEED;
                else
                    runIntake = 0;
            }
            
            justToggleIntake = true;
        }
        else {
            justToggleIntake = false;
        }
        if (controller.get_digital(DIGITAL_X)) {
            runIntake = INTAKE_OUT_SPEED;
        }
        
        // Toggle claw open/close
        if (controller.get_digital(DIGITAL_L1)) {
            if (!justToggledTray) {
                if (traySeek != TRAY_DOWN_POS)
                    traySeek = TRAY_DOWN_POS;
                else
                    traySeek = TRAY_UP_POS;
            }
            justToggledTray = true;
        }
        else {
            justToggledTray = false;
        }
        
        if (controller.get_digital(DIGITAL_L2))
            traySeek = TRAY_DOWN_POS;
        
        // Move lift up
        if (controller.get_digital(DIGITAL_R1)) {
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
            }
            // Otherwise move down by a set amount
            else if (!justLiftedDown) {
                if (liftSeek > LOW_TOWER_POS - 500) {
                    liftSeek = liftSeek - LIFT_DOWN_INCREMENT;
                    if (liftSeek < 0)
                        liftSeek = -1;
                }
                else {
                    liftSeek = LIFT_DOWN_POS;
                }
            }
            // Don't let us repeat until we've let go
            justLiftedDown = true;
        }
        else {
            justLiftedDown = false;
            lastReleasedTime = millis();
        }
        
        // Button to lift to claw push height
        if (controller.get_digital(DIGITAL_DOWN)) {
            liftSeek = LIFT_DOWN_POS;
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
            traySpeed = 127;
            traySeek = -1;
        }
        // Claw open
        if (controller.get_digital(DIGITAL_RIGHT)) {
            traySpeed = -127;
            traySeek = -1;
        }
        
        // Abort!
        if (controller.get_digital(DIGITAL_UP)) {
            traySeek = -1;
            liftSeek = -1;
            runIntake = 0;
        }
        
        intakeSpeed = runIntake;
        
        // Finally set motor voltages based on speed
        liftR.move_voltage(12000 * liftSpeed / 127);
        liftL.move_voltage(12000 * liftSpeed / 127);
        
        trayL.move_voltage(12000 * traySpeed / 127);
        trayR.move_voltage(12000 * traySpeed / 127);
        
        intakeR.move_voltage(12000 * intakeSpeed / 127);
        intakeL.move_voltage(12000 * intakeSpeed / 127);
        
        
        // Let other tasks have a turn
        delay(20);
    }
}

