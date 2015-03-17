#include <stdio.h>
#include <rhock/stream.h>
#include <rhock/event.h>
#include "rhock-functions.h"
#include "locomotion.h"
#ifndef __EMSCRIPTEN__
#include <wirish/wirish.h>
#endif
#include "leds.h"
#include "mapping.h"

struct rhock_context *controlling = NULL;
float save_dx, save_dy, save_turn;

void locomotion_stop()
{
    // When reseting the VM, stopping the robot
    locomotion_set_dx(0);
    locomotion_set_dy(0);
    locomotion_set_turn(0);
}

void locomotion_resume()
{
    locomotion_set_dx(save_dx);
    locomotion_set_dy(save_dy);
    locomotion_set_turn(save_turn);
}

RHOCK_NATIVE(robot_control)
{
    controlling = context;
    save_dx = RHOCK_POPF();
    save_dy = RHOCK_POPF();
    save_turn = RHOCK_POPF();
    locomotion_resume();

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(board_led)
{
#ifndef __EMSCRIPTEN__
    digitalWrite(BOARD_LED_PIN, !RHOCK_POPI());
#endif

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_h)
{
    locomotion_set_h(RHOCK_POPF());

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_led)
{
    int led = RHOCK_POPF();
    int value = RHOCK_POPF();
    led_set(led, value, true);
    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_leds)
{
    int value = RHOCK_POPF();
    led_set_all(value, true);
    return RHOCK_NATIVE_CONTINUE;
}

void rhock_on_all_stopped()
{
    // Decustom the leds
    leds_decustom();
    // Stopping the locomotion
    controlling = NULL;
    locomotion_stop();
    locomotion_reset();
}

void rhock_on_reset()
{
    // Resetting the mapping and the color
    remap(0);
    colorize();
    // Send a packet to tell the host reseted
    rhock_stream_begin(RHOCK_STREAM_RESET);
    rhock_stream_end();
}

void rhock_on_pause(struct rhock_context *context)
{
    if (context == controlling) {
        locomotion_stop();
    }
}

void rhock_on_start(struct rhock_context *context)
{
    if (context == controlling) {
        locomotion_resume();
    }
}

void rhock_on_stop(struct rhock_context *context)
{
    if (context == controlling) {
        locomotion_stop();
        controlling = NULL;
    }
}
