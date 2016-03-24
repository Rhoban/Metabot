#include <terminal.h>
#include "distance.h"
#include "motion.h"

TERMINAL_PARAMETER_BOOL(bhv, "Enable behaviour", false);

void bhv_tick()
{
    if (bhv) {
        float dist = distance_read(3);
        if (dist < 55.0) {
            motion_set_x_speed(0.0);
            motion_set_turn_speed(60);
        } else {
            motion_set_x_speed(150.0);
            motion_set_turn_speed(0);
        }
    }
}
