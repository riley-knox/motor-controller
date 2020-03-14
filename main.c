#include "NU32.h"
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "encoder.h"
#include "currentcontrol.h"
#include "isense.h"
#include "utilities.h"

#define MOTOR_DIR LATBbits.LATB15
#define BUF_SIZE 200
#define DEG_PER_COUNT 0.9375        // scaling factor: degrees per encoder count
#define HOME_POS 32768              // encoder "home" position (counts)
#define CURR_M 6.212                // ADC -> current linear mapping constants
#define CURR_B 3156.602

volatile int f;                         // duty cycle percentage
static volatile int eint_current;       // current control error integral
static volatile int StoringData = 0;    // "storing plot data" flag

int avg_counts(int pin){            // get average value from 3 ADC readings
    int count1, count2, count3;

    count1 = ADC_read(pin);
    count2 = ADC_read(pin);
    count3 = ADC_read(pin);

    int average = (count1 + count2 + count3)/3;

    return average;
}

float counts_to_current(int count) {
    float amps = CURR_M*count - CURR_B;
    return amps;
}

void __ISR(_TIMER_2_VECTOR, IPL5SOFT) CurrentControl(void) {
    int m = get_mode();

    static float reference_current = 200;       // reference current for ITEST
    static float actual_current = 0;            // measured current
    static int i;                               // number of samples processed in ITEST

    switch (m) {
        case IDLE:
        {
            OC1RS = 0;                      // no PWM
            MOTOR_DIR = 0;                  // forward (not relevant bc no PWM, but must be set)
            break;
        }
        case PWM:
        {
            if (f >= 0) {
                MOTOR_DIR = 0;              // forward direction
                OC1RS = ((float)f/100)*PERIOD_REG;
            }

            else if (f < 0) {
                MOTOR_DIR = 1;              // reverse direction
                OC1RS = (-(float)f/100)*PERIOD_REG;
            }
            break;
        }
        case ITEST:
        {
            if (i != 0 && i % 25 == 0) {     // flip reference current every 25 samples
                reference_current *= -1;
            }

            if (i < NUM_SAMPS) {
                actual_current = counts_to_current(avg_counts(16));  // measure current

                current_refs[i] = (int)reference_current;        // store reference and measured currents
                current_actuals[i] = (int)actual_current;

                // PI control
                float err = reference_current - actual_current;                     // error
                eint_current = eint_current + err;                                  // update error integral
                float control = curr_gains.Kp*err + curr_gains.Ki*eint_current;     // update control

                // calculate new PWM value
                float control_PWM = ((control/1000)/3.3)*100;   // count --> percentage

                if (control_PWM > 100.0) {                      // stay within interval -100:100
                    control_PWM = 100.0;
                }
                else if (control_PWM < -100.0) {
                    control_PWM = -100.0;
                }

                // set new PWM value
                if (control_PWM < 0) {
                    MOTOR_DIR = 1;                  // reverse direction if control signal negative
                    OC1RS = ((-control_PWM/100.0)*PERIOD_REG);
                }
                else {
                    MOTOR_DIR = 0;                  // positive direction
                    OC1RS = ((control_PWM/100.0)*PERIOD_REG);
                }

                i++;
            }
            else {
                StoringData = 0;            // reset everything
                i = 0;
                reference_current = 200;
                set_mode(0);
            }
            break;
        }
        // case HOLD:
        // {}
        // case TRACK:
        // {}
    }

    IFS0bits.T2IF = 0;              // clear interrupt flag
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
    set_mode(0);                        // set mode to IDLE on startup
    set_currentgains(0.0, 0.0);         // initialize Kp & Ki for current control
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
                sprintf(buffer, "%6.2f\r\n", counts_to_current(current_count));           // print to string
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
            case 'f':                       // set PWM frequency
            {
                NU32_ReadUART3(buffer, BUF_SIZE);   // read serial port for user input
                sscanf(buffer, "%d", &f);           // store input
                sprintf(buffer, "%d\r\n", f);
                NU32_WriteUART3(buffer);
                set_mode(1);                        // switch to PWM operating mode
                break;
            }
            case 'g':                       // set current gains`
            {
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%f", &curr_gains.Kp);
                NU32_ReadUART3(buffer, BUF_SIZE);
                sscanf(buffer, "%f", &curr_gains.Ki);
                break;
            }
            case 'h':                       // get current gains
            {
                // gain_struct current_gains = get_currentgains();
                sprintf(buffer, "%f\r\n", curr_gains.Kp);
                NU32_WriteUART3(buffer);
                sprintf(buffer, "%f\r\n", curr_gains.Ki);
                NU32_WriteUART3(buffer);
                break;
            }
            case 'i':
            {
                ;
            }
            case 'j':
            {
                ;
            }
            case 'k':                       // test current control
            {
                eint_current = 0;               // reset current error integral
                StoringData = 1;                // initialize data storage
                set_mode(2);                    // switch to ITEST operating mode
                while (StoringData) { ; }       // store data, wait until done
                // send client number of samples to plot
                sprintf(buffer, "%d\r\n", NUM_SAMPS);
                NU32_WriteUART3(buffer);
                // send data points to client
                __builtin_disable_interrupts();
                int a;
                for (a = 0; a < NUM_SAMPS; a++) {
                    sprintf(buffer, "%d %d\r\n", current_refs[a], current_actuals[a]);   // send reference:measured pair
                    NU32_WriteUART3(buffer);
                }
                __builtin_enable_interrupts();
                break;
            }
            case 'l':
            {
                ;
            }
            case 'm':
            {
                ;
            }
            case 'n':
            {
                ;
            }
            case 'o':
            {
                ;
            }
            case 'p':                       // motor power off
            {
                set_mode(0);
                break;
            }
            case 'q':                       // quit
            {
                set_mode(0);                // turn off motor power before exiting
                break;
            }
            case 'r':                       // get mode
            {
                int m = get_mode();
                sprintf(buffer, "%d\r\n", m);
                NU32_WriteUART3(buffer);
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
