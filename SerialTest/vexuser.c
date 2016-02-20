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

unsigned short lfsr = 0xACE1u;
unsigned bit;

unsigned rand()
{
	bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
	return lfsr =  (lfsr >> 1) | (bit << 15);
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
	while(!chThdShouldTerminate())
	{
		`kkk`
	  vexSleep( 10 );
	}

// 	#define S_RASPI &SD3
// 	// Must call this
// 	vexTaskRegister("operator");
// //	StartTask(serialComm);
// 	static char testString[] = "HELLO\n";
// 	SerialConfig serialConf = {
// 		115200,
// 		0,
// 		0,
// 		0
// 	};
// 	sdStart(S_RASPI, &serialConf);
//
// 	int counter = 0;
// 	while(!chThdShouldTerminate())
// 	{
// 		// Write Demo
// 		//sdWrite(S_RASPI, (unsigned char *)testString, 6);
//
// 		// Read Demo
// 		if(!sdGetWouldBlock(S_RASPI)) {
// 			int c = sdGetTimeout(S_RASPI, TIME_IMMEDIATE);
// 			if(c != Q_TIMEOUT && c != Q_RESET) {
// 				vex_printf("%c", (char)c);
// 			}
// 		}
//         vexSleep( 10 );
// 	}

	return (msg_t)0;
}
