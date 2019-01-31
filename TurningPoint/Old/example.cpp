//
//  example.cpp
//  
//  Will not compile, but shows old vs new code
//  New is much cleaner after setup.
//  Created by Sam Burton on 03/11/2018.
//

#include "posmech.h"

// With new code, more setup, but way less after
void armSetup() {   // called once at start of program
    
    // Arm setup
    PositionMachine arm = PositionMachine();
    arm.addMotor(&Motor1);           // two motors on arm
    arm.addMotor(&Motor2);
    arm.setScale(0.2);              // arm is greared 1:5
    arm.setPosition(0);             // arm starts at 0
    arm.setLimits(0,180);           // arm can move 180 degrees
    arm.setRange(5);                // seek to within 5 degrees
    arm.setSeekRates(0.1,0.2);      // need more power going up
    // set manual controls
    arm.setManualPos(&Controller.ButtonL1, 100);
    arm.setManualNeg(&Controller.ButtonL2, -100);
    
}

void doArm() {  // called every loop
    
        if (Controller.ButtonX.pressing()) arm.stop();      // emergency stop
        if (Controller.ButtonR1.pressing()) arm.hold(180);  // auto move arm up
        if (Controller.ButtonR1.pressing()) arm.seek(0);    // auto move arm down
    
        arm.run();  // actually moves the arm
}



// With old code, Cortex system only

int armLPos;
int armRPos;

void doArm() {
    armLPos = SensorValue[mobileBaseSensor];    //READ VALUES FROM SENSORS
    armRPos = SensorValue[mobileBaseSensor];
    
    //USER BUTTON TO 'THROW' BASE
    if (vexRT[Btn5U]) {
        if (SensorValue(mobileBaseSensor) > baseThrowPos) {
            baseSeek = baseDownPos;
        }
        else {
            baseSeek = baseUpPos;
        }
    }
    
    baseSpeed=0;
    //Base Seek Lerp
    if (baseSeek>0) baseSpeed = -(baseSeek - mobileBaseLPos)*baseSeekRate;
    if (abs(baseSeek-mobileBaseLPos)<baseCloseEnough) baseSeek=-1;
    
    //MANUAL BASE CONTROLS
    if (vexRT[Btn6U])
    {
        baseSeek = -1;
        baseSpeed=-127;
    }
    if (vexRT[Btn6D])
    {
        baseSeek = -1;
        baseSpeed=127;
    }
    
    //now set motors
    RegMotor(mobileBaseL,baseSpeed);
    RegMotor(mobileBaseR,baseSpeed);
}
