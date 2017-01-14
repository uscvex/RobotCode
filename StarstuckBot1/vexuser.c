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
#define M_DRIVE_LEFT_B     kVexMotor_5
#define M_DRIVE_LEFT_F     kVexMotor_8
#define M_DRIVE_RIGHT_F    kVexMotor_6

//Followers
#define M_FOLLOWERS_RIGHT       kVexMotor_7
#define M_FOLLOWERS_LEFT        kVexMotor_4


// Sensor mappings


#define P_DRIVE_ENC_RIGHT_A kVexDigital_7
#define P_DRIVE_ENC_RIGHT_B kVexDigital_8
#define P_DRIVE_ENC_LEFT_A  kVexDigital_9
#define P_DRIVE_ENC_LEFT_B  kVexDigital_10

#define S_DRIVE_ENC_LEFT  kVexSensorDigital_9
#define S_DRIVE_ENC_RIGHT kVexSensorDigital_7

// Controller mappings
#define J_LIFT_UP     Btn5U
#define J_LIFT_DOWN   Btn5D

#define J_SOLENOID_RELEASE   Btn6U
#define J_SOLENOID_RETRACT  Btn6D

#define J_DRIVE       Ch3
#define J_TURN        Ch1


#define J_FOLLOWERS_UP   Btn8U
#define J_FOLLOWERS_DOWN   Btn8D

#define J_AUTON_START Btn7L
#define J_AUTON_END   Btn7R

#define AUTON_SPEED 350


// PID Controls
EPidController *leftDrivePid;
EPidController *rightDrivePid;
// EPidController *rightLiftPid;
// EPidController *leftLiftPid;


//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {

  { M_FOLLOWERS_RIGHT,    kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_FOLLOWERS_LEFT,     kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_LIFT_RIGHT,         kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_LIFT_LEFT,          kVexMotor393S, kVexMotorReversed,    kVexSensorNone,  0 },
  { M_DRIVE_RIGHT_B,      kVexMotor393S, kVexMotorReversed,    kVexSensorNone,  0 },
  { M_DRIVE_LEFT_B,       kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_DRIVE_LEFT_F,       kVexMotor393S, kVexMotorNormal,    kVexSensorNone,  0 },
  { M_DRIVE_RIGHT_F,      kVexMotor393S, kVexMotorReversed,    kVexSensorNone,  0 }
};

static vexDigiCfg dConfig[] = {

  { P_DRIVE_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1, kVexQuadEncoder_2 },
  { P_DRIVE_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2, kVexQuadEncoder_2 },

  { P_DRIVE_ENC_LEFT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_3 },
  { P_DRIVE_ENC_LEFT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_3 },

  // { P_LIFT_ENC_LEFT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_4 },
  // { P_LIFT_ENC_LEFT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_4 },

  // { P_LIFT_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_5 },
  // { P_LIFT_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_5 }

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
    // rightLiftPid  =  EPidInit(kMinJerk, 0.001, 0, 0.01, S_LIFT_ENC_RIGHT, false);
    // leftLiftPid   =  EPidInit (kMinJerk, 0.001, 0, 0.01, S_LIFT_ENC_LEFT, false);
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

  vexMotorSet(M_DRIVE_LEFT_B,  ld);
  vexMotorSet(M_DRIVE_RIGHT_B, rd);
  vexMotorSet(M_DRIVE_LEFT_F,  ld);
  vexMotorSet(M_DRIVE_RIGHT_F, rd);


  return (ld != 0 || rd != 0);
}



void Set_Follower_Motors(int val){
  vexMotorSet(M_FOLLOWERS_RIGHT, val);
  vexMotorSet(M_FOLLOWERS_LEFT, val);
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

 
  int32_t duration = abs(ratio)*1000;
  int32_t target = 1500*ratio;
  EPidEnable(rightDrivePid,duration, target);
  EPidEnable(leftDrivePid,duration, target);
  systime_t end_time = chTimeNow() + duration;
  
  while (true){
      if(vexControllerGet(Btn7R)){
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
        break;
      }
  }

}

void turn_deg(double ratio){
  systime_t init_time = chTimeNow();

 
  int32_t duration = ratio*2350;
  duration = abs(duration);
  int32_t target = 3000*ratio;
  EPidEnable(rightDrivePid,duration, -target);
  EPidEnable(leftDrivePid,duration, target);
  systime_t end_time = chTimeNow() + duration;
  
  while (true){
      if(vexControllerGet(Btn7R)){
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
        break;
      }
  }

}

void lift_up(){
  systime_t init_time = chTimeNow();

 
  int32_t duration = 1000;
  
 
  
  while (true){
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


void lift_down(){
  systime_t init_time = chTimeNow();

 
  int32_t duration = 1000;
  
 
  
  while (true){
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
//---------------------Autonomous routine-------------------------------------//

msg_t vexAutonomous( void *arg )
{
    (void)arg;
    vexTaskRegister("auton");

    //Reset encoders
    //clearDriveEncoders();

    // systime_t init_time = chTimeNow();
    int step = 0;

    //int startOnLeft = -1 * (vexAdcGet(SIDE_CHOOSER) < 2048) ? 1 : 0;

    while(!chThdShouldTerminate())
    {
      if(vexControllerGet(Btn7R)){
        break;
      }
      // drive_forward(1, 1000);
      break;
      // systime_t autonTime = chTimeNow() - init_time;

      // if (step == 0 && autonTime > 0 && autonTime < 2400){
      //   EPidEnable(rightDrivePid, 1500, 1300);
      //   EPidEnable(leftDrivePid, 1500, 1300);
      //   step++;
      // }

      //Set motor values
    //   int16_t motorValL = EPidUpdate(leftDrivePid);
		  // int16_t motorValR = EPidUpdate(rightDrivePid);

    //   int16_t leftLiftVal = EPidUpdate(leftLiftPid);
    //   int16_t rightLiftVal = EPidUpdate(rightLiftPid);

		  // vexMotorSet(M_DRIVE_RIGHT_AB, motorValR);
    //   vexMotorSet(M_DRIVE_RIGHT_C, motorValR);
		  // vexMotorSet(M_DRIVE_LEFT_AB, motorValL);
    //   vexMotorSet(M_DRIVE_LEFT_C, motorValL);

    //   vexMotorSet(M_LIFT_LEFT_AB, leftLiftVal);
    //   vexMotorSet(M_LIFT_RIGHT_AB, rightLiftVal);

      vexSleep(10);
    }

    return (msg_t)0;
}

//---------------------User control settings----------------------------------//

msg_t vexOperator( void *arg )
{
    (void)arg;
    vexTaskRegister("operator");

    bool isMoving;

    //Clear encoders
    // clearDriveEncoders();

    while(!chThdShouldTerminate())
    {
      //Remember to Uncomment.
      driveMotors();

      // if(vexControllerGet(Btn7L)){
      //   vexAutonomous(NULL);
      // }

      if (vexControllerGet(J_AUTON_START)){
        lift_up();
        //vexAutonomous(NULL);
      } else if (vexControllerGet(J_AUTON_END)){
        lift_down();
      }
      if(vexControllerGet(J_LIFT_UP)){
        Set_Lift_Motors(100);
      }
      else if(vexControllerGet(J_LIFT_DOWN)){
        Set_Lift_Motors(-100);
      } else{
        Set_Lift_Motors(0);      
      }


      if(vexControllerGet(J_FOLLOWERS_UP)){
        Set_Follower_Motors(100);
      }
      else if(vexControllerGet(J_FOLLOWERS_DOWN)){
        Set_Follower_Motors(-100);
      } else{
        Set_Follower_Motors(0);      
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
