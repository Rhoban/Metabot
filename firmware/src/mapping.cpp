#include <stdio.h>
#include "config.h"
#include "mapping.h"
#include "leds.h"
#ifndef __EMSCRIPTEN__
#include <terminal.h>
#endif

// Current mapping
static uint8_t currentMapping = 0;

// This is the servo mappings
uint8_t mapping[18];

/**
 * Colorizes the two front legs
 */
void colorize()
{
    if (!leds_are_custom()) {
        for (int i=0; i<6; i++) {
            led_set(mapping[i], LED_R|LED_G|LED_B);
        }
        for (int i=6; i<18; i++) {
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
    currentMapping = direction;
    for (int i=0; i<18; i++) {
        mapping[i] = servos_order[(i+3*direction)%18];
    }
}


#ifndef __EMSCRIPTEN__
TERMINAL_COMMAND(remap,
        "Changes the mapping")
{
    if (argc == 0) {
        terminal_io()->print("remap=");
        terminal_io()->println(currentMapping);
    } else {
        currentMapping = atoi(argv[0]);
        remap(currentMapping);
    }
}
#endif
