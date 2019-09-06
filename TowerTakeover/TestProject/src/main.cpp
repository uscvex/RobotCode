/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Sam2.0                                                    */
/*    Created:      Mon Aug 26 2019                                           */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"
#include <cmath>

using namespace vex;

#define JOYZONE 10

// A global instance of vex::brain used for printing to the V5 brain screen
brain Brain;
// A global instance of vex::competition
competition Competition;
controller Controller1;

// xDrive Motors
motor mFrontLeftA(1, gearSetting::ratio18_1, false);
motor mFrontLeftB(2, gearSetting::ratio18_1, true);
motor mFrontRightA(3, gearSetting::ratio18_1, false);
motor mFrontRightB(4, gearSetting::ratio18_1, true);
motor mBackLeftA(5, gearSetting::ratio18_1, false);
motor mBackLeftB(6, gearSetting::ratio18_1, true);
motor mBackRightA(7, gearSetting::ratio18_1, false);
motor mBackRightB(8, gearSetting::ratio18_1, true);


double gyroDir = 0;


void pre_auton( void ) {
  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
  
}

void autonomous( void ) {
  // ..........................................................................
  // Insert autonomous user code here.
  // ..........................................................................

}

int driveTask() {

  double upDrive = 0;
  double rightDrive = 0;
  double turn = 0;
  double driveAngle = 1;
  double driveMag = 0;
  double gyroAng;

  while (true) {

    double speedFL = 0;
    double speedBL = 0;
    double speedFR = 0;
    double speedBR = 0;

    // Get desired turn rate
    turn = Controller1.Axis1.value();
    
    // Get forward/backward component of desired velocity
    upDrive = Controller1.Axis3.value();
    // Get left/right component of desired velocity
    rightDrive = Controller1.Axis4.value();
    
    // Prevent div/0 error for next step
    if (rightDrive==0)
        rightDrive=0.1;
    
    // Now convert components of desired velocity into angle
    // Use different tan functions depending on which quad. of unit circle
    if (rightDrive*upDrive>=0)
        driveAngle = atan((upDrive) / (rightDrive));
    else
        driveAngle = atan2((upDrive),  (rightDrive));
    
    //Change it to degrees
    driveAngle = (180 * driveAngle) / M_PI;
    
    // Check quad. of unit circle again and adjust if necessary
    if (rightDrive<0 && upDrive<0)
        driveAngle=driveAngle+180;
    
    //Find magnitude of joystick
    driveMag = sqrt((rightDrive*rightDrive) + (upDrive*upDrive));
    if (driveMag>127){driveMag=127;}
    
    // Check if magnitude is less than min. threshold & make zero if yes
    if (driveMag<JOYZONE){
        driveMag=0;
    }
    // Check if turn is less than min. threshold & make zero if yes
    if (std::abs(turn)<JOYZONE){
        turn=0;
    }
    
    // Find direction robot is facing
    gyroAng=gyroDir/10;
    
    // Change robot's reference frame to be relative to rest of world
    driveAngle=driveAngle-gyroAng;
    
    // Clamp angle to be [0,360] after doing subtraction
    if (driveAngle<0)
        driveAngle=360+driveAngle;
    
    // Set each drive motor to correct add/sub or sin/cos
    speedFL = driveMag*( cos(driveAngle) - sin(driveAngle) ) + turn;
    speedFR =  driveMag*( (-cos(driveAngle)) - sin(driveAngle) ) + turn;
    speedBL = -driveMag*( cos(driveAngle) + sin(driveAngle) ) - turn;
    speedBR =  -driveMag*( cos(driveAngle) - sin(driveAngle) ) - turn;

    mFrontLeftA.moveVoltage(12);
    mFrontLeftB
    mFrontRightA
    mFrontRightB
    mBackLeftA
    mBackLeftB
    mBackRightA
    mBackRightB

    
    // Pause to allow other tasks to run
    task::sleep(20);


  }


}

void usercontrol( void ) {
  // User control code here, inside the loop
  while (1) {
    // This is the main execution loop for the user control program.
    // Each time through the loop your program should update motor + servo 
    // values based on feedback from the joysticks.

    // ........................................................................
    // Insert user code here. This is where you use the joystick values to 
    // update your motors, etc.
    // ........................................................................
 
    vex::task::sleep(20); //Sleep the task for a short amount of time to prevent wasted resources. 
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
    //Set up callbacks for autonomous and driver control periods.
    Competition.autonomous( autonomous );
    Competition.drivercontrol( usercontrol );
    
    //Run the pre-autonomous function. 
    pre_auton();
       
    //Prevent main from exiting with an infinite loop.                        
    while(1) {
      vex::task::sleep(100);//Sleep the task for a short amount of time to prevent wasted resources.
    }    
       
}