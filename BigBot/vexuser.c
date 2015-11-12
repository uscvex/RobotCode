/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*                        Copyright (c) James Pearman                          */
/*                                   2013                                      */
/*                            All Rights Reserved                              */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    Module:     vexuser.c                                                    */
/*    Author:     James Pearman                                                */
/*    Created:    7 May 2013                                                   */
/*                                                                             */
/*    Revisions:                                                               */
/*                V1.00  04 July 2013 - Initial release                        */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    The author is supplying this software for use with the VEX cortex        */
/*    control system. This file can be freely distributed and teams are        */
/*    authorized to freely use this program , however, it is requested that    */
/*    improvements or additions be shared with the Vex community via the vex   */
/*    forum.  Please acknowledge the work of the authors when appropriate.     */
/*    Thanks.                                                                  */
/*                                                                             */
/*    Licensed under the Apache License, Version 2.0 (the "License");          */
/*    you may not use this file except in compliance with the License.         */
/*    You may obtain a copy of the License at                                  */
/*                                                                             */
/*      http://www.apache.org/licenses/LICENSE-2.0                             */
/*                                                                             */
/*    Unless required by applicable law or agreed to in writing, software      */
/*    distributed under the License is distributed on an "AS IS" BASIS,        */
/*    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. */
/*    See the License for the specific language governing permissions and      */
/*    limitations under the License.                                           */
/*                                                                             */
/*    The author can be contacted on the vex forums as jpearman                */
/*    or electronic mail using jbpearman_at_mac_dot_com                        */
/*    Mentor for team 8888 RoboLancers, Pasadena CA.                           */
/*                                                                             */
/*-----------------------------------------------------------------------------*/

#include <stdlib.h>

#include "ch.h"  		// needs for all ChibiOS programs
#include "hal.h" 		// hardware abstraction layer header
#include "vex.h"		// vex library header
#include "pidlib.h"
#include "robotc_glue.h"

// Digi IO configuration
static  vexDigiCfg  dConfig[kVexDigital_Num] = {
        { kVexDigital_1,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_1 }, // bottom
        { kVexDigital_2,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_1 },

        { kVexDigital_3,    kVexSensorQuadEncoder,   kVexConfigQuadEnc1,    kVexQuadEncoder_2 }, // top
        { kVexDigital_4,    kVexSensorQuadEncoder,   kVexConfigQuadEnc2,    kVexQuadEncoder_2 },

        { kVexDigital_5,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_6,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_7,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_8,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_9,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_10,   kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_11,   kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_12,   kVexSensorDigitalInput,  kVexConfigInput,       0 }
};

static  vexMotorCfg mConfig[kVexMotorNum] = {
        { kVexMotor_1,      kVexMotorUndefined,      kVexMotorNormal,       kVexSensorNone,        0 },
        { kVexMotor_2,      kVexMotor393T,           kVexMotorNormal,       kVexSensorNone,        0 },
        { kVexMotor_3,      kVexMotor393T,           kVexMotorNormal,       kVexSensorNone,        0 },
        { kVexMotor_4,      kVexMotor393T,           kVexMotorNormal,       kVexSensorNone,        0 },
        { kVexMotor_5,      kVexMotor393S,           kVexMotorReversed,       kVexSensorQuadEncoder, kVexQuadEncoder_2 },
        { kVexMotor_6,      kVexMotor393S,           kVexMotorReversed,       kVexSensorQuadEncoder, kVexQuadEncoder_1 },
        { kVexMotor_7,      kVexMotor393T,           kVexMotorReversed,       kVexSensorNone,        0 },
        { kVexMotor_8,      kVexMotor393T,           kVexMotorReversed,     kVexSensorNone,        0 },
        { kVexMotor_9,      kVexMotor393T,           kVexMotorReversed,       kVexSensorNone,        0 },
        { kVexMotor_10,     kVexMotorUndefined,      kVexMotorNormal,       kVexSensorNone,        0 }
};

#define M_FEED_FRONT         kVexMotor_2
#define M_DRIVE_FRONT_LEFT   kVexMotor_3
#define M_DRIVE_BACK_LEFT    kVexMotor_4
#define M_FLY_TOP            kVexMotor_5
#define M_FLY_BOT            kVexMotor_6
#define M_DRIVE_BACK_RIGHT   kVexMotor_7
#define M_FEED_SHOOT         kVexMotor_8
#define M_DRIVE_FRONT_RIGHT  kVexMotor_9

#define S_ENC_FLY_TOP kVexQuadEncoder_2
#define S_ENC_FLY_BOT kVexQuadEncoder_1

#define J_SHOOT Btn8R
#define J_FEED  Btn8L

#define FEED_SPOOL_TIME 100

pidController *pidcFlyTop;
pidController *pidcFlyBot;

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define ABS(v) (((v) < 0)?(-(v)):(v))
#define SIGN(v) ((v)<0?-1:1)
#define CLAMP(v, lower, upper) (MIN(MAX(ABS(v), (lower)), upper)*SIGN(v))
#define CLAMPMOTOR(v, lower) (ABS(v) < lower?0:(v))

void doMotorDrive(void) {
    int C1LX = CLAMPMOTOR(vexControllerGet(Ch4), 45);
    int C1LY = CLAMPMOTOR(vexControllerGet(Ch3), 45);
    int C1RX = CLAMPMOTOR(vexControllerGet(Ch1), 45);

    // Y component, X component, Rotation
    vexMotorSet(M_DRIVE_FRONT_LEFT,   CLAMPMOTOR(-C1LY - C1LX + C1RX, 45));
    vexMotorSet(M_DRIVE_FRONT_RIGHT,  CLAMPMOTOR(C1LY  - C1LX + C1RX,  45));
    vexMotorSet(M_DRIVE_BACK_RIGHT,   CLAMPMOTOR(C1LY  - C1LX - C1RX,  45));
    vexMotorSet(M_DRIVE_BACK_LEFT,    CLAMPMOTOR(-C1LY - C1LX - C1RX, 45));
}

/*-----------------------------------------------------------------------------*/
/** @brief      User setup                                                     */
/*-----------------------------------------------------------------------------*/
/** @details
 *  The digital and motor ports can (should) be configured here.
 */
void
vexUserSetup()
{
	vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
	vexMotorConfigure( mConfig, MOT_CONFIG_SIZE( mConfig ) );
}

/*-----------------------------------------------------------------------------*/
/** @brief      User initialize                                                */
/*-----------------------------------------------------------------------------*/
/** @details
 *  This function is called after all setup is complete and communication has
 *  been established with the master processor.
 *  Start other tasks and initialize user variables here
 */
void
vexUserInit()
{
}

/*-----------------------------------------------------------------------------*/
/** @brief      Autonomous                                                     */
/*-----------------------------------------------------------------------------*/
/** @details
 *  This thread is started when the autonomous period is started
 */
msg_t
vexAutonomous( void *arg )
{
    (void)arg;

    // Must call this
    vexTaskRegister("auton");

    while(1)
        {
        // Don't hog cpu
        vexSleep( 25 );
        }

    return (msg_t)0;
}

msg_t flyWheelTask(void *arg) {
    (void)arg;

    vexTaskRegister("flyWheelTask");

    PidControllerMakeLut();

    // initialize PID COntroller
    pidcFlyTop = PidControllerInit(0.5, 0, 0, S_ENC_FLY_TOP, true);
    pidcFlyBot = PidControllerInit(0.5, 0, 0, S_ENC_FLY_BOT, true);

    vexSensorValueSet(S_ENC_FLY_BOT, 0);
    vexSensorValueSet(S_ENC_FLY_TOP, 0);

    int step = 0;
    while(!chThdShouldTerminate()) {
        if(vexControllerGet(J_SHOOT)) {
            if(!pidcFlyTop->enabled) {
                pidcFlyTop->enabled = true;
                pidcFlyBot->enabled = true;
                pidcFlyBot->target_value = 0;
                pidcFlyTop->target_value = 0;
                vexSensorValueSet(S_ENC_FLY_BOT, 0);
                vexSensorValueSet(S_ENC_FLY_TOP, 0);
            } else if(step <= 0) {
                //vexSensorValueSet(S_ENC_FLY_BOT, 0);
                //vexSensorValueSet(S_ENC_FLY_TOP, 0);
                pidcFlyBot->target_value = 600;
                pidcFlyTop->target_value = 600;
                step = 0;
            }
        } else {
            step = 0;
            pidcFlyTop->enabled = false;
            pidcFlyBot->enabled = false;
        }
        //vex_printf("V=%d E=%f C=%d R=%d, ", pidcFlyTop->sensor_value, pidcFlyTop->error, pidcFlyTop->drive_cmd, pidcFlyTop->drive_raw);
        //vex_printf("V=%d E=%f C=%d R=%d\n", pidcFlyBot->sensor_value, pidcFlyBot->error, pidcFlyBot->drive_cmd, pidcFlyBot->drive_raw);
        PidControllerUpdate(pidcFlyTop);
        PidControllerUpdate(pidcFlyBot);
        vexMotorSet(M_FLY_TOP, pidcFlyTop->drive_cmd);
        vexMotorSet(M_FLY_BOT, pidcFlyBot->drive_cmd);

        step--;
        vexSleep(25);
    }

    return (msg_t)0;
}


/*-----------------------------------------------------------------------------*/
/** @brief      Driver control                                                 */
/*-----------------------------------------------------------------------------*/
/** @details
 *  This thread is started when the driver control period is started
 */
msg_t
vexOperator( void *arg )
{
	(void)arg;

	// Must call this
	vexTaskRegister("operator");

    StartTask(flyWheelTask);

	// Run until asked to terminate
    int feedSpoolCounter = 0;
	while(!chThdShouldTerminate())
	{
        doMotorDrive();
        vexMotorSet(M_FEED_FRONT, vexControllerGet(J_FEED)?100:0);
        if(vexControllerGet(J_SHOOT)) {
            if(feedSpoolCounter > FEED_SPOOL_TIME) {
                vexMotorSet(M_FEED_SHOOT, 100);
                vexMotorSet(M_FEED_FRONT, 100);
            } else {
                feedSpoolCounter++;
            }
        } else {
            feedSpoolCounter = 0;
            vexMotorSet(M_FEED_SHOOT, 0);
        }
        vexSleep( 25 );
	}

	return (msg_t)0;
}
