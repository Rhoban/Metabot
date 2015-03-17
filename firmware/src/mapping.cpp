#include <stdio.h>
#include "config.h"
#include "mapping.h"
#include "leds.h"
#ifndef __EMSCRIPTEN__
#include <terminal.h>
#endif

// This is the servo mappings
uint8_t mapping[12];

/**
 * Colorizes the two front legs
 */
void colorize()
{
    if (!leds_are_custom()) {
        for (int i=0; i<6; i++) {
            led_set(mapping[i], LED_R|LED_G|LED_B);
        }
        for (int i=6; i<12; i++) {
            led_set(mapping[i], 0);
        }
    }
}

/**
 * Changes the mapping of the leg, the input is the direction
 * (between 0 and 3)
 */
void remap(int direction)
{
    for (int i=0; i<12; i++) {
        mapping[i] = servos_order[(i+3*direction)%12];
    }
}


#ifndef __EMSCRIPTEN__
TERMINAL_COMMAND(remap,
        "Changes the mapping")
{
    remap(atoi(argv[0]));
}
#endif
