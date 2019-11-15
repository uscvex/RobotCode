#include "main.h"

using namespace pros;

#define LOW_TOWER_POS 1800
#define MID_TOWER_POS 2250
#define HIGH_TOWER_POS 3600
#define LIFT_CUBE_PUSH_POS 650


#define LIFT_DOWN_POS 1

#define LIFT_SEEK_RATE 1

#define LIFT_DOWN_INCREMENT 50


#define CLAW_OPEN_POS 1
#define CLAW_CLOSE_POS 180
#define CLAW_SEEK_RATE 0.5

#define BUTTON_HOLD_TIME 250

Motor liftR(5,TORQUE,1);
Motor liftL(6,TORQUE,0);

Motor claw(16,SPEED,1);


double liftPos = 0;
double clawPos = 0;
double clawSeek = 0;
double liftSeek = 0;


void runLift(void* params) {
    
    // Toggle bools
    bool justClawed = false;
    bool justLiftedUp = false;
    bool justLiftedDown = false;
    
    // Var to measure how long button held
    double lastReleasedTime = millis();
    
    while (true) {
        
        // Calculate positions
        liftPos = (liftR.get_position() + liftL.get_position()) / 2;
        clawPos = claw.get_position();
        
        // Print to screen
        pros::lcd::print(3, "Lift pos: %f", liftPos);
        pros::lcd::print(4, "Claw pos: %f", clawPos);
        
        
        // Start speeds at 0 for safety
        double liftSpeed = 0;
        double clawSpeed = 0;
        
        // If we are asking to seek to position, perform the seek
        if (clawSeek >= 0) {
            clawSpeed = (clawSeek - clawPos) / CLAW_SEEK_RATE;
        }
        if (liftSeek >= 0) {
            liftSpeed = (liftSeek - liftPos) / LIFT_SEEK_RATE;
        }
        

        
        // Semi auto controls
        
        // Toggle claw open/close
        if (controller.get_digital(DIGITAL_L1)) {
            if (!justClawed) {
                if (clawSeek != CLAW_OPEN_POS)
                    clawSeek = CLAW_OPEN_POS;
                else
                    clawSeek = CLAW_CLOSE_POS;
            }
            justClawed = true;
        }
        else {
            justClawed = false;
        }
        
        if (controller.get_digital(DIGITAL_L2))
            clawSeek = CLAW_OPEN_POS;
        
        // Move lift up
        if (controller.get_digital(DIGITAL_R1)) {
            // If this is a new press
            if (!justLiftedUp) {
                // Then move lift up to next position
                if (liftSeek < LOW_TOWER_POS && liftPos < LOW_TOWER_POS)
                    liftSeek = LOW_TOWER_POS;
                else if (liftSeek < MID_TOWER_POS && liftPos < MID_TOWER_POS)
                    liftSeek = MID_TOWER_POS;
                else if (liftSeek < HIGH_TOWER_POS)
                    liftSeek = HIGH_TOWER_POS;
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
                liftSeek = liftSeek - LIFT_DOWN_INCREMENT;
                if (liftSeek < 0)
                    liftSeek = -1;
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
            liftSeek = LIFT_CUBE_PUSH_POS;
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
            clawSpeed = 127;
            clawSeek = -1;
        }
        // Claw open
        if (controller.get_digital(DIGITAL_RIGHT)) {
            clawSpeed = -127;
            clawSeek = -1;
        }
        
        // Abort!
        if (controller.get_digital(DIGITAL_UP)) {
            clawSeek = -1;
            liftSeek = -1;
        }
        
        
        // Finally set motor voltages based on speed
        liftR.move_voltage(12000 * liftSpeed / 127);
        liftL.move_voltage(12000 * liftSpeed / 127);
        
        claw.move_voltage(12000 * clawSpeed / 127);
        
        // Let other tasks have a turn
        delay(20);
    }
}

