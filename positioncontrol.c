#include <xc.h>
#include "currentcontrol.h"
#include "positioncontrol.h"

void positioncontrol_init(void) {

}

void set_positiongains(float p, float i, float d) {
    pos_gains.Kp = p;
    pos_gains.Ki = i;
    pos_gains.Kd = d;
}

gain_struct get_positiongains(void) {
    return pos_gains;
}
