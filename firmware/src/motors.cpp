#ifndef __EMSCRIPTEN__
#include <commands.h>
#endif
#include "leds.h"
#include "mapping.h"
#include "motors.h"

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
