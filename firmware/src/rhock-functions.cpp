#include <stdio.h>
#include <rhock/stream.h>
#include <rhock/event.h>
#include "rhock-functions.h"
#include "motion.h"
#ifndef __EMSCRIPTEN__
#include <wirish/wirish.h>
#endif
#include "motors.h"
#include "leds.h"
#include "mapping.h"

struct rhock_context *controlling = NULL;
float save_dx, save_dy, save_turn;

void motion_stop()
{
    // When reseting the VM, stopping the robot
    motion_set_dx(0);
    motion_set_dy(0);
    motion_set_turn(0);
    save_dx = 0;
    save_dy = 0;
    save_turn = 0;
    controlling = NULL;
}

void motion_resume()
{
    // Resume the motion
    motion_set_dx(save_dx);
    motion_set_dy(save_dy);
    motion_set_turn(save_turn);
}

/**
 * Called when all rhock processes are stopped
 */
void rhock_on_all_stopped()
{
    // Decustom the leds
    leds_decustom();
    // Stopping the motion
    controlling = NULL;
    motion_stop();
    motion_reset();
}

/**
 * called when rhock process starts
 */
void rhock_on_reset()
{
    // Resetting the mapping and the color
    motors_colorize();
}

/**
 * Handling thread pause, stop and start
 */
void rhock_on_pause(struct rhock_context *context)
{
    if (context == controlling) {
        motion_stop();
    }
}

void rhock_on_stop(struct rhock_context *context)
{
    if (context == controlling) {
        motion_stop();
        controlling = NULL;
    }
}

void rhock_on_start(struct rhock_context *context)
{
    if (context == controlling) {
        motion_resume();
    }
}

RHOCK_NATIVE(robot_control)
{
    controlling = context;
    save_turn = RHOCK_POPF();
    save_dy = RHOCK_POPF();
    save_dx = RHOCK_POPF();
    motion_resume();

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_stop_moving)
{
    motion_stop();

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_dx)
{
    controlling = context;
    save_turn = 0;
    save_dy = 0;
    save_dx = RHOCK_POPF();
    motion_resume();

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_dy)
{
    controlling = context;
    save_turn = 0;
    save_dy = RHOCK_POPF();
    save_dx = 0;
    motion_resume();

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_turn)
{
    controlling = context;
    save_turn = RHOCK_POPF();
    save_dy = 0;
    save_dx = 0;
    motion_resume();

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(board_led)
{
#ifndef __EMSCRIPTEN__
    digitalWrite(BOARD_LED_PIN, !RHOCK_POPI());
#endif

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_f)
{
    motion_set_f(RHOCK_POPF());

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_h)
{
    motion_set_h(RHOCK_POPF());

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_led)
{
    int value = RHOCK_POPF();
    int led = RHOCK_POPF();
    led_set(led, value, true);
    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_leds)
{
    int value = RHOCK_POPF();
    led_set_all(value, true);
    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_leg_leds)
{
    int value = RHOCK_POPF();
    int leg = RHOCK_POPF();
    leg = ((leg-1)&3);
    for (int k=0; k<3; k++) {
        led_set(mapping[3*leg+k], value, true);
    }
    return RHOCK_NATIVE_CONTINUE;
}
