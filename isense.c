#include "isense.h"
#include <xc.h>

#define SAMPLE_TIME 100

void ADC_init(void){
    AD1PCFGbits.PCFG0 = 0;      // set AN0 as analog input
    AD1CON1bits.SSRC = 0b111;   // auto conversion
    AD1CON1bits.ASAM = 0;       // manual sampling
    AD1CON3bits.ADCS = 2;       // set Tad = 75ns
    AD1CON3bits.SAMC = 2;       // sample for 2 Tad
    AD1CON1bits.ON = 1;       // turn on ADC
}

unsigned int ADC_read(int pin){
    // unsigned int t_run = 0;
    // unsigned int t_end;

    AD1CHSbits.CH0SA = pin;                 // pin to sample
    AD1CON1bits.SAMP = 1;                   // start sampling

    // t_run = _CP0_GET_COUNT();               // get current core timer count
    // t_end = t_run + SAMPLE_TIME;            // sampling time = 250 ns

    // while (_CP0_GET_COUNT() < t_end) { ; }  // sample for 250 ns

    // AD1CON1bits.SAMP = 0;                   // stop sampling, start converting

    while (!AD1CON1bits.DONE) { ; }         // wait for conversion

    return ADC1BUF0;                        // read buffer with result
}
