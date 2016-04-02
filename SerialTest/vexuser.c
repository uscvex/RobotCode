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

	while(!chThdShouldTerminate())
	{
        deadReckUpdate(dreck);
        vex_printf("botX = %f, botY = %f, botTheta = %f\n", dreck->botX, dreck->botY, dreck->botTheta);
		// Write Demo
        vexSleep( 10 );
	}

	return (msg_t)0;
}
