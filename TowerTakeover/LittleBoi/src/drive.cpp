#include "main.h"

using namespace pros;

// Drive dampening parameters (anti-tip)
#define DRIVE_DAMPEN 1
#define TURN_DAMPEN 8
#define FORWARD_DAMPEN 8
#define LIFT_DAMPEN_THRESHOLD 500
#define LIFT_SLOWDOWN 0.0001

// Joystick deadzone
#define DEADZONE 15

// Drive motor declarations
Motor driveR1(1,SPEED,0);
Motor driveR2(2,SPEED,1);
Motor driveR3(3,SPEED,0);
Motor driveR4(4,SPEED,1);

Motor driveL1(10,SPEED,1);
Motor driveL2(9,SPEED,0);
Motor driveL3(8,SPEED,1);
Motor driveL4(7,SPEED,0);


// Drive task
void runDrive(void* params) {
    
    while (true) {
        
        // User input
        double forwardSpeedUser = controller.get_analog(ANALOG_LEFT_Y);
        double turnSpeedUser = controller.get_analog(ANALOG_RIGHT_X);
        
        // Deadzone
        if (forwardSpeedUser * forwardSpeedUser < DEADZONE * DEADZONE)
            forwardSpeedUser = 0;
        if (turnSpeedUser * turnSpeedUser < DEADZONE * DEADZONE)
            turnSpeedUser = 0;
        
        // Limit max speed based on lift height
        double speedDampen = (LIFT_ABS_TOP_POS - liftPos) / (LIFT_ABS_TOP_POS * LIFT_SLOWDOWN);
        if (speedDampen > 1)
            speedDampen = 1;
        if (speedDampen < 0.5)
            speedDampen = 0.5;
        
        forwardSpeedUser *= speedDampen;
        turnSpeedUser *= speedDampen;
        
        // Variable dampening based on lift height
        double forwardDampen = DRIVE_DAMPEN + FORWARD_DAMPEN * (liftPos / LIFT_ABS_TOP_POS);
        double turnDampen = DRIVE_DAMPEN + TURN_DAMPEN * (liftPos / LIFT_ABS_TOP_POS);
        
        if (liftPos < LIFT_DAMPEN_THRESHOLD) {
            forwardDampen = 1;
            turnDampen = 1;
        }
        
        double turnSpeed = turnSpeed + (turnSpeedUser - turnSpeed) / turnDampen;
        double forwardSpeed = forwardSpeed + (forwardSpeedUser - forwardSpeed) / forwardDampen;
        
        
        // Auton control code here:
        
        
        
        // Finally set motor voltages
        driveR1.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveR2.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveR3.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveR4.move_voltage(12000 * (forwardSpeed - turnSpeed) / 127);
        driveL1.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
        driveL2.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
        driveL3.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
        driveL4.move_voltage(12000 * (forwardSpeed + turnSpeed) / 127);
        
        
        // Let other tasks have a turn
        delay(20);
    }
}

