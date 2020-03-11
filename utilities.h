#ifndef UTILS__H__
#define UTILS__H__

enum avail_modes {IDLE, PWM, ITEST, HOLD, TRACK};

int get_mode(void);
void set_mode(void);

#endif
