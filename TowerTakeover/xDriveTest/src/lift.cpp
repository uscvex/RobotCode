#include "main.h"

// Motor declarations

// Lift
Motor mLiftR(M_LIFT_R_P,TORQUE,0);
Motor mLiftL(M_LIFT_L_P,TORQUE,1);

// Inner intake
Motor mInIntakeR(M_IN_INTAKE_R_P,SPEED,0);
Motor mInIntakeL(M_IN_INTAKE_L_P,SPEED,1);

// Outer intake
Motor mOutIntakeR(M_OUT_INTAKE_R_P,SPEED,1);
Motor mOutIntakeL(M_OUT_INTAKE_L_P,SPEED,0);

// Intake Arm
Motor mIntakeArmR(M_INTAKE_ARM_R_P,TORQUE,1);
Motor mIntakeArmL(M_INTAKE_ARM_L_P,TORQUE,0);

// Vars for auton control of mechanisms
double liftSeek = -1;
double intakeArmSeekLeft = -1;
double intakeArmSeekRight = -1;
double runIntake = 0;


// Task to run lift
void runLift(void* params) {
    
    bool justToggledLift = false;
    bool justToggledClamp = false;
    bool justToggledIntake = false;
    bool justToggledIntakeArm = false;
    
    while (true) {
        // Vars to hold speed values
        double liftSpeed = 0;
        double inIntakeSpeed = 0;
        double outIntakeSpeed = 0;
        double intakeArmSpeedLeft = 0;
        double intakeArmSpeedRight = 0;
        
        // Calculate mechanism positions
        double liftPos = (mLiftL.get_position() + mLiftR.get_position())/2;
        double intakeArmPosLeft = mIntakeArmL.get_position();
        double intakeArmPosRight = mIntakeArmR.get_position();
        
        
        // Do seek movements if we need to
        if (liftSeek != -1) {
            liftSpeed = (liftSeek - liftPos) * LIFT_SEEK_RATE;
        }
        if (intakeArmSeekRight != -1) {
            intakeArmSpeedRight = (intakeArmSeekRight - intakeArmPosRight) * INTAKE_ARM_SEEK_RATE;
        }
        if (intakeArmSeekLeft != -1) {
            intakeArmSpeedLeft = (intakeArmSeekLeft - intakeArmPosLeft) * INTAKE_ARM_SEEK_RATE;
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
        
        // Toggle intake arms deplot/retract
        if (controller.get_digital(DIGITAL_B)) {
            if (!justToggledIntakeArm) {
                if (intakeArmSeekLeft == INTAKE_ARM_IN_POS || intakeArmSeekRight == INTAKE_ARM_IN_POS) {
                    intakeArmSeekLeft = INTAKE_ARM_OUT_POS;
                    intakeArmSeekRight = INTAKE_ARM_OUT_POS;
                }
                else {
                    intakeArmSeekLeft = INTAKE_ARM_IN_POS;
                    intakeArmSeekRight = INTAKE_ARM_IN_POS;
                }
            }
            justToggledIntakeArm = true;
        }
        else {
            justToggledIntakeArm = false;
        }
        
        // Toggle intake
        if (controller.get_digital(DIGITAL_L1)) {
            if (!justToggledIntake) {
                if (runIntake == INTAKE_IN_SPEED)
                    runIntake = 0;
                else
                    runIntake = INTAKE_IN_SPEED;
            }
            justToggledIntake = true;
        }
        else {
            justToggledIntake = false;
        }
        
        // Set intake motor speed based on toggle/auton
        inIntakeSpeed = runIntake;
        outIntakeSpeed = runIntake;
        
        // Check manual overrides
        
        // Abort!
        if (controller.get_digital(DIGITAL_UP)) {
            intakeArmSeekLeft = -1;
            intakeArmSeekRight = -1;
            liftSeek = -1;
            runIntake = 0;
        }
        
        // Manual tower lift down
        if (controller.get_digital(DIGITAL_Y)) {
            liftSeek = -1;
            liftSpeed = -127;
        }
        // Manual tower lift up
        if (controller.get_digital(DIGITAL_A)) {
            liftSeek = -1;
            liftSpeed = 127;
        }
        
        // Manual intake arm retract
        if (controller.get_digital(DIGITAL_LEFT)) {
            intakeArmSeekLeft = -1;
            intakeArmSeekRight = -1;
            intakeArmSpeedLeft = -127;
            intakeArmSpeedRight = -127;
        }
        // Manual intake arm deploy
        if (controller.get_digital(DIGITAL_RIGHT)) {
            intakeArmSeekLeft = -1;
            intakeArmSeekRight = -1;
            intakeArmSpeedLeft = 127;
            intakeArmSpeedRight = 127;
        }
        
        // Manual intake wheels out
        if (controller.get_digital(DIGITAL_L2)) {
            inIntakeSpeed = -INTAKE_OUT_SPEED;
            outIntakeSpeed = -INTAKE_OUT_SPEED;
            runIntake = 0;
        }
        
        
        // Now send values to motors
        mLiftL.move_voltage(12000 * liftSpeed / 127);
        mLiftR.move_voltage(12000 * liftSpeed / 127);
        
        mInIntakeL.move_voltage(12000 * inIntakeSpeed / 127);
        mInIntakeR.move_voltage(12000 * inIntakeSpeed / 127);
        mOutIntakeL.move_voltage(12000 * outIntakeSpeed / 127);
        mOutIntakeR.move_voltage(12000 * outIntakeSpeed / 127);
        
        mIntakeArmR.move_voltage(12000 * intakeArmSpeedRight / 127);
        mIntakeArmL.move_voltage(12000 * intakeArmSpeedLeft / 127);
        
        delay(20);
    }
}
