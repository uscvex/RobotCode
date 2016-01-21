#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "pidlib.h"
#include "robotc_glue.h"
#include "../Common/common.h"

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

//msg_t
//serialComm(void *arg) {
//	(void)arg;
//	static char testString[] = "HELLO\n";
//	SerialConfig serialConf = {
//		115200,
//		0,
//		0,
//		0
//	};
//	sdStart(&SD1, &serialConf);
//	while(chThdShouldTerminate()) {
//		sdWrite(&SD1, (unsigned char *)testString, 6);
//		vexSleep(10);
//	}
//	return (msg_t)0;
//}

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
//	StartTask(serialComm);
//	static char testString[] = "HELLO\n";
//	SerialConfig serialConf = {
//		115200,
//		0,
//		0,
//		0
//	};
//	sdStart(&SD1, &serialConf);

	while(!chThdShouldTerminate())
	{
		vex_printf("okay\n");
//		sdWrite(&SD1, (unsigned char *)testString, 6);
        vexSleep( 10 );
	}

	return (msg_t)0;
}
