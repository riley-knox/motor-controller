#ifndef UTILS__H__
#define UTILS__H__

#define NUM_SAMPS 100

enum avail_modes {IDLE, PWM, ITEST, HOLD, TRACK};

volatile int current_refs[NUM_SAMPS];
volatile int current_actuals[NUM_SAMPS];

int get_mode(void);
void set_mode(enum avail_modes);


#endif
