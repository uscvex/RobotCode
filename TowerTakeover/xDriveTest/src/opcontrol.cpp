#include "main.h"

using namespace pros;

#define JOYZONE 10
#define SPEED E_MOTOR_GEARSET_18

Controller controller(E_CONTROLLER_MASTER);
Motor mFLA(17,SPEED,0);
Motor mFLB(18,SPEED,1);
Motor mFRA(1,SPEED,0);
Motor mFRB(2,SPEED,1);
Motor mBLA(19,SPEED,0);
Motor mBLB(20,SPEED,1);
Motor mBRA(4,SPEED,0);
Motor mBRB(3,SPEED,1);


double gyroDir = 0;

void runDrive(void* params)
{
    double upDrive;
    double turn = 0;
    double rightDrive;
    double driveAngle=1;
    double driveMag;
    double gyroAng;
    double seek,angle,ang1;
    
    double speedFL = 0;
    double speedBL = 0;
    double speedFR = 0;
    double speedBR = 0;
    
    while (true) {
        // Get desired turn rate
        turn = controller.get_analog(ANALOG_RIGHT_X);
        
        // Get forward/backward component of desired velocity
        upDrive=controller.get_analog(ANALOG_LEFT_Y);
        // Get left/right component of desired velocity
        rightDrive=controller.get_analog(ANALOG_LEFT_X);
        
        // Prevent div/0 error for next step
        if (rightDrive==0)
            rightDrive=0.1;
        
        // Now convert components of desired velocity into angle
        driveAngle = atan((upDrive) / (rightDrive));
        
        //Change it to degrees
        driveAngle = (180 * driveAngle)/ M_PI;

        // Fix quadrant error
        if (rightDrive < 0)
            driveAngle = 180 + driveAngle;
        
        // Find magnitude of joystick
        driveMag = sqrt((rightDrive*rightDrive) + (upDrive*upDrive));
        if (driveMag>127){driveMag=127;}
        
        // Check if magnitude is less than min. threshold & make zero if yes
        if (driveMag<JOYZONE){
            driveMag=0;
        }
        // Check if turn is less than min. threshold & make zero if yes
        if (abs(turn)<JOYZONE){
            turn=0;
        }
        
        // Find direction robot is facing
        gyroAng=gyroDir;
        
        // Change robot's reference frame to be relative to rest of world
        driveAngle=driveAngle-gyroAng;
        
        // Clamp angle to be [0,360] after doing subtraction
        if (driveAngle<0)
            driveAngle=360+driveAngle;
        
        driveAngle = (M_PI * driveAngle) / 180;
        
        if (controller.get_digital(DIGITAL_UP)) {
            driveAngle = M_PI/2;
        }
        if (controller.get_digital(DIGITAL_DOWN)) {
            driveAngle = 3*M_PI/2;
        }
        if (controller.get_digital(DIGITAL_RIGHT)) {
            driveAngle = 0;
        }
        if (controller.get_digital(DIGITAL_LEFT)) {
            driveAngle = M_PI;
        }
        
        
        // Set each drive motor to correct add/sub or sin/cos
        speedFL = driveMag*( cos(driveAngle) + sin(driveAngle) ) + turn;
        speedBL = -driveMag*( cos(driveAngle+M_PI/2) + sin(driveAngle+M_PI/2) ) + turn;
        speedBR = driveMag*( cos(driveAngle+M_PI) + sin(driveAngle+M_PI) ) + turn;
        speedFR = -driveMag*( cos(driveAngle+(3*M_PI/2)) + sin(driveAngle+(3*M_PI/2)) ) + turn;
        
//        speedFR =  driveMag*( cos(driveAngle) - sin(driveAngle) ) + turn;
//        speedBL = driveMag*( cos(driveAngle) - sin(driveAngle) ) + turn;
//        speedBR =  driveMag*( -cos(driveAngle) - sin(driveAngle) ) + turn;
        
        mFLA.move_voltage(speedFL * 12000 / 127);
        mFLB.move_voltage(speedFL * 12000 / 127);
        mFRA.move_voltage(speedFR * 12000 / 127);
        mFRB.move_voltage(speedFR * 12000 / 127);
        mBLA.move_voltage(speedBL * 12000 / 127);
        mBLB.move_voltage(speedBL * 12000 / 127);
        mBRA.move_voltage(speedBR * 12000 / 127);
        mBRB.move_voltage(speedBR * 12000 / 127);
        
        // Pause to allow other tasks to run
        delay(20);
    }
}

void opcontrol() {

    pros::Task driveTask (runDrive);
    
	while (true) {
		
		delay(20);
	}
}
