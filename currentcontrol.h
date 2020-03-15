#ifndef CURRENT__H__
#define CURRENT__H__

#define PERIOD_REG 3999

typedef struct {
    float Kp, Ki, Kd;
} gain_struct;

gain_struct curr_gains;

void currentcontrol_init();
void set_currentgains(float p, float i);
gain_struct get_currentgains();

#endif
