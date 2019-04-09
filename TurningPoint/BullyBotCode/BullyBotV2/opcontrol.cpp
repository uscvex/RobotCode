
#include "main.h"
using namespace pros;
                                                                 /* Button definition */
#define ANLG_FB ANALOG_LEFT_Y                                          // Define front-back channel for arcade mode drive
#define ANLG_LR ANALOG_RIGHT_X                                         // Define turning channel for arcade mode

#define BTN_ARM_UP DIGITAL_UP                                          // Manually raise arm
#define BTN_ARM_DOWN DIGITAL_DOWN                                      // Manually lower arm
#define BTN_MANUAL_CW DIGITAL_RIGHT                                    // Manually rotate wrist clockwise
#define BTN_MANUAL_CCW DIGITAL_LEFT                                    // Manually rotate wrist counterclockwise
#define BTN_FLOOR_FLIP DIGITAL_B                                       // For automatic cap-flipping routine
#define BTN_TARE_ARM DIGITAL_Y                                         // Sets arm position to 0

#define BTN_ARM_OTHERSIDE DIGITAL_R1                                   // Toggles arm position: pole height or ground
#define BTN_ARM_AUTOBUMPUP DIGITAL_L1                                  // Raises arm predetermined number of ticks
#define BTN_ARM_AUTOBUMPDOWN DIGITAL_L2                                // Lowers arm predetermined number of ticks
#define BTN_ARM_AUTODOWN DIGITAL_R2                                    // Lowers arm to the ground

Controller controller (E_CONTROLLER_MASTER);                     /* Instantiate controller */
    
                                                                 /* Global variables and constants */
double armSeek = -1;                                                   // The position arm is trying to reach. -1 when not seeking
double armPos = 0;                                                     // Current arm position                                                                             
double armPower = 0;                                                   // Power sent to the arm, from 0 to 12000
double armSpeed = 12000;                                               // Max speed of arm (usual speed)

int colorMultiplier = 1;                                               // 1 or -1. Causes mirroring of auton routines for red/blue sides

int flipStep = -1;                                                     // Tracks step# in a multi-step routine
int closeEnough = 10;                                                  // Threshold error for motor position seeking
int DEGS_FOR_COMPLETE_DOWN = 20;                                       // Used instead of 0 when seeking ground. Compensates for mech issues
int BUMP_VALUE = 110;                                                  // Amount (ticks) by which arm goes up/down
int UPPER_ARM_THRESHOLD = 1300;                                        // Highest position the arm can go

double POLE_HEIGHT_DEGS = 750;                                         // Arm height to reach low pole
double ABOVE_POLE_HEIGHT_DEGS = 1200;                                  // Arm height to seek when flipping cap above pole

                                                                    /* Motors Instantiation */
Motor left1(8, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);    // Set drive motors
Motor left2(2, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);
Motor left3(10, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);
Motor left4(9, E_MOTOR_GEARSET_18, false, E_MOTOR_ENCODER_DEGREES);
Motor right1(6, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
Motor right2(4, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
Motor right3(15, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
Motor right4(5, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);

Motor wrist(1, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);     // Wrist motor
Motor flipper(8, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);   // Arm motor

/* Zero motors at beginning of the match */
void myInit(void* param){
    wrist.tare_position();
    flipper.tare_position();
}

/* Drive in arcade mode activated*/
void enable_drive(void* param){
    while(true) {
        left1.move_voltage((controller.get_analog(ANLG_FB) +  controller.get_analog(ANLG_LR))*12000/127);
        left2.move_voltage((controller.get_analog(ANLG_FB) +  controller.get_analog(ANLG_LR))*12000/127);
        left3.move_voltage((controller.get_analog(ANLG_FB) +  controller.get_analog(ANLG_LR))*12000/127);
        left4.move_voltage((controller.get_analog(ANLG_FB) +  controller.get_analog(ANLG_LR))*12000/127);
        right1.move_voltage((controller.get_analog(ANLG_FB) -  controller.get_analog(ANLG_LR))*12000/127);
        right2.move_voltage((controller.get_analog(ANLG_FB) -  controller.get_analog(ANLG_LR))*12000/127);
        right3.move_voltage((controller.get_analog(ANLG_FB) -  controller.get_analog(ANLG_LR))*12000/127);
        right4.move_voltage((controller.get_analog(ANLG_FB) -  controller.get_analog(ANLG_LR))*12000/127);
        pros::delay(20);
    }
}

/* Task for arm controls */
void manual_arm(void* param){
    armSeek = -1;
    
    while(true){
        armPower = 0;
        if(controller.get_digital(BTN_ARM_UP)) //manual arm up
        {
            armPower = armSpeed;
            if(armPos > UPPER_ARM_THRESHOLD){ //dont keep going. Gonna skip gears
                pros::lcd::print(0, "armPos: %f\n", armPos);
                pros::lcd::print(0, "armPos: %f\n", armPos);
                armPower = 0;
            }
            armSeek = -1;
        }
        else if(controller.get_digital(BTN_ARM_DOWN)) //manual arm down
        {
            armSeek = -1;
            armPower = -1*armSpeed;
            if(armPos < -15){ //dont keep going. Gonna skip gears
                armPower = 0;
            }
        }
        else if(controller.get_digital(BTN_ARM_AUTOBUMPUP)){
            if(armPos + BUMP_VALUE <= UPPER_ARM_THRESHOLD){
                armSeek = armPos + BUMP_VALUE;
            }
        }
        else if(controller.get_digital(BTN_ARM_AUTOBUMPDOWN)){
            if(armPos - BUMP_VALUE >= -20){
                armSeek = armPos - BUMP_VALUE;
            }
        }
        else if(controller.get_digital(BTN_TARE_ARM)){
            //motorApexA.tare_position();
            //motorApexB.tare_position();
        }
        
        /*
         else if(controller.get_digital(BTN_ARM_AUTODOWN)){
         if(isFlipped == true)
         //wristSeek = 0;
         armSeek = DEGS_FOR_COMPLETE_DOWN;
         }*/
        else{ //manual mode...
            armPower = 0;
        }
        
        if(controller.get_digital(BTN_ARM_OTHERSIDE)){
            if(justFlipped == false){
                justFlipped = true;
                pros::lcd::print(5, "(t)justFlipped: ", justFlipped);
                if(armSeek == POLE_HEIGHT_DEGS)
                    armSeek = 100;
                else
                    armSeek = OTHER_SIDE;
                
            }
        }
        else{
            justFlipped = false;
            pros::lcd::print(5, "(f)justFlipped: ", justFlipped);
            
        }
        /*
         if(controller.get_digital(BTN_MANUAL_CW)) //Y
         {
         wristSeek = -1;
         motorWrist.tare_position();
         wristPower = MANUAL_WRIST_SPEED;
         }
         
         else if(controller.get_digital(BTN_MANUAL_CCW)) //A
         {
         wristSeek = -1;
         motorWrist.tare_position();
         wristPower = -1*MANUAL_WRIST_SPEED;
         }
         else if(controller.get_digital(BTN_FLOOR_FLIP)) //
         {
         flipStep = 1;
         armSeek = armPos + 400;
         }
         /*
         else if(controller.get_digital(BTN_WRIST_AUTOCW)){
         if(isFlipped == false) //pos = 0
         wristSeek = DEGS_FOR_A_180;
         else if(isFlipped == true){
         wristSeek = 0; //original position
         }
         }
         else{
         wristPower = 0;
         }
         */
        //flipstep conditionals
        /*
         if(flipStep == 1){
         if(abs(armSeek-(armPos+60) < closeEnough+15)){ //reached end of lift
         flipStep = 2;
         }
         }
         //flip wrist
         if(flipStep == 2){
         if(isFlipped == false) //pos = 0
         wristSeek = DEGS_FOR_A_180;
         else if(isFlipped == true){
         wristSeek = 0; //original position
         }
         flipStep = 3;
         
         pros::lcd::print(5, "2entered flipstep2 conditional");
         }
         
         if(flipStep == 3){
         armSeek = armPos-400;
         flipStep = 4;
         pros::lcd::print(6, "3entered flipstep3 conditional");
         }
         if(flipStep == 4){
         if(abs(armSeek-(armPos+60) < closeEnough+15)){ //reached end of lift
         flipStep = -1;
         }
         pros::lcd::print(7, "4entered flipstep4 conditional");
         }
         */
        //armPos = motorApexA.get_position();
        //wristPos = motorWrist.get_position();
        pros::lcd::print(0, "armPosA: %f\n", armPos);
        //pros::lcd::print(1, "armPosB: %f\n", motorApexB.get_position());
        pros::lcd::print(2, "armSeek: %f\n", armSeek);
        //pros::lcd::print(3, "wristPos: %f\n", wristPos);
        //pros::lcd::print(4, "wristSeek: %f\n", wristSeek);
        /*
         if(abs(wristPos-wristSeek) < closeEnough)
         {
         if(wristSeek == DEGS_FOR_A_180)
         isFlipped = true;
         else if(wristSeek == 0)
         isFlipped = false;
         }
         */
        //if doing an auto arm thing, auto-set to perfect speed
        if(armSeek != -1){
            armPower = (armSeek - armPos)*30;
            if(armPower > 12000)
                armPower = 12000;
            if(armPower < -12000)
                armPower = -12000;
        }
        
        //motorApexA.move_voltage(armPower);
        
        pros::delay(20);
    }
}


int armAutonSeek = -1;

void opcontrol() {
    bool justPushed = false;
    while (true) {
        armAutonSeek = -1; //don't run auton arms during opcontrol.
        
        if(controller.get_digital(DIGITAL_X)){
            if(!justPushed){//if we're toggling
                colorMultiplier*=-1;
                if(colorMultiplier>=0){
                    pros::lcd::print(5,"Red Mode %d\n", colorMultiplier);
                }
                else{
                    pros::lcd::print(5,"Blue Mode %d\n", colorMultiplier);
                }
            }
            justPushed = true;
        }
        else{
            justPushed = false;
        }
        delay(20);
    }
}
/*Auton Vars*/
int MIN_DRIVE_VOLTAGE = 5000;
int armAutonPos = -1;
int armAutonPower = 0;

/*Auton Code*/

/*Zeros all motors on the robot*/
void tareDriveMotors(){
    left1.tare_position();
    left2.tare_position();
    left3.tare_position();
    left4.tare_position();
    right1.tare_position();
    right2.tare_position();
    right3.tare_position();
    right4.tare_position();
}
void driveMotorsAt(int driveVoltage){
    left1.move_voltage(driveVoltage);
    left2.move_voltage(driveVoltage);
    left3.move_voltage(driveVoltage);
    left4.move_voltage(driveVoltage);
    right1.move_voltage(driveVoltage);
    right2.move_voltage(driveVoltage);
    right3.move_voltage(driveVoltage);
    right4.move_voltage(driveVoltage);
}

/*positive = counterclockwise*/
void turnMotorsAt(int wheelSeek){
    tareDriveMotors();
    int wheelPos = right1.get_position();//0 to neg
    
    if(wheelSeek > 0){
        while(wheelPos < wheelSeek){
            
            int driveVoltage = (wheelSeek - wheelPos)*60;
            if(driveVoltage < MIN_DRIVE_VOLTAGE)
                driveVoltage = MIN_DRIVE_VOLTAGE;
            left1.move_voltage(-1*driveVoltage);
            left2.move_voltage(-1*driveVoltage);
            left3.move_voltage(-1*driveVoltage);
            left4.move_voltage(-1*driveVoltage);
            right1.move_voltage(driveVoltage);
            right2.move_voltage(driveVoltage);
            right3.move_voltage(driveVoltage);
            right4.move_voltage(driveVoltage);
            wheelPos = right1.get_position();
        }
        driveMotorsAt(0);
    }
    else{
        while(wheelPos > wheelSeek){
            
            int driveVoltage = (wheelSeek - wheelPos)*60;
            if(driveVoltage > -1*MIN_DRIVE_VOLTAGE)
                driveVoltage = -1*MIN_DRIVE_VOLTAGE;
            
            left1.move_voltage(-1*driveVoltage);
            left2.move_voltage(-1*driveVoltage);
            left3.move_voltage(-1*driveVoltage);
            left4.move_voltage(-1*driveVoltage);
            right1.move_voltage(driveVoltage);
            right2.move_voltage(driveVoltage);
            right3.move_voltage(driveVoltage);
            right4.move_voltage(driveVoltage);
            wheelPos = right1.get_position();
        }
        driveMotorsAt(0);
    }
    delay(1);
}

void driveForward(int wheelSeek){
    int t0 = pros::millis() + abs(wheelSeek)*3;
    tareDriveMotors();
    int wheelPos = left2.get_position();
    if(wheelSeek > 0){
        while(wheelPos < wheelSeek){
            if (pros::millis() > t0)
                break;
            pros::lcd::print(4,"Positive Seek: %f\n", wheelPos);
            
            wheelPos = left2.get_position();
            int driveVoltage = (wheelSeek - wheelPos)*60;
            
            if(driveVoltage < MIN_DRIVE_VOLTAGE)
                driveVoltage = MIN_DRIVE_VOLTAGE;
            
            driveMotorsAt(driveVoltage);
        }
        driveMotorsAt(0);
    }
    else{
        while(wheelPos > wheelSeek){
            if (pros::millis() > t0)
                break;
            wheelPos = left2.get_position();
            int driveVoltage = (wheelSeek - wheelPos)*60;
            
            if(driveVoltage > -1*MIN_DRIVE_VOLTAGE)
                driveVoltage = -1*MIN_DRIVE_VOLTAGE;
            
            driveMotorsAt(driveVoltage);
        }
        driveMotorsAt(0);
    }
    delay(1);
}
/*slow drive fwd*/
void driveForward(int wheelSeek, int maxVoltage){
    int t0 = pros::millis();
    
    tareDriveMotors();
    int wheelPos = left2.get_position();
    if(wheelSeek > 0){
        while(wheelPos < wheelSeek){
            if(pros::millis() == t0 + abs(wheelSeek) * 3)
                break;
            
            wheelPos = left2.get_position();
            int driveVoltage = (wheelSeek - wheelPos)*60;
            
            if(driveVoltage < MIN_DRIVE_VOLTAGE)
                driveVoltage = MIN_DRIVE_VOLTAGE;
            /*if(driveVoltage > maxVoltage)
             driveVoltage = maxVoltage;*/
            
            driveMotorsAt(driveVoltage);
        }
        driveMotorsAt(0);
    }
    else{
        while(wheelPos > wheelSeek){
            if(pros::millis() == t0 + abs(wheelSeek) * 3)
                break;
            wheelPos = left2.get_position();
            int driveVoltage = (wheelSeek - wheelPos)*60;
            
            if(driveVoltage > -1*MIN_DRIVE_VOLTAGE)
                driveVoltage = -1*MIN_DRIVE_VOLTAGE;
            
            driveMotorsAt(driveVoltage);
        }
        driveMotorsAt(0);
    }
    delay(1);
}

/*
 void armMove(int seekDegrees){
 motorApexA.tare_position();
 motorApexB.tare_position();
 
 int t0 = pros::millis();
 int autoArmPos = motorApexA.get_position();
 if(seekDegrees > 0){
 while(autoArmPos < seekDegrees){
 if(pros::millis() == t0 + abs(seekDegrees) * 3)
 break;
 autoArmPos = motorApexA.get_position();
 int driveVoltage = (seekDegrees - autoArmPos)*35;
 
 motorApexA.move_voltage(driveVoltage);
 motorApexB.move_voltage(driveVoltage);
 
 
 }
 }
 
 else{
 while(autoArmPos > seekDegrees){
 
 if(pros::millis() == t0 + abs(seekDegrees) * 3)
 break;
 
 autoArmPos = motorApexA.get_position();
 int driveVoltage = (seekDegrees - autoArmPos)*35;
 
 if(driveVoltage > -1*MIN_DRIVE_VOLTAGE)
 driveVoltage = -1*MIN_DRIVE_VOLTAGE;
 
 motorApexA.move_voltage(driveVoltage);
 
 }
 }
 }
 */
void run_arm(void* param){
    motorApexA.tare_position();
    
    while(true){
        if(armAutonSeek != -1){
            armAutonPos = motorApexA.get_position();
            pros::lcd::print(4,"armAutonPos: %f\n", armAutonPos);
            pros::lcd::print(5,"armAutonSeek: %f\n", armAutonSeek);
            
            armAutonPower = (armAutonSeek - armAutonPos)*60;
            if(armAutonSeek > 0){
                if(armAutonPower > 12000)
                    armAutonPower = 12000;
                
            }
            else if(armAutonSeek < 0){
                if(armAutonPower < -12000)
                    armAutonPower = -12000;
                
            }
            
            pros::lcd::print(6,"armPower: %f\n", armAutonPower);
            
            //motorApexA.move_voltage(armAutonPower);
            //motorApexB.move_voltage(armAutonPower);
        }
        pros::delay(20);
    }
    pros::lcd::print(7,"Error: loop exited");
}

void runAuton() {
    armAutonSeek = 0;
    
    driveForward(-377,8000);
    delay(1200);
    driveForward(870,8000);
    delay(1000);
    
    //200 turn = 90 deg.
    turnMotorsAt(-300*colorMultiplier);
    delay(1000);
    driveForward(-100,5000);
    turnMotorsAt(120*colorMultiplier);
    delay(1000);
    driveForward(-350,8000); //engage w/ cap
    delay(1000);
    driveForward(1000,9900);
    driveMotorsAt(0);
    
    delay(1000);
    
    armAutonSeek = 350;
    delay(1200);
    //flipCap();
    
    delay(500);
    armAutonSeek = 0;
    
    delay(500);
    driveForward(-304,8000);
    delay(500);
    
    turnMotorsAt(-120);
    delay(500);
    driveForward(450,12000);
    driveMotorsAt(0);
    delay(500);
    
    turnMotorsAt(170);
    delay(500);
    
    driveForward(-1100,12000);
    driveMotorsAt(0);
    delay(1000);
    driveForward(350,12000);
    
    delay(1000);
    turnMotorsAt(230);
    
    delay(500);
    
    driveForward(-800,12000);
    driveMotorsAt(0);
    delay(1000);
    
    armAutonSeek = 377;
    
    turnMotorsAt(-200);
    delay(1000);
    
    driveForward(400,3000);
    driveMotorsAt(0);
    
    delay(1000);
    
    driveForward(1200,12000);
    driveMotorsAt(0);
}
