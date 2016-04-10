#include "ch.h"
#include "hal.h"
#include "vex.h"
#include "mousedeadreck.h"

static DeadReck deadRecks[MAXDEADRECK];
static int currDeadReck = 0;

void copyDouble(uint8_t *src, double *value) {
    uint8_t *dest = (uint8_t*)(value);
    uint16_t i;
    for(i = 0;i < sizeof(double);i++) {
        *dest = *src;
        dest++;
        src++;
    }
}

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

int16_t deadReckRead(DeadReck *dreck) {
    int16_t retVal = 0;
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
                    dreck->readState = RDST_STUFF_START;
                } else if(readValue == END_BYTE) {
                    uint8_t checkSum = 0;
                    int i;
                    for(i = 1;i < dreck->bufLen;i++) {
                        checkSum ^= dreck->buffer[i];
                    }
                    if(checkSum == 0) {
                        retVal = dreck->buffer[2];
                    } else {
                        dreck->bufLen = 0;
                        dreck->readState = RDST_SEARCHING;
                    }
                }
                else {
                    dreck->buffer[dreck->bufLen++] = readValue;
                }
                break;
            case RDST_STUFF_START: // found byte stuffing
                if(readValue != START_BYTE || readValue != END_BYTE) {
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
    }
    return retVal;
}

/**
 * Sends a clear command and waits till a
 * clear acknowledgement is received
 */
void deadReckClear(DeadReck *dreck, systime_t timeout) {
    systime_t lastTime = 0;
    const unsigned char clearPacket[] = {
        START_BYTE,
        PCKT_TYPE_CLEAR, // checksum
        PCKT_TYPE_CLEAR,
        END_BYTE
    };
	while(!chThdShouldTerminate()) {
        systime_t currentTime = chTimeNow();
        if(lastTime == 0 || (currentTime - lastTime) > timeout) {
            lastTime = currentTime;
            sdWrite(dreck->driver, clearPacket, sizeof(clearPacket));
        }
        int16_t code = deadReckRead(dreck);
        if(code) {
            dreck->bufLen = 0;
            dreck->readState = RDST_SEARCHING;
            if(code == PCKT_TYPE_CLEAR_ACK) {
                return;
            }
        }
    }
}

/**
 * Reads serial port and updates dead reckoning values.
 * Call once in the beginning of main loop.
 */
void deadReckUpdate(DeadReck *dreck) {
    int16_t code = deadReckRead(dreck);
    if(code) {
        if(code == PCKT_TYPE_UPDATE) {
            // copy from buffer to double
            // we do this because ARM cannot read byte
            // misaligned memory locations
            copyDouble(&(dreck->buffer[3]), &(dreck->botX));
            copyDouble(&(dreck->buffer[11]), &(dreck->botY));
            copyDouble(&(dreck->buffer[19]), &(dreck->botTheta));
        }
        dreck->bufLen = 0;
        dreck->readState = RDST_SEARCHING;
    }
}
