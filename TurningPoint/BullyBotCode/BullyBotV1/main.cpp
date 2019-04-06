
#include "robot-config.h"
#include <iostream>


int apexSpeedPCT = 50;                          //global vars
int armParallelSpeedPCT=100;
int armParallelPositionDEG=45;
int armExtendedSpeedPCT=100;
int armExtendedPositionDEG=135;
int manualArmSpeedPCT=80;
int armAutoReturnSpeedPCT=100;

int step=-1;                                    //for auton motor position seeking
double seek=-1;
double currentPos=0;
double speed=0;

//competition object
vex::competition Competition;

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */                                                                           */
/*  Called once after cortex powered on (NOT every time robot is disabled)   */
/*---------------------------------------------------------------------------*/

void pre_auton( void ) {
}

/*---------------------------------------------------------------------------*/
/*                              Autonomous Task                              */
/*---------------------------------------------------------------------------*/

void autonomous( void ) {
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
        
        
        //11.5
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
void runner(double a, double b, double c, double d, double e, double f){
    LeftMotor1.rotateFor(vex::directionType::fwd, a, vex::rotationUnits::deg, false);
    LeftMotor2.rotateFor(vex::directionType::fwd, b, vex::rotationUnits::deg, false);
    LeftMotor3.rotateFor(vex::directionType::fwd, c, vex::rotationUnits::deg, false);
    RightMotor1.rotateFor(vex::directionType::fwd, d, vex::rotationUnits::deg);
    RightMotor2.rotateFor(vex::directionType::fwd, e, vex::rotationUnits::deg);
    RightMotor3.rotateFor(vex::directionType::fwd, f, vex::rotationUnits::deg);
}

/*---------------------------------------------------------------------------*/
/*                              User Control Task+                           */
/*---------------------------------------------------------------------------*/

/*
Brings arm up, then brings it down. (arm goes higher up w/ more negative numbers) 
Enabled w/ usercontrol
*/
void armcontrol(void) {
    ApexMotor1.startRotateTo(-250,vex::rotationUnits::deg, 100, vex::velocityUnits::pct);//up
    vex::task::sleep(500);
    ApexMotor1.startRotateTo(-100,vex::rotationUnits::deg, 100, vex::velocityUnits::pct);//down
    vex::task::sleep(300);
    ApexMotor1.startRotateTo(0, vex::rotationUnits::deg, 25, vex::velocityUnits::pct);//down slower
    vex::task::sleep(300);
}

/*For opcontrol/ non-autonomous purposes*/
void usercontrol( void ) { 
    //Enable arm
    armcontrol();
    
    //Drive Control: Arcade Mode
    while (1) {
        LeftMotor1.spin(vex::directionType::fwd, (Controller1.Axis3.value() + Controller1.Axis1.value()/2), vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor2.spin(vex::directionType::fwd, (Controller1.Axis3.value() + Controller1.Axis1.value()/2), vex::velocityUnits::pct); //(Axis3+Axis1)
        LeftMotor3.spin(vex::directionType::fwd, (Controller1.Axis3.value() + Controller1.Axis1.value()/2), vex::velocityUnits::pct); //(Axis3+Axis1)
        RightMotor1.spin(vex::directionType::fwd, (Controller1.Axis3.value() - Controller1.Axis1.value()/2), vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor2.spin(vex::directionType::fwd, (Controller1.Axis3.value() - Controller1.Axis1.value()/2), vex::velocityUnits::pct);//(Axis3-Axis1)
        RightMotor3.spin(vex::directionType::fwd, (Controller1.Axis3.value() - Controller1.Axis1.value()/2), vex::velocityUnits::pct);//(Axis3-Axis1)
        
        vex::task::sleep(20); //Prevents wasted resources.
    }
}

int main() {
    pre_auton();
        
    //Set up callbacks for autonomous and driver control periods.
    Competition.autonomous( autonomous );
    Competition.drivercontrol( usercontrol );
    
    //Prevent main from exiting with an infinite loop.
    while(1) {
        vex::task::sleep(100);//Sleep the task for a short amount of time to prevent wasted resources.
    }
    
}
