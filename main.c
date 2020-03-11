#include "NU32.h"
#include <stdio.h>
#include "encoder.h"
#include "currentcontrol.h"
#include "isense.h"
// #include "utilities.h"

#define BUF_SIZE 200
#define DEG_PER_COUNT 0.9375        // scaling factor: degrees per encoder count
#define HOME_POS 32768              // encoder "home" position (counts)
#define MOTOR_DIR LATBbits.LATB15   // motor direction bit
#define CURR_M 6.212                // ADC -> current linear mapping constants
#define CURR_B 3156.602

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) CurrentControl(void) {
    // switch (mode){
    //     case 'IDLE':
    //     {
    //
    //     }
    //     case 'PWM':
    //     {
    //
    //     }
    // }
    MOTOR_DIR = !MOTOR_DIR;         // invert motor direction bit
    OC1RS = 1000;                   // 25% duty cycle

    IFS0bits.T2IF = 0;              // clear interrupt flag
}

int avg_counts(int pin){            // get average value from 3 ADC readings
    int count1, count2, count3;

    count1 = ADC_read(pin);
    count2 = ADC_read(pin);
    count3 = ADC_read(pin);

    int average = (count1 + count2 + count3)/3;

    return average;
}

int main(void){
    char buffer[BUF_SIZE];          // buffer for read/write

    NU32_Startup();                 // initialize NU32 board

    NU32_LED1 = 1;                  // turn off onboard LEDs
    NU32_LED2 = 1;

    __builtin_disable_interrupts();     // disable interrupts at CPU
    encoder_init();                     // initialize SPI4
    ADC_init();                         // initialize ADC with input AN0
    currentcontrol_init();              // initialize current control timers & output compare
    __builtin_enable_interrupts();      // re-enable interrupts at CPU

    while(1){
        NU32_ReadUART3(buffer, BUF_SIZE);
        NU32_LED2 = 1;                      // clear error LED

        switch (buffer[0]){
            case 'a':                       // read current sensor (ADC counts)
            {
                sprintf(buffer, "%d\r\n", avg_counts(16));      // print average count to string
                NU32_WriteUART3(buffer);                        // serial write
                break;
            }
            case 'b':                       // get current value in milliamps (mA)
            {
                int current_count = avg_counts(16);                 // current value (ADC)
                float current_ma = CURR_M*current_count - CURR_B;   // current value (mA)
                sprintf(buffer, "%6.2f\r\n", current_ma);           // print to string
                NU32_WriteUART3(buffer);
                break;
            }
            case 'c':                       // reads the encoder count
            {
                sprintf(buffer, "%d\r\n", encoder_counts());    // print count to string
                NU32_WriteUART3(buffer);                        // serial write
                break;
            }
            case 'd':                       // read encoder count in degrees
            {
                int count = encoder_counts() - HOME_POS;    // number of counts away from "home"
                float deg = count*DEG_PER_COUNT;            // convert counts -> degrees
                sprintf(buffer, "%6.2f\r\n", deg);          // print to string
                NU32_WriteUART3(buffer);                    // serial write
                break;
            }
            case 'e':                       // resets the encoder count
            {
                encoder_reset();
                break;
            }
            case 'q':                       // quit (NEEDS TO BE ALTERED)
            {
                break;
            }
            case 'r':                       // get mode
            {
                ;
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
