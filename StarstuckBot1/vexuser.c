#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"
#include "../Common/easing.h"

// Motor mappings

// Lift
#define M_LIFT_RIGHT       kVexMotor_3
#define M_LIFT_LEFT        kVexMotor_9

// Drive
#define M_DRIVE_RIGHT_B    kVexMotor_2
#define M_DRIVE_LEFT_B     kVexMotor_4
#define M_DRIVE_LEFT_F     kVexMotor_8
#define M_DRIVE_RIGHT_F    kVexMotor_6

//Followers
#define M_DUMPER_TOP       kVexMotor_7
#define M_DUMPER_BOTTOM        kVexMotor_5


// Sensor mappings


#define P_DRIVE_ENC_RIGHT_A kVexDigital_7
#define P_DRIVE_ENC_RIGHT_B kVexDigital_8
#define P_DRIVE_ENC_LEFT_A  kVexDigital_9
#define P_DRIVE_ENC_LEFT_B  kVexDigital_10


#define P_DUMP_A kVexDigital_5
#define P_DUMP_B kVexDigital_6


#define P_LIFT_LEFT_A   kVexDigital_1
#define P_LIFT_LEFT_B  kVexDigital_2

#define P_LIFT_RIGHT_A   kVexDigital_3
#define P_LIFT_RIGHT_B  kVexDigital_4


#define S_DRIVE_ENC_LEFT  kVexSensorDigital_9
#define S_DRIVE_ENC_RIGHT kVexSensorDigital_7
#define S_LIFT_LEFT       kVexSensorDigital_1
#define S_LIFT_RIGHT      kVexSensorDigital_3
#define S_DUMP           kVexSensorDigital_5


// Controller mappings
#define J_LIFT_UP     Btn5U
#define J_LIFT_DOWN   Btn5D


#define J_DRIVE       Ch3
#define J_TURN        Ch1


#define J_DUMPER_DOWN   Btn6D
#define J_DUMPER_UP   Btn6U


#define AUTON_SPEED 350


// PID Controls
EPidController *leftDrivePid;
EPidController *rightDrivePid;



//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {

  { M_DUMPER_TOP,    kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_DUMPER_BOTTOM,     kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_LIFT_RIGHT,         kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_LIFT_LEFT,          kVexMotor393S, kVexMotorReversed,    kVexSensorNone,  0 },
  { M_DRIVE_RIGHT_B,      kVexMotor393S, kVexMotorReversed,    kVexSensorNone,  0 },
  { M_DRIVE_LEFT_B,       kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_DRIVE_LEFT_F,       kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_DRIVE_RIGHT_F,      kVexMotor393S, kVexMotorReversed,    kVexSensorNone,  0 }
};

static vexDigiCfg dConfig[] = {

  { P_DRIVE_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1, kVexQuadEncoder_1 },
  { P_DRIVE_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2, kVexQuadEncoder_1 },

  { P_DRIVE_ENC_LEFT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_3 },
  { P_DRIVE_ENC_LEFT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },

  { P_LIFT_LEFT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_4 },
  { P_LIFT_LEFT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_4 },

  { P_LIFT_RIGHT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_2 },
  { P_LIFT_RIGHT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_2 },

  { P_DUMP_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_5 },
  { P_DUMP_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_5 }

};

//-------------------Setup----------------------------------------------------//
void vexUserSetup()
{
    //vexDigitalPinSet(kVexDigital_5, 0);
    vexMotorConfigure( mConfig,   MOT_CONFIG_SIZE( mConfig ) )  ;
    vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
}

void vexUserInit()
{
    // Initialize PID
    leftDrivePid  =  EPidInit(kMinJerk, 0.01, 0, 0.01, S_DRIVE_ENC_LEFT,  false);
    rightDrivePid =  EPidInit(kMinJerk, 0.01, 0, 0.01, S_DRIVE_ENC_RIGHT,  true);
   
}

//-------------Miscellaneous functions----------------------------------------//

bool driveMotors(void) {
  short ld, rd ;
  //Calculate Motor Power
  int forward = VALLEY(vexControllerGet(J_DRIVE), 20, 127);
  int turn;


  int turnChannel = vexControllerGet(J_TURN) * 0.6;

  turn = VALLEY(turnChannel, 20, 127);
  ld = VALLEY(forward + turn, 20, 127);
  rd = VALLEY(forward - turn, 20, 127);
// 
  vexMotorSet(M_DRIVE_LEFT_B,  ld);
  vexMotorSet(M_DRIVE_RIGHT_B, rd);
  vexMotorSet(M_DRIVE_LEFT_F,  ld);
  vexMotorSet(M_DRIVE_RIGHT_F, rd);


  return (ld != 0 || rd != 0);
}



void Set_Follower_Motors(int val){
  vexMotorSet(M_DUMPER_TOP, val);
  vexMotorSet(M_DUMPER_BOTTOM, val);
}


void Set_Lift_Motors(int val){
  vexMotorSet(M_LIFT_LEFT, val);
  vexMotorSet(M_LIFT_RIGHT, val);
}


//---------------------Autonomous Functions ----------------------------------//

/**
drive_forward(double ratio);
param:
  @ratio is the ditance you want to move. One block take a second to move
*/
void drive_forward(double ratio){
  systime_t init_time = chTimeNow();

 
  systime_t duration = abs(ratio*800);
  int32_t target = 850*ratio;
  EPidEnable(rightDrivePid,duration, target);
  EPidEnable(leftDrivePid,duration, target);
  systime_t end_time = chTimeNow() + duration;
  
  while (!chThdShouldTerminate()){
      if(vexControllerGet(Btn7R)){
        vexMotorSet(M_DRIVE_LEFT_B,  0);
        vexMotorSet(M_DRIVE_RIGHT_B, 0);
        vexMotorSet(M_DRIVE_LEFT_F,  0);
        vexMotorSet(M_DRIVE_RIGHT_F, 0);
        break;
      }

      systime_t autonTime = chTimeNow() - init_time;
      if (autonTime < duration){
        int16_t ld = EPidUpdate(leftDrivePid);
        int16_t rd = EPidUpdate(rightDrivePid);
        vexMotorSet(M_DRIVE_LEFT_B,  ld);
        vexMotorSet(M_DRIVE_RIGHT_B, rd);
        vexMotorSet(M_DRIVE_LEFT_F,  ld);
        vexMotorSet(M_DRIVE_RIGHT_F, rd);
        vexSleep(10);
      }
      else{
        vexMotorSet(M_DRIVE_LEFT_B,  0);
        vexMotorSet(M_DRIVE_RIGHT_B, 0);
        vexMotorSet(M_DRIVE_LEFT_F,  0);
        vexMotorSet(M_DRIVE_RIGHT_F, 0);
        break;
      }
  }

}

void turn_deg(double ratio){
  systime_t init_time = chTimeNow();

 
  systime_t duration = abs(ratio*1900);
  int32_t target = 2300*ratio;
  EPidEnable(rightDrivePid,duration, -target);
  EPidEnable(leftDrivePid,duration, target);
  systime_t end_time = chTimeNow() + duration;
  
  while (!chThdShouldTerminate()){
      if(vexControllerGet(Btn7R)){
         vexMotorSet(M_DRIVE_LEFT_B,  0);
        vexMotorSet(M_DRIVE_RIGHT_B, 0);
        vexMotorSet(M_DRIVE_LEFT_F,  0);
        vexMotorSet(M_DRIVE_RIGHT_F, 0);
        break;
      }

      systime_t autonTime = chTimeNow() - init_time;
      if (autonTime < duration){
        int16_t ld = EPidUpdate(leftDrivePid);
        int16_t rd = EPidUpdate(rightDrivePid);
        vexMotorSet(M_DRIVE_LEFT_B,  ld);
        vexMotorSet(M_DRIVE_RIGHT_B, rd);
        vexMotorSet(M_DRIVE_LEFT_F,  ld);
        vexMotorSet(M_DRIVE_RIGHT_F, rd);
        vexSleep(10);
      }
      else{
        vexMotorSet(M_DRIVE_LEFT_B,  0);
        vexMotorSet(M_DRIVE_RIGHT_B, 0);
        vexMotorSet(M_DRIVE_LEFT_F,  0);
        vexMotorSet(M_DRIVE_RIGHT_F, 0);
        break;
      }
  }

}


void wait(double ratio){
  systime_t init_time = chTimeNow();
  systime_t duration = abs(ratio*1000);  
  while (!chThdShouldTerminate()){
      if(vexControllerGet(Btn7R)){
     
          break;
      }
      
      systime_t autonTime = chTimeNow() - init_time;
      if (autonTime < duration){
        
        vexSleep(10);
      }
      else{
        break;
      }
  }
}

// task lift_up(){
//   systime_t init_time = chTimeNow();
//   systime_t duration = 1200;
//   EPidEnable(rightLiftPid,duration, 2000);
//   EPidEnable(leftLiftPid,duration, 2000);
//   while (true){
//       if(vexControllerGet(Btn7R)){
//         break;
//       }

//       systime_t autonTime = chTimeNow() - init_time;
//       if (autonTime < duration){
//          vexMotorSet(M_LIFT_LEFT, EPidUpdate(leftLiftPid));
//          vexMotorSet(M_LIFT_RIGHT, EPidUpdate(rightLiftPid));
//       }
//       else{
//         vexMotorSet(M_LIFT_LEFT, 0);
//         vexMotorSet(M_LIFT_RIGHT, 0);
//         break;
//       }
//   }

// }

task lift_up(){
  systime_t init_time = chTimeNow();

  // StartTask(lift_up_1);
  systime_t duration = abs(800);
  
 
  
  while (!chThdShouldTerminate()){
      if(vexControllerGet(Btn7R)){
        break;
      }

      systime_t autonTime = chTimeNow() - init_time;
      if (autonTime < duration){
        // if (vexSensorValueGet(S_LIFT_LEFT) < 550) {
          Set_Lift_Motors(100);
        // } 
      }
      else{
         Set_Lift_Motors(0);
        break;
      }
  }

}

task lift_down(){
  systime_t init_time = chTimeNow();

  // StartTask(lift_up_1);
  systime_t duration = abs(1500);
  
 
  
  while (!chThdShouldTerminate()){
      if(vexControllerGet(Btn7R)){
        break;
      }

      systime_t autonTime = chTimeNow() - init_time;
      if (autonTime < duration){
        if (vexSensorValueGet(S_DUMP) < 630)
        Set_Lift_Motors(-100);
      }
      else{
         Set_Lift_Motors(0);
        break;
      }
  }

}

void lift_down_1(){
  systime_t init_time = chTimeNow();

 
  systime_t duration = abs(700);
  
 
  
  while (!chThdShouldTerminate()){
      if(vexControllerGet(Btn7R)){
        break;
      }

      systime_t autonTime = chTimeNow() - init_time;
      if (autonTime < duration){
        Set_Lift_Motors(-100);
      }
      else{
        break;
      }
  }

}

void lift_up_2(){
  systime_t init_time = chTimeNow();
  systime_t duration = abs(1000);
  
  while (!chThdShouldTerminate()){
      if(vexControllerGet(Btn7R)){
        break;
      }

      systime_t autonTime = chTimeNow() - init_time;
      if (autonTime < duration){
        Set_Lift_Motors(100);
      }
      else{
        break;
      }
  }

}


void lift_down_2(){
  systime_t init_time = chTimeNow();
  systime_t duration = abs(1000);
  
  while (!chThdShouldTerminate()){
      if(vexControllerGet(Btn7R)){
        break;
      }

      systime_t autonTime = chTimeNow() - init_time;
      if (autonTime < duration){
        Set_Lift_Motors(-100);
      }
      else{
        break;
      }
  }

}

task dumper_retract (){
  systime_t init_time = chTimeNow();
  systime_t duration = abs(1300);
  while (!chThdShouldTerminate()){
      if(vexControllerGet(Btn7R)){
        break;
      }
      systime_t autonTime = chTimeNow() - init_time;
      if (autonTime < duration && vexSensorValueGet(S_DUMP) < 640){
        Set_Follower_Motors(-90);
      }
      else{
        Set_Follower_Motors(0);
        break;
      }
  }
}

task dumper_down (){
  systime_t init_time = chTimeNow();
  systime_t duration = abs(1000);
  while (!chThdShouldTerminate()){
      if(vexControllerGet(Btn7R)){
        Set_Follower_Motors(0);
        break;
      }
      systime_t autonTime = chTimeNow() - init_time;
      if (autonTime < duration){
        if (vexSensorValueGet(S_DUMP) >0)
          Set_Follower_Motors(60);
        else{
          Set_Follower_Motors(0);
        }
      }
      else{
        Set_Follower_Motors(0);
        break;
      }
  }
}



void dumper_deploy (){
  systime_t init_time = chTimeNow();
  systime_t duration = abs(700);
  while (!chThdShouldTerminate()){
      if(vexControllerGet(Btn7R)){
        break;
      }

      systime_t autonTime = chTimeNow() - init_time;
      if (autonTime < duration && vexSensorValueGet(S_DUMP)>0) {
        Set_Follower_Motors(100);
      }
      else{
        break;
      }
  }
}

task dumper_reset_zero (){
  systime_t init_time = chTimeNow();
  systime_t duration = abs(1000);
  while (!chThdShouldTerminate()){
      if(vexControllerGet(Btn7R)){
        break;
      }

      systime_t autonTime = chTimeNow() - init_time;
      if (autonTime < duration){
        Set_Follower_Motors(40);
        vexSensorValueSet(S_DUMP, 0);
      }
      else{
         Set_Follower_Motors(0);
        break;
      }
  }
}

void stop_dumper(){
  StopTask(dumper_retract);
  Set_Follower_Motors(0);

}
//---------------------Autonomous routine-------------------------------------//

msg_t vexAutonomous( void *arg )
{
    (void)arg;
    vexTaskRegister("auton");

    // while(!chThdShouldTerminate())
    // {
      // if(vexControllerGet(Btn7R)){
      //   break;
      // }
      // StartTask(lift_up);
      /**** Second Function Dump***/
      
      //StartTask(dumper_reset_zero);
      drive_forward(-1.23);
      turn_deg(0.245);
      wait(1);
      drive_forward(1.8);
      wait(1);
      turn_deg(-0.57);
      /** Second Function Dump End**/
      wait(1);
      drive_forward(0.35);
      wait(1);
      StartTask(dumper_retract);
      drive_forward(-1.47);
      stop_dumper();
// 
      wait(1);
      StartTask(dumper_down);
      drive_forward(1.4);
      wait(1);
      StartTask(dumper_retract);
      drive_forward(-1);
      StartTask(lift_up);
      stop_dumper();
      StartTask(dumper_down);
      drive_forward(0.5);
      turn_deg(0.25);
      drive_forward(1.5);
      turn_deg(-0.25);
      // StopTask(dumper_down);
      // StartTask(dumper_retract);
      // drive_forward(1);
      // turn_deg(0.09);
      /*StartTask(dumper_reset_zero);
      turn_deg(0.13);
      wait(0.2);
      drive_forward(-0.80);
      wait(0.2);
      turn_deg(-0.13);
      wait(0.2);
      drive_forward(-0.53);
      wait(0.2);
      StartTask(lift_up);
      wait(5);
      turn_deg(0.18);
      StartTask(dumper_reset_zero);
      drive_forward(1);
      wait(2); */
      
      // StartTask(lift_down);
      // drive_forward(1.3);
      // wait(1.5);
      // dumper_retract();

      vexSleep(10);
    // }

    return (msg_t)0;
}

//---------------------User control settings----------------------------------//

msg_t vexOperator( void *arg )
{
    (void)arg;
    vexTaskRegister("operator");
    int16_t left_lift_val = -580, right_lift_val = 580;

    bool keepLiftinPlace = false, keepDumpInPlace = false;

    //Clear encoders
    // clearDriveEncoders();

    while(!chThdShouldTerminate())
    {
      //Remember to Uncomment.
      driveMotors();

      if(vexControllerGet(Btn7L)){
        vexAutonomous(NULL);
      }

      //640 ticks
      

      if(vexControllerGet(J_LIFT_UP)){
        keepLiftinPlace = true;
        left_lift_val = vexSensorValueGet(S_LIFT_LEFT);
        right_lift_val = vexSensorValueGet(S_LIFT_RIGHT);
        Set_Lift_Motors(100);
      }
      else if(vexControllerGet(J_LIFT_DOWN)){
        keepLiftinPlace = false;
        Set_Lift_Motors(-100);
      } else{
        Set_Lift_Motors(0);      
      }


      if(vexControllerGet(J_DUMPER_DOWN) && vexSensorValueGet(S_DUMP) > 0){
        keepDumpInPlace = false;
        Set_Follower_Motors(50);
      }
      else if(vexControllerGet(J_DUMPER_UP)){
        if (vexSensorValueGet(S_DUMP) < 630)
          keepDumpInPlace = true;
        Set_Follower_Motors(-80);
      } else if  (keepDumpInPlace && !vexControllerGet(J_DUMPER_UP)){
        Set_Follower_Motors(-10);
      }
      else{
        Set_Follower_Motors(0);      
      }

      if (keepLiftinPlace && vexSensorValueGet(S_LIFT_LEFT) > left_lift_val && vexSensorValueGet(S_LIFT_RIGHT) < right_lift_val){
        Set_Lift_Motors(100);
      }

      



      if(vexControllerGet(J_DUMPER_DOWN) && vexControllerGet(Btn8L) ){
        Set_Follower_Motors(65);
        vexSensorValueSet(S_DUMP,0);
      }

      // if(vexControllerGet(J_SOLENOID_RELEASE)) {
      //   vexDigitalPinSet(P_SOLENOID_1, 0);
      //   vexDigitalPinSet(P_SOLENOID_2, 0);
      // }
      // else  if(vexControllerGet(J_SOLENOID_RETRACT)) {
      //   vexDigitalPinSet(P_SOLENOID_1, 1);
      //   vexDigitalPinSet(P_SOLENOID_2, 1);
      // }

      
      //Don't hog cpu
      vexSleep(10);
    }
    return (msg_t)0;
}
