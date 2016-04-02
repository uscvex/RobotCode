#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "mousedeadreck.h"

static DeadReck deadRecks[MAXDEADRECK];
static int currDeadReck = 0;

#define RDST_SEARCHING 0
#define RDST_FILL  1
#define RDST_STUFF 2

/**
 * Initialized a DeadReckoning device
 * driver - the serial driver to be used. typically &SD2 or &SD2
 * baud - baud rate
 */
DeadReck *deadReckInit(SerialDriver *driver, int baud) {
    DeadReck *dreck = &(deadRecks[currDeadReck++]);
    dreck->driver = driver;
    dreck->baud = baud;
    dreck->botX = 0;
    dreck->botY = 0;
    dreck->botTheta = 0;
    dreck->readState = RDST_SEARCHING;
    dreck->bufLen = 0;
    return dreck;
}

/**
 * Config and start serial communication for deadreckoning device
 */
void deadReckStart(DeadReck *dreck) {
    SerialConfig serialConf = {
        dreck->baud,
        0,0,0
    };
    sdStart(dreck->driver, &serialConf);
}

/**
 * Stops serial communication with deadreckoning device
 */
void deadReckStop(DeadReck *dreck) {
    sdStop(dreck->driver);
}

/**
 * Reads serial port and updates dead reckoning values.
 * Call once in the beginning of main loop.
 */
void deadReckUpdate(DeadReck *dreck) {
    while(!sdGetWouldBlock(dreck->driver)) {
        uint8_t readValue = sdGetTimeout(dreck->driver, TIME_IMMEDIATE);
        switch(dreck->readState) {
            case RDST_SEARCHING: // searching for start byte
                if(readValue == START_BYTE) {
                    dreck->readState = RDST_FILL;
                    dreck->buffer[dreck->bufLen++] = readValue;
                }
                break;
            case RDST_FILL: // filling packet bytes
                if(readValue == START_BYTE) {
                    dreck->readState = RDST_STUFF;
                }
                else {
                    dreck->buffer[dreck->bufLen++] = readValue;
                }
                break;
            case RDST_STUFF: // found byte stuffing
                if(readValue != START_BYTE) {
                    // we may have somehow reached beginning of
                    // another packet. discard previous packet and
                    // continue reading
                    dreck->buffer[0] = START_BYTE;
                    dreck->buffer[1] = readValue;
                    dreck->bufLen = 2;
                }
                else {
                    dreck->buffer[dreck->bufLen++] = readValue;
                }
                dreck->readState = RDST_FILL;
                break;
        }
        if(dreck->bufLen == DRECK_PACKET_SIZE) {
            // finished reading a full packet
            if(dreck->readState == RDST_FILL) {
                uint8_t checkSum = 0;
                int i;
                for(i = 1;i < DRECK_PACKET_SIZE;i++) {
                    checkSum ^= dreck->buffer[i];
                }
                if(checkSum == 0) {
                    double *values = (double*)(&(dreck->buffer[2]));
                    dreck->botX = values[0];
                    dreck->botY = values[1];
                    dreck->botTheta = values[2];
                }
            } // else case shouldn't happen
            dreck->bufLen = 0;
            dreck->readState = RDST_SEARCHING;
        }
    }
}
