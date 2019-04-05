
#include "robot-config.h"
#include <iostream>
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*        Description: Competition template for VCS VEX V5                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

//global vars
int apexSpeedPCT = 50;
int armParallelSpeedPCT=100;
int armParallelPositionDEG=45; //TODO: change this
int armExtendedSpeedPCT=100;
int armExtendedPositionDEG=135;
int manualArmSpeedPCT=80;
int armAutoReturnSpeedPCT=100;

int step=-1;
double seek=-1;
double currentPos=0;
double speed=0;

//Creates a competition object that allows access to Competition methods.
vex::competition Competition;

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the cortex has been powered on and    */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton( void ) {
    // All activities that occur before the competition starts
    // Example: clearing encoders, setting servo positions, ...
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void autonomous( void ) {
    // ..........................................................................
    {
        //1. move -458
        
        //1
        LeftMotor1.rotateFor(-1.273, vex::rotationUnits::rev, 80, vex::velocityUnits::pct, false);
        LeftMotor2.rotateFor(-1.273, vex::rotationUnits::rev, 80, vex::velocityUnits::pct, false);
        LeftMotor3.rotateFor(-1.273, vex::rotationUnits::rev, 80, vex::velocityUnits::pct, false);
        RightMotor1.rotateFor(-1.273, vex::rotationUnits::rev, 80, vex::velocityUnits::pct, false);
        RightMotor2.rotateFor(-1.273, vex::rotationUnits::rev, 80, vex::velocityUnits::pct, false);
        RightMotor3.rotateFor(-1.273, vex::rotationUnits::rev, 80, vex::velocityUnits::pct, false);
        vex::task::sleep(1000);
        //2
        LeftMotor1.rotateFor(1450, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        LeftMotor2.rotateFor(1450, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        LeftMotor3.rotateFor(1450, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor1.rotateFor(1450, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor2.rotateFor(1450, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor3.rotateFor(1450, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        vex::task::sleep(2000);
        //3
        LeftMotor1.rotateFor(-295, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        LeftMotor2.rotateFor(-295, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        LeftMotor3.rotateFor(-295, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor1.rotateFor(295, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor2.rotateFor(295, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor3.rotateFor(295, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        vex::task::sleep(1000);
        //4
        LeftMotor1.spin(vex::directionType::rev, 50, vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor2.spin(vex::directionType::rev, 50, vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor3.spin(vex::directionType::rev, 50, vex::velocityUnits::pct); //(Axis3+Axis1)
        RightMotor1.spin(vex::directionType::rev, 50, vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor2.spin(vex::directionType::rev, 50, vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor3.spin(vex::directionType::rev, 50, vex::velocityUnits::pct);//(Axis3-Axis1)
        vex::task::sleep(500);
        
        
        LeftMotor1.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor2.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor3.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct); //(Axis3+Axis1)
        RightMotor1.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor2.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor3.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct);//(Axis3-Axis1)
        vex::task::sleep(1000);
        
        LeftMotor1.rotateFor(10, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        LeftMotor2.rotateFor(10, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        LeftMotor3.rotateFor(10, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor1.rotateFor(-10, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor2.rotateFor(-10, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor3.rotateFor(-10, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        vex::task::sleep(1000);
        
        //5
        LeftMotor1.rotateFor(1440, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        LeftMotor2.rotateFor(1440, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        LeftMotor3.rotateFor(1440, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor1.rotateFor(1440, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor2.rotateFor(1440, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor3.rotateFor(1440, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        vex::task::sleep(2000);
        //6
        ApexMotor1.startRotateTo(-275,vex::rotationUnits::deg, 100, vex::velocityUnits::pct);//up
        vex::task::sleep(500);
        ApexMotor1.startRotateTo(-100,vex::rotationUnits::deg, 100, vex::velocityUnits::pct);//down
        vex::task::sleep(300);
        ApexMotor1.startRotateTo(0, vex::rotationUnits::deg, 25, vex::velocityUnits::pct);//down slower
        vex::task::sleep(300);
        //7
        LeftMotor1.rotateFor(-170, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        LeftMotor2.rotateFor(-170, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        LeftMotor3.rotateFor(-170, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor1.rotateFor(170, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor2.rotateFor(170, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor3.rotateFor(170, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        vex::task::sleep(1000);
        //8
        LeftMotor1.rotateFor(710, vex::rotationUnits::deg, 100, vex::velocityUnits::pct, false);
        LeftMotor2.rotateFor(710, vex::rotationUnits::deg, 100, vex::velocityUnits::pct, false);
        LeftMotor3.rotateFor(710, vex::rotationUnits::deg, 100, vex::velocityUnits::pct, false);
        RightMotor1.rotateFor(710, vex::rotationUnits::deg, 100, vex::velocityUnits::pct, false);
        RightMotor2.rotateFor(710, vex::rotationUnits::deg, 100, vex::velocityUnits::pct, false);
        RightMotor3.rotateFor(710, vex::rotationUnits::deg, 100, vex::velocityUnits::pct, false);
        vex::task::sleep(1000);
        //9
        LeftMotor1.rotateFor(-710, vex::rotationUnits::deg, 100, vex::velocityUnits::pct, false);
        LeftMotor2.rotateFor(-710, vex::rotationUnits::deg, 100, vex::velocityUnits::pct, false);
        LeftMotor3.rotateFor(-710, vex::rotationUnits::deg, 100, vex::velocityUnits::pct, false);
        RightMotor1.rotateFor(-710, vex::rotationUnits::deg, 100, vex::velocityUnits::pct, false);
        RightMotor2.rotateFor(-710, vex::rotationUnits::deg, 100, vex::velocityUnits::pct, false);
        RightMotor3.rotateFor(-710, vex::rotationUnits::deg, 100, vex::velocityUnits::pct, false);
        vex::task::sleep(1000);
        //10
        LeftMotor1.rotateFor(443, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        LeftMotor2.rotateFor(443, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        LeftMotor3.rotateFor(443, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor1.rotateFor(-443, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor2.rotateFor(-443, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        RightMotor3.rotateFor(-443, vex::rotationUnits::deg, 80, vex::velocityUnits::pct, false);
        vex::task::sleep(1000);
        
        LeftMotor1.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor2.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor3.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct); //(Axis3+Axis1)
        RightMotor1.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor2.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor3.spin(vex::directionType::fwd, 100, vex::velocityUnits::pct);//(Axis3-Axis1)
        vex::task::sleep(500);
        
        //11
        LeftMotor1.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor2.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor3.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct); //(Axis3+Axis1)
        RightMotor1.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor2.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor3.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct);//(Axis3-Axis1)
        vex::task::sleep(23000);
        
        
        //changed!!!
        LeftMotor1.spin(vex::directionType::rev, 100, vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor2.spin(vex::directionType::rev, 100, vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor3.spin(vex::directionType::rev, 100, vex::velocityUnits::pct); //(Axis3+Axis1)
        RightMotor1.spin(vex::directionType::rev, 100, vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor2.spin(vex::directionType::rev, 100, vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor3.spin(vex::directionType::rev, 100, vex::velocityUnits::pct);//(Axis3-Axis1)
        vex::task::sleep(2000);
        
        //12
        LeftMotor1.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor2.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor3.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct); //(Axis3+Axis1)
        RightMotor1.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor2.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor3.spin(vex::directionType::fwd, 0, vex::velocityUnits::pct);//(Axis3-Axis1)
        vex::task::sleep(23000);
    }
}
void runner(double a, double b, double c, double d, double e, double f){
    LeftMotor1.rotateFor(vex::directionType::fwd, a, vex::rotationUnits::deg, false);
    LeftMotor2.rotateFor(vex::directionType::fwd, b, vex::rotationUnits::deg, false);
    LeftMotor3.rotateFor(vex::directionType::fwd, c, vex::rotationUnits::deg, false);
    RightMotor1.rotateFor(vex::directionType::fwd, d, vex::rotationUnits::deg);
    RightMotor2.rotateFor(vex::directionType::fwd, e, vex::rotationUnits::deg);
    RightMotor3.rotateFor(vex::directionType::fwd, f, vex::rotationUnits::deg);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void autoArm()
{//seek 275 for the 90 degrees
    ApexMotor1.rotateTo(-230,vex::rotationUnits::deg, 15, vex::velocityUnits::pct);
    
    /*step=1;
     currentPos=ApexMotor1.rotation(vex::rotationUnits::deg);
     if(seek!=-1){
     speed=(seek-currentPos);//yields thing in degrees.
     }
     if(step==1){//seek the 90 degrees
     seek = 22;
     if (speed < 5) {
     seek = -1;
     step = -1; 
     }
     }
     ApexMotor1.setVelocity(speed, vex::velocityUnits::pct);
     ApexMotor1.spin(vex::directionType::rev);*/
    
}
void armcontrolR1(void) {
    //runArm();
    ApexMotor1.startRotateTo(-250,vex::rotationUnits::deg, 100, vex::velocityUnits::pct);//up
    vex::task::sleep(500);
    ApexMotor1.startRotateTo(-100,vex::rotationUnits::deg, 100, vex::velocityUnits::pct);//down
    vex::task::sleep(300);
    ApexMotor1.startRotateTo(0, vex::rotationUnits::deg, 25, vex::velocityUnits::pct);//down slower
    vex::task::sleep(300);
}
/*
 void armcontrolL1(void) {
 ApexMotor1.spin(vex::directionType::fwd,90,vex::velocityUnits::pct);
 
 }
 */
/*void manual(void) {
 while(true) {
 if(Controller1.ButtonL2.pressing()){
 ApexMotor1.spin(vex::directionType::fwd,90,vex::velocityUnits::pct);
 //manual down.
 }
 else if(Controller1.ButtonL1.pressing()){
 ApexMotor1.spin(vex::directionType::rev,90,vex::velocityUnits::pct);
 //manual up.
 }
 else {
 ApexMotor1.stop(vex::brakeType::brake);
 }
 
 
 }
 
 }
 */
void usercontrol( void ) {
    // User control code here, inside the loop
    armcontrolR1();
    //manual();
    while (1) {
        
        //run arm
        //**pressing returns bool
        /*if(Controller1.ButtonR1.pressing()){
         
         //runArm();
         ApexMotor1.startRotateTo(-250,vex::rotationUnits::deg, 100, vex::velocityUnits::pct);//up
         vex::task::sleep(500);
         ApexMotor1.startRotateTo(-100,vex::rotationUnits::deg, 100, vex::velocityUnits::pct);//down
         vex::task::sleep(300);
         ApexMotor1.startRotateTo(0, vex::rotationUnits::deg, 25, vex::velocityUnits::pct);//down slower
         vex::task::sleep(300);
         
         }
         
         else if(Controller1.ButtonL2.pressing()){
         ApexMotor1.spin(vex::directionType::fwd,90,vex::velocityUnits::pct);
         //manual down.
         }
         else if(Controller1.ButtonL1.pressing()){
         ApexMotor1.spin(vex::directionType::rev,90,vex::velocityUnits::pct);
         //manual up.
         }
         else {
         ApexMotor1.stop(vex::brakeType::brake);
         }
         
         
         */
        //
        //ApexMotor.rotateTo(armParallelPositionDEG,vex::rotationUnits::deg,armParallelSpeedPCT,vex::velocityUnits::pct,false);
        //ApexMotor.rotateTo(0,vex::rotationUnits::deg,armParallelSpeedPCT,vex::velocityUnits::pct,false);
        
        LeftMotor1.spin(vex::directionType::fwd, (Controller1.Axis3.value() + Controller1.Axis1.value()/2), vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor2.spin(vex::directionType::fwd, (Controller1.Axis3.value() + Controller1.Axis1.value()/2), vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor3.spin(vex::directionType::fwd, (Controller1.Axis3.value() + Controller1.Axis1.value()/2), vex::velocityUnits::pct); //(Axis3+Axis1)
        RightMotor1.spin(vex::directionType::fwd, (Controller1.Axis3.value() - Controller1.Axis1.value()/2), vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor2.spin(vex::directionType::fwd, (Controller1.Axis3.value() - Controller1.Axis1.value()/2), vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor3.spin(vex::directionType::fwd, (Controller1.Axis3.value() - Controller1.Axis1.value()/2), vex::velocityUnits::pct);//(Axis3-Axis1)
        /* if(Controller1.ButtonL2.pressing()){
         ApexMotor1.spin(vex::directionType::fwd,90,vex::velocityUnits::pct);
         //manual down.
         }
         else if(Controller1.ButtonL1.pressing()){
         ApexMotor1.spin(vex::directionType::rev,90,vex::velocityUnits::pct);
         //manual up.
         }
         else{
         ApexMotor1.spin(vex::directionType::fwd,0,vex::velocityUnits::pct);
         //ApexMotor1.spin(vex::directionType::rev,90,vex::velocityUnits::pct);
         }
         */
        vex::task::sleep(20); //Sleep the task for a short amount of time to prevent wasted resources.
    }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
    
    //Run the pre-autonomous function.
    pre_auton();
    Controller1.ButtonR1.pressed(armcontrolR1);
    //Controller1.ButtonL1.pressed(armcontrolL1);
    //Set up callbacks for autonomous and driver control periods.
    Competition.autonomous( autonomous );
    Competition.drivercontrol( usercontrol );
    
    //Prevent main from exiting with an infinite loop.
    while(1) {
        vex::task::sleep(100);//Sleep the task for a short amount of time to prevent wasted resources.
    }
    
}
