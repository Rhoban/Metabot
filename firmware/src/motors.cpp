#ifndef __EMSCRIPTEN__
#include <commands.h>
#endif
#include "leds.h"
#include "motors.h"

static bool is_enabled = false;

void motors_enable()
{
    // Motors are enabled
    is_enabled = true;
#ifndef __EMSCRIPTEN__
    // Running the actual motors
    start();
#endif
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
}

bool motors_enabled()
{
#ifndef __EMSCRIPTEN__
    return started;
#else
    return is_enabled;
#endif
}
