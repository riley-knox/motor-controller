#include <xc.h>
#include "currentcontrol.h"

void currentcontrol_init(void) {
    AD1PCFGbits.PCFG15 = 1;     // set pin B15 as digital pin
    TRISBbits.TRISB15 = 0;      // set pin B15 as digital output

    // Timer2 (5 kHz)
    T2CONbits.TCKPS = 0;        // Timer2 prescale = 1
    PR2 = 15999;                // set PR2 to 16000 - 1 for 5 kHz
    TMR2 = 0;
    T2CONbits.ON = 1;           // enable Timer2
    IPC2bits.T2IP = 5;          // Timer2 interrupt priority = 5
    IPC2bits.T2IS = 0;          // Timer2 interrupt subpriority = 0
    IFS0bits.T2IF = 0;          // clear Timer2 interrupt flag
    IEC0bits.T2IE = 1;          // enable Timer2 interrupt

    // Timer3 (20 kHz)
    T3CONbits.TCKPS = 0;        // Timer3 prescale = 1
    PR3 = PERIOD_REG;           // set PR3 to 4000 - 1 for 20 kHz
    TMR3 = 0;
    OC1CONbits.OCTSEL = 1;      // use Timer3 as clock source for OC1
    OC1CONbits.OCM = 0b110;     // OC1 in PWM mode, no fault pin
    OC1RS = 1000;
    OC1R = 1000;
    T3CONbits.ON = 1;           // enable Timer3
    OC1CONbits.ON = 1;          // enable OC1
}

void set_currentgains(float p, float i) {
    curr_gains.Kp = p;
    curr_gains.Ki = i;
}

gain_struct get_currentgains(void) {
    return curr_gains;
}
