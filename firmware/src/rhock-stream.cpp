#include <stdio.h>
#include <rhock/event.h>
#include <rhock/stream.h>
#include "rhock-stream.h"
#include "leds.h"
#include "mapping.h"
#include "motion.h"
#include "buzzer.h"
#include "motors.h"
#include "imu.h"
#include "leds.h"
#include "behavior.h"

#define RHOCK_STREAM_METABOT    50

short rhock_controls[16] = {0};

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
                case 2: // Rotate calibration
#ifndef __EMSCRIPTEN__
                    imu_calib_rotate();
#endif
                    return 1;
                    break;
                case 3: // Set control
                    if (rhock_stream_available() == 3) {
                        uint8_t control = rhock_stream_read();
                        if (control < RHOCK_CONTROLS) {
                            uint16_t value = rhock_stream_read_short();
                            rhock_controls[control] = (short)value;
                        }
                    }
                    return 1;
                    break;
                case 4: // Speed
                    if (rhock_stream_available() == 6) {
                        motion_set_x_speed(((int16_t)rhock_stream_read_short())/10.0);
                        motion_set_y_speed(((int16_t)rhock_stream_read_short())/10.0);
                        motion_set_turn_speed(((int16_t)rhock_stream_read_short())/10.0);
                    }
                    return 1;
                    break;
                case 5: // Beep
                    if (rhock_stream_available() == 4) {
                        uint16_t freq = rhock_stream_read_short();
                        uint16_t duration = rhock_stream_read_short();
                        buzzer_beep(freq, duration);
                    }
                    return 1;
                    break;
                case 6: // Leds
                    if (rhock_stream_available() == 1) {
                        led_set_all(rhock_stream_read(), true);
                    }
                    return 1;
                    break;
                case 7: // Reset
                    buzzer_stop();
                    leds_decustom();
                    motion_set_x_speed(0.0);
                    motion_set_y_speed(0.0);
                    motion_set_turn_speed(0.0);
                    motion_reset();
                    return 1;
                    break;
                case 8: // Behavior
                    if (rhock_stream_available() == 1) {
                        behavior_set(rhock_stream_read());
                    }
                    break;
            }
        }
    }
    return 0;
}
