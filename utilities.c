#include "utilities.h"
#include <xc.h>

enum avail_modes mode;

int get_mode(void) {
    return mode;
}

void set_mode(enum avail_modes i) {
    mode = i;
}
