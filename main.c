#include "NU32.h"

#define BUF_SIZE 200

int main(void){
    char buffer[BUF_SIZE];

    NU32_Startup();                 // initialize NU32 board

    NU32_LED1 = 1;                  // turn off onboard LEDs
    NU32_LED2 = 1;

    __builtin_disable_interrupts();     // disable interrupts at CPU

    __builtin_enable_interrupts();      // re-enable interrupts at CPU

    while(1){
        NU32_ReadUART3(buffer, BUF_SIZE);
        NU32_LED2 = 1;                      // clear error LED

        switch (buffer[0]){
            // case 'a': {
                //
            // }

            default:{
                NU32_LED2 = 0;              // turn on error LED
                break;
            }
        }
    }

    return 0;
}
