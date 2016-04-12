#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "math.h"
#include "../Common/common.h"
#include "../Common/mousedeadreck.h"

// Digi IO configuration
static  vexDigiCfg  dConfig[] = {
};

// Motor Config
static  vexMotorCfg mConfig[] = {
};

void
vexUserSetup()
{
	vexDigitalConfigure( dConfig, DIG_CONFIG_SIZE( dConfig ) );
	vexMotorConfigure( mConfig, MOT_CONFIG_SIZE( mConfig ) );
}

void
vexUserInit()
{
}

msg_t
vexAutonomous( void *arg )
{
    (void)arg;
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

    DeadReck *dreck = deadReckInit(&SD3, 115200);
    deadReckStart(dreck);
 	/* SerialConfig serialConf = { */
 	/* 	115200, */
 	/* 	0, */
 	/* 	0, */
 	/* 	0 */
 	/* }; */
    /* #define S_RASPI &SD3 */
 	/* sdStart(S_RASPI, &serialConf); */

	while(!chThdShouldTerminate())
	{
        if(vexControllerGet(Btn8U)) {
            vex_printf("Clearing...\n");
            deadReckClear(dreck, 1000);
            vex_printf("Clear ACK Received\n");
        }
        deadReckUpdate(dreck);

 		// Read Demo
		/* if(!sdGetWouldBlock(S_RASPI)) { */
		/* 	int c = sdGetTimeout(S_RASPI, TIME_IMMEDIATE); */
		/* 	if(c != Q_TIMEOUT && c != Q_RESET) { */
		/* 		vex_printf("%c", (char)c); */
		/* 	} */
		/* } */
        vexSleep( 10 );
	}

	return (msg_t)0;
}
