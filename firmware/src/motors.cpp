#ifndef __EMSCRIPTEN__
#include <commands.h>
#include <dxl.h>
#endif
#include "leds.h"
#include "motion.h"
#include "mapping.h"
#include "motors.h"

static float motors_positions[12];
static bool is_enabled = false;

void motors_colorize()
{
    if (motors_enabled()) {
        // Colorizing the front of the robot
        colorize();
    } else {
        // Turning all leds red
        led_set_all(LED_R);
    }
}

void motors_enable()
{
    // Motors are enabled
    is_enabled = true;
#ifndef __EMSCRIPTEN__
    // Running the actual motors
    start();
#endif
    motors_colorize();
}

void motors_disable()
{
    // Motors are disabled
    is_enabled = false;
    // Marking leds as non custom
#ifndef __EMSCRIPTEN__
    // Stopping the actual motors
    stop();
#endif
    motors_colorize();
}

bool motors_enabled()
{
#ifndef __EMSCRIPTEN__
    return started;
#else
    return is_enabled;
#endif
}

void motors_read()
{
#ifndef __EMSCRIPTEN__
    ui8 ids[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    uint16_t positions[24];
    if (dxl_sync_read(ids, 12, DXL_POSITION, 2, (ui8*)positions)) {
        for (int k=0; k<12; k++) {
            motors_positions[k] = dxl_value_to_position(k+1, positions[k]);
        }
    }
#endif
}

float motors_get_position(int i)
{
#ifndef __EMSCRIPTEN__
    if (motors_enabled()) {
        return motion_get_motor(i);
    } else {
        return motors_positions[i];
    }
#else
    return motion_get_motor(i);
#endif
}
