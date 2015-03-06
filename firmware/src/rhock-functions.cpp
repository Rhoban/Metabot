#include <stdio.h>
#include <rhock/stream.h>
#include <rhock/clock.h>
#include <rhock/event.h>
#include "rhock-functions.h"
#include "locomotion.h"
#ifndef __EMSCRIPTEN__
#include <wirish/wirish.h>
#endif

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

void rhock_on_all_stopped()
{
    controlling = NULL;
    locomotion_stop();
}

void rhock_on_reset()
{
    rhock_stream_begin(6);
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

#ifndef __EMSCRIPTEN__
uint32_t rhock_gettime()
{
    return millis();
}
#endif
