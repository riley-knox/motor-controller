#ifndef POSITION__H__
#define POSITION__H__

gain_struct pos_gains;

void positioncontrol_init();
void set_positiongains(float p, float i, float d);
gain_struct get_positiongains();

#endif
