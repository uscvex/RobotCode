////////////////////////////////////////////////
// BIG BOT DRIVE
//

#include "main.h"

// Drive Motors
#define M_FRA_P 8
#define M_FRB_P 7
#define M_BRA_P 9
#define M_BRB_P 10
#define M_FLA_P 18
#define M_FLB_P 17
#define M_BLA_P 19
#define M_BLB_P 20

// Auton tuning parameters
#define TURN_RATE 0.25           // Bigger = slower
#define MAX_TURN_SPEED 90      // Bigger = faster
#define MIN_TURN_SPEED 20       // Bigger = faster
#define MIN_DRIVE_SPEED 40      // Bigger = faster
#define DRIVE_DIST_RATE 16       // Bigger = slower

#define TURN_PULSE_ON 1         // Bigger = longer pulse
#define TURN_PULSE_OFF 16        // Bigger = longer wait between pulses
#define TURN_DRIVE_RATE 2           // Bigger = weaker turn whilst driving

#define AUTON_DRIVE_SLEW 16      // Bigger = slower

#define DEPOSIT_PLACE_SPEED 20


// Drive motors
Motor mFRA(M_FRA_P,SPEED,0);
Motor mFRB(M_FRB_P,SPEED,1);
Motor mBRA(M_BRA_P,SPEED,0);
Motor mBRB(M_BRB_P,SPEED,1);
Motor mFLA(M_FLA_P,SPEED,0);
Motor mFLB(M_FLB_P,SPEED,1);
Motor mBLA(M_BLA_P,SPEED,0);
Motor mBLB(M_BLB_P,SPEED,1);


// Auton control variables
int driveMode = USER;
double faceDir = -1;
double driveDir = -1;
double driveSpeed = 0;
double startingDrivePosX = 0;
double startingDrivePosY = 0;
double distanceToDrive = 0;

double driveToX = 0;
double driveToY = 0;


// Drive task (xDrive)
void runDrive(void* params) {
    
    double turnSeek = -1;
    
    int turnPulseCounter = 0;
    
    double driveMagSlew = 0;
    double driveMagSlewRunning = 0;
 
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
        double upDrive = controller.get_analog(ANALOG_LEFT_Y);
        // Get left/right component of desired velocity
        double rightDrive = controller.get_analog(ANALOG_LEFT_X);
        
        // Prevent div/0 error for next step
        if (rightDrive == 0)
            rightDrive = 0.1;
        
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
        
        
        /////////////////////////////////////////////////////////////////
        
        // Auton control code
        if (driveMode != USER) {
            
            // We have been given a direction to face, so record it
            turnSeek = faceDir;     // We will face this direction
            driveAngle = driveDir + 90;  // We will drive this direction
            if (driveAngle > 360) driveAngle -= 360;
            
            driveMagSlew = driveSpeed;  // We will drive at this speed
            
            // Turn power will be how hard to turn
            double turnPower = globalDirection - turnSeek;
            
            // Clamp turn to +/- 180 so we always turn shorter distance
            if (turnPower < 0) turnPower += 360;
            if (turnPower > 180) turnPower -= 360;
            
            // Decrease turn speed by tuning parameter
            turnPower /= TURN_RATE;
            
            // Clamp max speed
            if (turnPower > MAX_TURN_SPEED) turnPower = MAX_TURN_SPEED;
            if (turnPower < -MAX_TURN_SPEED) turnPower = -MAX_TURN_SPEED;
            
            
            // Record sign
            bool negative = false;
            
            // Take abs value
            if (turnPower < 0) {
                turnPower *= -1;
                negative = true;
            }
            
            // If turn is small
            if (turnPower < MIN_TURN_SPEED) {
                
                // Incremnent pulse counter
                turnPulseCounter++;
                
                // If pulse is on, then set turnPower to min speed
                if (turnPulseCounter > TURN_PULSE_OFF) {
                    turnPower = MIN_TURN_SPEED;
                }
                
                // Wrap counter when limit reached
                if (turnPulseCounter > TURN_PULSE_ON + TURN_PULSE_OFF)
                    turnPulseCounter = 0;
                
                // If we're driving fast, turn weak
                if (driveMagSlew > MIN_DRIVE_SPEED) {
                    turn /= TURN_DRIVE_RATE;
                }
                
            }
            
            // Undo the abs from earlier
            if (negative)
                turnPower *= -1;
            
            // We are driving a distance
            if (driveMode == DRIVE_DIST) {
                
                // Calculate slow down factor based on distance remaining
                double driveDistSlowDown = distanceToDrive - pythag(startingDrivePosX, startingDrivePosY, globalX, globalY);
                
                driveDistSlowDown /= DRIVE_DIST_RATE;
                
                // Make it positive
                if (driveDistSlowDown < 0) driveDistSlowDown *= -1;
                
                // Clamp to 1
                if (driveDistSlowDown > 1) driveDistSlowDown = 1;
                
                // Scale forwardSpeed by this factor
                driveMagSlew *= driveDistSlowDown;
            }
            
            // We want to drive to a point
            if (driveMode == DRIVE_TO) {
                
                // Calculate angle to drive at
                double deltaX = driveToX - globalX;
                double deltaY = driveToY - globalY;
                
                // Prevent div/0 error for next step
                if (deltaX == 0)
                    deltaX = 0.01;
                
                // Find angle to drive in
                driveAngle = atan(deltaY / deltaX);
                
                // Convert to degrees
                driveAngle = (180 * driveAngle) / (double)M_PI;
                
//                if (deltaX < 0 && deltaY >= 0) {
//                    // Quadrent 2
//                    driveAngle = 90 - driveAngle;
//                }
//                if (deltaX < 0 && deltaY < 0) {
//                    // Quadrent 3
//                    driveAngle = 180 + driveAngle;
//                }
                if (deltaX < 0)
                    driveAngle += 180;
                
                // Subtract 90 to get to our coordinate system
                //driveAngle -= 90;
                // Bound to [0,360]
                while (driveAngle < 0) driveAngle += 360;
                while (driveAngle > 360) driveAngle -= 360;

                
                // Slow down as we get close
                double driveDistSlowDown = pythag(driveToX, driveToY, globalX, globalY);
                
                driveDistSlowDown /= DRIVE_DIST_RATE;
                
                // Make it positive
                if (driveDistSlowDown < 0) driveDistSlowDown *= -1;
                
                // Clamp to 1
                if (driveDistSlowDown > 1) driveDistSlowDown = 1;
                
                // Scale forwardSpeed by this factor
                driveMagSlew *= driveDistSlowDown;
                
                pros::lcd::print(5, "driveAngle: %f", driveAngle);
            }
            
            
            // Make sure forward speed is greater that some min speed
            if (driveMagSlew > 0) {
                if (driveMagSlew < MIN_DRIVE_SPEED)
                    driveMagSlew = MIN_DRIVE_SPEED;
            }
            else if (driveMagSlew < 0) {
                if (driveMagSlew > -MIN_DRIVE_SPEED)
                    driveMagSlew = -MIN_DRIVE_SPEED;
            }
            
            // Finally set turn speed
            turn = turnPower;
            
            // Don't drive faster than we ask for
            if (driveSpeed <= driveMagSlew)
                driveMagSlew = driveSpeed;
            
            // Calculate driveMag based on slew
            driveMagSlewRunning += (driveMagSlew - driveMagSlewRunning) / AUTON_DRIVE_SLEW;
            driveMag = driveMagSlewRunning;
        }
        
        
        
        
        // Cancel auto-function
        if (controller.get_digital(DIGITAL_UP)) {
            turnSeek = -1;
            faceDir = -1;
            driveMode = USER;
        }
        
        /////////////////////////////////////////////////////////////////
        
        
        
        
        // Find direction robot is facing
        double gyroAng = globalDirection;
        
        // Change robot's reference frame to be relative to rest of world
        driveAngle = driveAngle - gyroAng;
        
        // Clamp angle to be [0,360] after doing subtraction
        if (driveAngle < 0)
            driveAngle = 360 + driveAngle;
        
        // Convert back to radians
        driveAngle = ((double)M_PI * driveAngle) / 180;
        
        // Debug face & drive angles
        pros::lcd::print(2, "driveAngle: %f", driveAngle*180/M_PI);
        pros::lcd::print(4, "driveMag: %f", driveMag);
        
        
        
        // Manual buttons
        // Drive back slowly button
        if (controller.get_digital(DIGITAL_DOWN)) {
            driveMag = 40;
            driveAngle = 3*M_PI/2;
        }
        if (driveMode == DRIVE_DEPOSIT) {
            driveAngle = 3*M_PI/2;
//            driveMag = DEPOSIT_BACKOFF_SPEED;
            turn = 0;
        }
        if (driveMode == DRIVE_PLACE) {
            driveAngle = M_PI/2;
            driveMag = DEPOSIT_PLACE_SPEED;
            turn = 0;
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
