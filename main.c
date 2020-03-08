#include "NU32.h"
#include <stdio.h>
#include "encoder.h"

#define BUF_SIZE 200

int main(void){
    char buffer[BUF_SIZE];

    NU32_Startup();                 // initialize NU32 board

    NU32_LED1 = 1;                  // turn off onboard LEDs
    NU32_LED2 = 1;

    __builtin_disable_interrupts();     // disable interrupts at CPU
    encoder_init();                     // initialize SPI4
    __builtin_enable_interrupts();      // re-enable interrupts at CPU

    while(1){
        NU32_ReadUART3(buffer, BUF_SIZE);
        NU32_LED2 = 1;                      // clear error LED

        switch (buffer[0]){
            case 'c':                       // reads the encoder count
            {
                // int ecount = encoder_counts();      // get encoder count
                sprintf(buffer, "%d\r\n", encoder_counts());  // print count to string
                NU32_WriteUART3(buffer);            // serial write
                break;
            }
            case 'd':                       // dummy function
            {
                int n = 0;
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%d", &n);
                sprintf(buffer, "%d\r\n", n+1);
                NU32_WriteUART3(buffer);
                break;
            }
            case 'e':                       // resets the encoder count
            {
                encoder_reset();
                break;
            }
            case 'q':                       // quit
            {
                break;
            }
            case 'x':                       // sum function
            {
                int m = 0, n = 0;                   // entry variables
                NU32_ReadUART3(buffer, BUF_SIZE);   // read via serial port
                sscanf(buffer, "%d", &m);           // store entry as first number
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%d", &n);           // store entry as second number
                sprintf(buffer, "%d\r\n", m+n);     // print sum to string
                NU32_WriteUART3(buffer);            // write sum as output
                break;
            }
            default:
            {
                NU32_LED2 = 0;              // turn on error LED
                break;
            }
        }
    }

    return 0;
}
