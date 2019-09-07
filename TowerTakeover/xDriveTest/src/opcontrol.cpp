#include "main.h"

using namespace pros;

#define JOYZONE 10                          // Joystick dead zone

#define SPEED E_MOTOR_GEARSET_18            // Speed motor gearset

// Tuning value for position tracking
#define INCHESPERTICK 0.02836160034
#define TRACKINGDIAMETER 19


Controller controller(E_CONTROLLER_MASTER);
Motor mFLA(17,SPEED,0);
Motor mFLB(18,SPEED,1);
Motor mFRA(1,SPEED,0);
Motor mFRB(2,SPEED,1);
Motor mBLA(19,SPEED,0);
Motor mBLB(20,SPEED,1);
Motor mBRA(4,SPEED,0);
Motor mBRB(3,SPEED,1);

ADIEncoder leftEncoder(1,2);
ADIEncoder rightEncoder(3,4);
ADIEncoder backEncoder(5,6);


// Vars to save robot global position
double globalDirection = 0;
double globalX = 0;
double globalY = 0;


// Vars to save previous encoder values
double lastRightEncoder = 0;
double lastLeftEncoder = 0;
double lastBackEncoder = 0;


// Function to update global position based on encoder inputs
void trackPosition() {
    
    // Get values from encoders
    double rightValue = rightEncoder.get_value();
    double leftValue = leftEncoder.get_value();
    double backValue = backEncoder.get_value();
    
    // Calculate change in encoder values
    double rightChange = rightValue - lastRightEncoder;
    double leftChange = leftValue - lastLeftEncoder;
    double backChange = backValue - lastBackEncoder;
    
    // Update last values
    lastRightEncoder = rightValue;
    lastLeftEncoder = leftValue;
    lastBackEncoder = backValue;
    
    pros::lcd::print(1, "rEnc: %f", rightChange);
    pros::lcd::print(2, "lEnc: %f", leftChange);
    pros::lcd::print(3, "bEnc: %f", backChange);
    
    // Now find average change in encoders (ticks)
    double angleChange = rightChange + leftChange;
    //angleChange /= 2;
    
    // Take out the rotational component from strafe wheel
    backChange -= angleChange/2;
    
    // Convert ticks to inches
    angleChange *= INCHESPERTICK;
    
    // Find actual angle change (rad) based on distance between encoders
    angleChange /= TRACKINGDIAMETER;
    
    // Convert to degrees
    angleChange = (180 * angleChange) / M_PI;
    
    // Calculate new global direction
    globalDirection += angleChange;
    
    // Wrap to [0,360]
    if (globalDirection > 360) globalDirection -= 360;
    if (globalDirection < 0) globalDirection += 360;
    
    // Get new global direction in degrees
    double globalDirectionRad = (M_PI * globalDirection) / 180;
    
    // Find change in forward position and sideways position
    double forwardChange = INCHESPERTICK * (leftChange - rightChange) / 2;
    double strafeChange = backChange * INCHESPERTICK;
    
    // Calculate global change based on current angle
    double changeX = forwardChange * sin(globalDirectionRad) + strafeChange * cos(globalDirectionRad);
    double changeY = strafeChange * sin(globalDirectionRad) - forwardChange * cos(globalDirectionRad);
    
    // Calculate new global position
    globalX += changeX;
    globalY += changeY;
    
    pros::lcd::print(5, "X: %f", globalX);
    pros::lcd::print(6, "Y: %f", globalY);
    pros::lcd::print(7, "D: %f", globalDirection);
    
}


void runDrive(void* params)
{
    bool turnToFace = false;
    bool justToggledTurn = false;
    while (true) {
        
        // Run position update function
        trackPosition();
        
        // Get desired turn rate
        double turn = 0;//controller.get_analog(ANALOG_RIGHT_X);
        
        // Get forward/backward component of desired velocity
        double upDrive=controller.get_analog(ANALOG_LEFT_Y);
        // Get left/right component of desired velocity
        double rightDrive=controller.get_analog(ANALOG_LEFT_X);
        
        // Prevent div/0 error for next step
        if (rightDrive==0)
            rightDrive=0.1;
        
        // Now convert components of desired velocity into angle
        double driveAngle = atan((upDrive) / (rightDrive));
        
        //Change it to degrees
        driveAngle = (180 * driveAngle) / M_PI;

        // Fix quadrant error
        if (rightDrive < 0)
            driveAngle = 180 + driveAngle;
        
        
        if (controller.get_digital(DIGITAL_B)) {
            if (!justToggledTurn)
                turnToFace = !turnToFace;
            justToggledTurn = true;
        }
        else {
            justToggledTurn = false;
        }
        
        // Turn to face code
        if (!turnToFace) {
            turn = controller.get_analog(ANALOG_RIGHT_X);
        }
        else {
            // Now convert components of desired turn into angle
            double faceAngle = atan((controller.get_analog(ANALOG_RIGHT_Y)) / (controller.get_analog(ANALOG_RIGHT_X)));
            
            //Change it to degrees
            faceAngle = (180 * faceAngle) / M_PI;
            
            // Fix quadrant error
            if (controller.get_analog(ANALOG_RIGHT_X) < 0)
                faceAngle = 180 + faceAngle;
            
            faceAngle -= 90;
            if (faceAngle < 0) faceAngle += 360;
            
            double tempDir = globalDirection;
            
            if (tempDir > 180) tempDir -= 360;
            if (sqrt((controller.get_analog(ANALOG_RIGHT_Y)*controller.get_analog(ANALOG_RIGHT_Y)) + (controller.get_analog(ANALOG_RIGHT_X)*controller.get_analog(ANALOG_RIGHT_X))) > JOYZONE)
                turn = (tempDir - faceAngle);
            if (turn < 0) turn += 360;
            if (turn > 180) turn -= 360;
            turn *= 4;
        }
        
        
        
        // Find magnitude of joystick
        double driveMag = sqrt((rightDrive*rightDrive) + (upDrive*upDrive));
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
        driveAngle = (M_PI * driveAngle) / 180;
        
        // Set each wheel speed to correct value
        double speedFL = driveMag*( cos(driveAngle) + sin(driveAngle) ) + turn;
        double speedBL = -driveMag*( cos(driveAngle+M_PI/2) + sin(driveAngle+M_PI/2) ) + turn;
        double speedBR = driveMag*( cos(driveAngle+M_PI) + sin(driveAngle+M_PI) ) + turn;
        double speedFR = -driveMag*( cos(driveAngle+(3*M_PI/2)) + sin(driveAngle+(3*M_PI/2)) ) + turn;

        // Finally set motor values
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

void opcontrol() {

    pros::Task driveTask (runDrive);
    rightEncoder.reset();
    leftEncoder.reset();
    backEncoder.reset();
    
	while (true) {
		
        if (controller.get_digital(DIGITAL_X)) {
            globalDirection = 0;
            globalX = 0;
            globalY = 0;
        }
        
		delay(20);
	}
}
