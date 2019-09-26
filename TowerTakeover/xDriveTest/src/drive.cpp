#include "main.h"


// Drive motors
Motor mFRA(8,SPEED,0);
Motor mFRB(7,SPEED,1);
Motor mBRA(9,SPEED,0);
Motor mBRB(10,SPEED,1);
Motor mFLA(13,SPEED,0);
Motor mFLB(17,SPEED,1);
Motor mBLA(19,SPEED,0);
Motor mBLB(20,SPEED,1);

// Drive task (xDrive)
void runDrive(void* params)
{
 
    while (true) {
        
        // Run position update function
        trackPosition();
        
        // Var to hold turn speed
        double turn = controller.get_analog(ANALOG_RIGHT_X);
        
        // Fit turn to curve
        if (turn < 0)
            turn = -turn * turn / 127;
        else {
            turn = turn * turn / 127;
        }
        
        // Get forward/backward component of desired velocity
        double upDrive=controller.get_analog(ANALOG_LEFT_Y);
        // Get left/right component of desired velocity
        double rightDrive=controller.get_analog(ANALOG_LEFT_X);
        
        // Prevent div/0 error for next step
        if (rightDrive==0)
            rightDrive=0.1;
        
        // Now convert components of desired velocity into angle
        double driveAngle = atan((upDrive) / (rightDrive));
        
        // Change angle to degrees
        driveAngle = (180 * driveAngle) / (double)M_PI;

        
        // Fix quadrant error
        if (rightDrive < 0)
            driveAngle = 180 + driveAngle;
        
        // Find magnitude of joystick
        // Use mag^2 / 127 to give different curve
        double driveMag = /*sqrt*/((rightDrive*rightDrive) + (upDrive*upDrive)) / 127;
        if (driveMag>127)
            driveMag = 127;
        
        // Check if magnitude is less than min. threshold & make zero if yes
        if (driveMag < JOYZONE){
            driveMag = 0;
        }
        // Check if turn is less than min. threshold & make zero if yes
        if (abs(turn) < JOYZONE) {
            turn = 0;
        }
        
        // Find direction robot is facing
        double gyroAng=globalDirection;
        
        // Change robot's reference frame to be relative to rest of world
        driveAngle = driveAngle - gyroAng;
        
        // Clamp angle to be [0,360] after doing subtraction
        if (driveAngle  <0)
            driveAngle = 360 + driveAngle;
        
        // Convert back to radians
        driveAngle = ((double)M_PI * driveAngle) / 180;
        
        // Debug face & drive angles
        pros::lcd::print(2, "driveAngle: %f", driveAngle*180/M_PI);
        pros::lcd::print(4, "driveMag: %f", driveMag);
        
        
        
        // Manual buttons
        // Drive back slowly button
        if (controller.get_digital(DIGITAL_DOWN)) {
            driveMag = 30;
            driveAngle = 3*M_PI/2;
        }
        
        // Button to reset position info
        if (controller.get_digital(DIGITAL_X)) {
            globalDirection = 0;
            globalX = 0;
            globalY = 0;
        }
        
        
        // Set each wheel speed to correct value
        double speedFL = driveMag*( cos(driveAngle) + sin(driveAngle) ) + turn;
        double speedBL = -driveMag*( cos(driveAngle+(double)M_PI/2) + sin(driveAngle+(double)M_PI/2) ) + turn;
        double speedBR = driveMag*( cos(driveAngle+(double)M_PI) + sin(driveAngle+(double)M_PI) ) + turn;
        double speedFR = -driveMag*( cos(driveAngle+(3*(double)M_PI/2)) + sin(driveAngle+(3*(double)M_PI/2)) ) + turn;

        // Finally set motor values
        // Map our speed values [-127,127] to voltage [-12000,12000]
        mFLA.move_voltage(speedFL * 12000 / 127);
        mFLB.move_voltage(speedFL * 12000 / 127);
        mFRA.move_voltage(speedFR * 12000 / 127);
        mFRB.move_voltage(speedFR * 12000 / 127);
        mBLA.move_voltage(speedBL * 12000 / 127);
        mBLB.move_voltage(speedBL * 12000 / 127);
        mBRA.move_voltage(speedBR * 12000 / 127);
        mBRB.move_voltage(speedBR * 12000 / 127);
        
        // Update fast to help position tracking
        delay(10);
    }
}
