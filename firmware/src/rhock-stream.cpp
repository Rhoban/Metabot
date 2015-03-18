#include <stdio.h>
#include <rhock/event.h>
#include <rhock/stream.h>
#include "leds.h"
#include "mapping.h"
#include "motion.h"
#include "motors.h"

#define RHOCK_STREAM_METABOT    50

char rhock_on_packet(uint8_t type)
{
    if (type == RHOCK_STREAM_METABOT) {
        if (rhock_stream_available() >= 1) {
            uint8_t command = rhock_stream_read();
            switch (command) {
                case 0: // Starting the robot
                    motors_enable();
                    return 1;
                    break;
                case 1: // Stop
                    motors_disable();
                    return 1;
                    break;
            }
        }
    }
    return 0;
}
