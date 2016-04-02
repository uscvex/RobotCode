#ifndef __MOUSEDEADRECK_H__
#define __MOUSEDEADRECK_H__

#include "ch.h"
#include "hal.h"
#include "vex.h"

#define START_BYTE 0xB0
#define DRECK_PACKET_SIZE 25
#define MAXDEADRECK 2

typedef struct _DeadReck {
    SerialDriver *driver;
    int baud;
    double botX;
    double botY;
    double botTheta;
    int readState;
    uint8_t buffer[DRECK_PACKET_SIZE];
    int bufLen;
} DeadReck;


DeadReck *deadReckInit(SerialDriver *driver, int baud);
void deadReckStart(DeadReck *dreck);
void deadReckStop(DeadReck *dreck);
void deadReckUpdate(DeadReck *dreck);

#endif
