#include "encoder.h"
#include <xc.h>

static int encoder_command(int read){
    SPI4BUF = read;                     // send command
    while (!SPI4STATbits.SPIRBF){;}     // wait for response
    SPI4BUF;                            // read buffer to clear it
    SPI4BUF = 5;                        // write to buffer to enable read ability
    while (!SPI4STATbits.SPIRBF){;}
    return SPI4BUF;                     // return count in buffer
}

int encoder_counts(void) {          // reads encoder count
    return encoder_command(1);
}

void encoder_reset(void) {           // resets count to 32768
    encoder_command(0);
}

void encoder_init(void){
    SPI4CON = 0;                    // stop/reset SPI4
    SPI4BUF;                        // clear RX receive buffer
    SPI4BRG = 0x4;                  // bit rate = 8 MHz
    SPI4STATbits.SPIROV = 0;        // clear overflow
    SPI4CONbits.MSTEN = 1;          // master mode
    SPI4CONbits.MSSEN = 1;          // enable slave select
    SPI4CONbits.MODE16 = 1;         // 16-bit mode
    SPI4CONbits.MODE32 = 0;
    SPI4CONbits.SMP = 1;            // sample at end of clock
    SPI4CONbits.ON = 1;             // turn on SPI
}
