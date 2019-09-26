#include "main.h"

// Motor declarations
// Clamp winch
Motor mClamp(16,SPEED,1);

// Lift
Motor mLiftR(15,TORQUE,1);
Motor mLiftL(14,TORQUE,0);

// Inner intake
Motor mInIntakeR(11,SPEED,0);
Motor mInIntakeL(12,SPEED,1);



// Vars for auton control of mechanisms
double liftSeek = -1;
double clampSeek = -1;
double runIntake = 0;


// Task to run lift
void runLift(void* params) {
    
    bool justToggledLift = false;
    bool justToggledClamp = false;
    bool justToggledIntake = false;
    
    while (true) {
        // Vars to hold speed values
        double liftSpeed = 0;
        double clampSpeed = 0;
        double inIntakeSpeed = 0;
        
        // Calculate mechanism positions
        double liftPos = (mLiftL.get_position() + mLiftR.get_position())/2;
        double clampPos = mClamp.get_position();
        
        // Do seek movements if we need to
        if (liftSeek != -1) {
            liftSpeed = (liftSeek - liftPos) * LIFT_SEEK_RATE;
        }
        if (clampSeek != -1) {
            clampSpeed = (clampSeek - clampPos) * CLAMP_SEEK_RATE;
        }
        
        
        // Auto functions
        // Toggle lift
        if (controller.get_digital(DIGITAL_R1)) {
            if (!justToggledLift) {
                if (liftSeek == LIFT_HOLD_POS)
                    liftSeek = LIFT_DEPOSIT_POS;
                else
                    liftSeek = LIFT_HOLD_POS;
            }
            justToggledLift = true;
        }
        else {
            justToggledLift = false;
        }
        
        // Toggle clamp
        if (controller.get_digital(DIGITAL_R2)) {
            if (!justToggledClamp) {
                if (clampSeek == CLAMP_RELEASE_POS)
                    clampSeek = CLAMP_HOLD_POS;
                else
                    clampSeek = CLAMP_RELEASE_POS;
            }
            justToggledClamp = true;
        }
        else {
            justToggledClamp = false;
        }
        
        // Toggle intake
        if (controller.get_digital(DIGITAL_L1)) {
            if (!justToggledIntake) {
                if (runIntake == 127)
                    runIntake = 0;
                else
                    runIntake = 127;
            }
            justToggledIntake = true;
        }
        else {
            justToggledIntake = false;
        }
        
        inIntakeSpeed = runIntake;
        
        // Check manual overrides
        if (controller.get_digital(DIGITAL_UP)) {
            clampSeek = -1;
            liftSeek = -1;
            runIntake = 0;
        }
        if (controller.get_digital(DIGITAL_Y)) {
            liftSeek = -1;
            liftSpeed = -127;
        }
        if (controller.get_digital(DIGITAL_A)) {
            liftSeek = -1;
            liftSpeed = 127;
        }
        if (controller.get_digital(DIGITAL_LEFT)) {
            clampSeek = -1;
            clampSpeed = -127;
        }
        if (controller.get_digital(DIGITAL_RIGHT)) {
            clampSeek = -1;
            clampSpeed = 127;
        }
        if (controller.get_digital(DIGITAL_L2)) {
            inIntakeSpeed = -30;
            runIntake = 0;
        }
        
        
        // Now send values to motors
        mLiftL.move_voltage(12000 * liftSpeed / 127);
        mLiftR.move_voltage(12000 * liftSpeed / 127);
        mClamp.move_voltage(12000 * clampSpeed / 127);
        mInIntakeL.move_voltage(12000 * inIntakeSpeed / 127);
        mInIntakeR.move_voltage(12000 * inIntakeSpeed / 127);
        
        
        delay(20);
    }
}
