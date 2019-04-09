#ifndef __MOUSEDEADRECK_H__
#define __MOUSEDEADRECK_H__

#include "ch.h"
#include "hal.h"
#include "vex.h"

#define START_BYTE 0xB0
#define END_BYTE 0xBE

#define MAX_BUFFER_SIZE 127
#define MAXDEADRECK 2

#define PCKT_TYPE_UPDATE    0xA0
#define PCKT_TYPE_CLEAR     0xA1
#define PCKT_TYPE_CLEAR_ACK 0xA2

#define RDST_SEARCHING 0
#define RDST_FILL  1
#define RDST_STUFF_START 2

typedef struct _DeadReck {
    SerialDriver *driver;
    int baud;
    double botX;
    double botY;
    double botTheta;
    int readState;
    uint8_t buffer[MAX_BUFFER_SIZE];
    int bufLen;
    bool log;
} DeadReck;


DeadReck *deadReckInit(SerialDriver *driver, int baud);
void deadReckStart(DeadReck *dreck);
void deadReckStop(DeadReck *dreck);
void deadReckClear(DeadReck *dreck, systime_t timeout);
void deadReckUpdate(DeadReck *dreck);

#endif
