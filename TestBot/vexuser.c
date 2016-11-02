#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"
#include "../Common/easing.h"
#include "SmartMotor.h"

// Motor mappings
// Drive
#define M_DRIVE_RIGHT  kVexMotor_2
#define M_DRIVE_LEFT   kVexMotor_3


// Sensor mappings
#define P_LIFT_ENC_RIGHT_A   kVexDigital_7
#define P_LIFT_ENC_RIGHT_B   kVexDigital_8
#define P_LIFT_ENC_LEFT_A   kVexDigital_9
#define P_LIFT_ENC_LEFT_B   kVexDigital_10

#define S_LIFT_ENC_LEFT   kVexSensorDigital_7
#define S_LIFT_ENC_RIGHT  kVexSensorDigital_9



// Controller mappings
#define J_START     Btn6U
#define J_DRIVE       Ch3
#define J_TURN        Ch1


// PID Controls
EPidController *leftDrivePid;
EPidController *rightDrivePid;
EPidController *rightLiftPid;
EPidController *leftLiftPid;


//------------------Motor Configurations--------------------------------------//

static vexMotorCfg mConfig[] = {
  { M_DRIVE_RIGHT,    kVexMotor393S, kVexMotorNormal,    kVexSensorQuadEncoder,  kVexQuadEncoder_1 },
  { M_DRIVE_LEFT,     kVexMotor393S, kVexMotorReversed,    kVexSensorQuadEncoder,  kVexQuadEncoder_2 }
};

static vexDigiCfg dConfig[] = {

  
  { P_LIFT_ENC_LEFT_A,  kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_1 },
  { P_LIFT_ENC_LEFT_B,  kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_1 },
  { P_LIFT_ENC_RIGHT_A, kVexSensorQuadEncoder, kVexConfigQuadEnc1,  kVexQuadEncoder_2 },
  { P_LIFT_ENC_RIGHT_B, kVexSensorQuadEncoder, kVexConfigQuadEnc2,  kVexQuadEncoder_2 }

};

//-------------------Setup----------------------------------------------------//
void vexUserSetup()
{
    //vexDigitalPinSet(kVexDigital_5, 0);
    vexMotorConfigure( mConfig, MOT_CONFIG_SIZE( mConfig ) )  ;
    vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
}

void vexUserInit()
{
    SmartMotorsInit();
    SmartMotorPtcMonitorEnable();
    SmartMotorPtcMonitorEnable();
    SmartMotorRun();


    // SmartMotorSetRpmSensor(M_DRIVE_RIGHT, S_LIFT_ENC_RIGHT);
    // SmartMotorSetRpmSensor(M_DRIVE_LEFT, S_LIFT_ENC_LEFT);

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
  SetMotor(M_DRIVE_LEFT,  ld);
  SetMotor(M_DRIVE_RIGHT, rd);

  return (ld != 0 || rd != 0);
}



//---------------------Autonomous routine-------------------------------------//

msg_t vexAutonomous( void *arg )
{
    (void)arg;
    vexTaskRegister("auton");

    while(!chThdShouldTerminate())
    {
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
    // systime_t encoderStopTime = chTimeNow();
    // int32_t curr_drive_enc_val_right = 0;
    // int32_t curr_drive_enc_val_left = 0;
    // int32_t last_drive_enc_val_right = 0;
    // int32_t last_drive_enc_val_left = 0;

    //Clear encoders
    // clearDriveEncoders();

    while(!chThdShouldTerminate())
    {
      driveMotors();

      

    //   //Don't hog cpu
      vexSleep(10);
    }
    return (msg_t)0;
}
