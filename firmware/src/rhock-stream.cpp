#include <stdio.h>
#include <rhock/event.h>
#include <rhock/stream.h>
#ifndef __EMSCRIPTEN__
#include <commands.h>
#endif
#include "leds.h"
#include "mapping.h"
#include "locomotion.h"

#define RHOCK_STREAM_METABOT    50

char rhock_on_packet(uint8_t type)
{
    if (type == RHOCK_STREAM_METABOT) {
        if (rhock_stream_available() > 1) {
            uint8_t command = rhock_stream_read();
            switch (command) {
                case 0: // Starting the robot
#ifndef __EMSCRIPTEN__
                    // Running the actual motors
                    start();
#endif
                    // Colorizing the front of the robot
                    colorize();
                    break;
                case 1: // Stop
                    // Marking leds as non custom
                    leds_decustom();
#ifndef __EMSCRIPTEN__
                    // Stopping the actual motors
                    stop();
#endif
                    // Turning all leds red
                    led_set_all(LED_R);
                    break;
            }
        }
    }
    return 0;
}
