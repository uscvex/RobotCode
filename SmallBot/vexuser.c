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
#include "../Common/common.h"

// Digi IO configuration
static  vexDigiCfg  dConfig[kVexDigital_Num] = {
        { kVexDigital_1,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc1,    kVexQuadEncoder_1 }, // bottom
        { kVexDigital_2,    kVexSensorQuadEncoder ,  kVexConfigQuadEnc2,    kVexQuadEncoder_1 },
        { kVexDigital_3,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
        { kVexDigital_4,    kVexSensorDigitalInput,  kVexConfigInput,       0 },
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
        { kVexMotor_1,      kVexMotorUndefined,      kVexMotorNormal,     kVexSensorNone,        0 },
        { kVexMotor_2,      kVexMotor393S,           kVexMotorNormal,     kVexSensorIME,         kImeChannel_1 },
        { kVexMotor_3,      kVexMotor393S,           kVexMotorNormal,     kVexSensorIME,         kImeChannel_2 },
        { kVexMotor_4,      kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
        { kVexMotor_5,      kVexMotor393T,           kVexMotorReversed,   kVexSensorQuadEncoder, kVexQuadEncoder_1 },
        { kVexMotor_6,      kVexMotorUndefined,      kVexMotorNormal,     kVexSensorNone,        0 },
        { kVexMotor_7,      kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
        { kVexMotor_8,      kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
        { kVexMotor_9,      kVexMotor393S,           kVexMotorNormal,     kVexSensorNone,        0 },
        { kVexMotor_10,     kVexMotorUndefined,      kVexMotorNormal,     kVexSensorNone,        0 }
};

#define M_DRIVE_LEFT1     kVexMotor_2
#define M_DRIVE_LEFT2     kVexMotor_8
#define M_DRIVE_RIGHT1    kVexMotor_3
#define M_DRIVE_RIGHT2    kVexMotor_9
#define M_FRONT_FEED      kVexMotor_7
#define M_SHOOT_FEED      kVexMotor_4
#define M_FLY_WHEEL       kVexMotor_5

#define S_ENC_FLY         kVexQuadEncoder_1

#define J_DRIVE Ch3
#define J_TURN  Ch1
#define J_SHOOT Btn6U
#define J_FEED_SHOOT Btn6D
#define J_FEED_U  Btn5U
#define J_FEED_D  Btn5D

#define FEED_SPOOL_TIME 100

pidController *pidcFly;

void driveMotors(void) {
    short ld, rd ;
    //Calculate Motor Power
    int forward = CLAMPMOTOR(vexControllerGet(J_DRIVE), 45);
    int turn = CLAMPMOTOR(vexControllerGet(J_TURN), 45);
    ld = forward + turn;
    rd = forward - turn;

    vexMotorSet(M_DRIVE_LEFT1, CLAMPMOTOR(ld, 45));
    vexMotorSet(M_DRIVE_LEFT2, CLAMPMOTOR(ld, 45));
    vexMotorSet(M_DRIVE_RIGHT1, CLAMPMOTOR(rd, 45));
    vexMotorSet(M_DRIVE_RIGHT2, CLAMPMOTOR(rd, 45));
}


msg_t flyWheelTask(void *arg) {
    (void)arg;

    vexTaskRegister("flyWheelTask");

    PidControllerMakeLut();

    // initialize PID COntroller
    pidcFly = PidControllerInit(0.5, 0, 0, S_ENC_FLY, true);

    vexSensorValueSet(S_ENC_FLY, 0);

    int step = 0;
    while(!chThdShouldTerminate()) {
        if(vexControllerGet(J_SHOOT)) {
            if(!pidcFly->enabled) {
                pidcFly->enabled = true;
                pidcFly->target_value = 0;
                vexSensorValueSet(S_ENC_FLY, 0);
            } else if(step <= 0) {
                pidcFly->target_value = 700;
                step = 0;
            }
        } else {
            step = 0;
            pidcFly->enabled = false;
        }
        PidControllerUpdate(pidcFly);
        vexMotorSet(M_FLY_WHEEL, pidcFly->drive_cmd);

        step--;
        vexSleep(25);
    }

    return (msg_t)0;
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
    //int feedSpoolCounter = 0;
	while(!chThdShouldTerminate())
	{
        driveMotors();
        //vexMotorSet()
        vexMotorSet(M_FRONT_FEED, vexControllerGet(J_FEED_U)?100:0);
        vexMotorSet(M_FRONT_FEED, vexControllerGet(J_FEED_D)?-100:0);
        vexMotorSet(M_SHOOT_FEED, vexControllerGet(J_FEED_SHOOT)?127:0);

/*        if(vexControllerGet(J_SHOOT)) {
            vexMotorSet(M_FLY_WHEEL, 100);
            if(feedSpoolCounter > FEED_SPOOL_TIME) {
                vexMotorSet(M_SHOOT_FEED, 100);
            } else {
                feedSpoolCounter++;
            }
        } else {
            vexMotorSet(M_FLY_WHEEL, 0);
            feedSpoolCounter = 0;
            vexMotorSet(M_SHOOT_FEED, 0);
        }
*/
        vexSleep( 25 );
	}

	return (msg_t)0;
}
