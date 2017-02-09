#include <math.h>
#include <stdio.h>
#include <rhock/stream.h>
#include <rhock/event.h>
#include "rhock-functions.h"
#include "motion.h"
#ifndef __EMSCRIPTEN__
#include <wirish/wirish.h>
#else
#include <emscripten.h>
#endif
#include "distance.h"
#include "motors.h"
#include "leds.h"
#include "mapping.h"
#include "buzzer.h"
#include "imu.h"
#include "rhock-stream.h"

struct rhock_context *controlling = NULL;
struct rhock_context *controllingBuzzer = NULL;
float save_x_speed, save_y_speed, save_turn_speed;

void motion_stop()
{
    // When reseting the VM, stopping the robot
    motion_set_x_speed(0);
    motion_set_y_speed(0);
    motion_set_turn_speed(0);
    save_x_speed = 0;
    save_y_speed = 0;
    save_turn_speed = 0;
    controlling = NULL;
}

void motion_resume()
{
    // Resume the motion
    motion_set_x_speed(save_x_speed);
    motion_set_y_speed(save_y_speed);
    motion_set_turn_speed(save_turn_speed);
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
    buzzer_stop();
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
    if (context == controllingBuzzer) {
        buzzer_stop();
        controllingBuzzer = NULL;
    }
}

void rhock_on_start(struct rhock_context *context)
{
    if (context == controlling) {
        motion_resume();
    }
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

static void motion_control(float x_speed, float y_speed, float turn_speed,
        struct rhock_context *context)
{
    controlling = context;
    save_x_speed = x_speed;
    save_y_speed = y_speed;
    save_turn_speed = turn_speed;
    motion_resume();
}

RHOCK_NATIVE(robot_control)
{
    float turn_speed = RHOCK_POPF();
    float y_speed = RHOCK_POPF();
    float x_speed = RHOCK_POPF();
    motion_control(x_speed, y_speed, turn_speed, context);

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_stop)
{
    motion_stop();

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_x_speed)
{
    float x_speed = RHOCK_POPF();
    motion_control(x_speed, 0, 0, context);

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_y_speed)
{
    float y_speed = RHOCK_POPF();
    motion_control(0, y_speed, 0, context);

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_turn_speed)
{
    float turn_speed = RHOCK_POPF();
    motion_control(0, 0, turn_speed, context);

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

RHOCK_NATIVE(robot_r)
{
    motion_set_r(RHOCK_POPF());

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_ex)
{
    int leg = RHOCK_POPF();
    float extra = RHOCK_POPF();
    leg = leg-1;
    motion_extra_x(leg, extra);

    return RHOCK_NATIVE_CONTINUE;
}
RHOCK_NATIVE(robot_ey)
{
    int leg = RHOCK_POPF();
    float extra = RHOCK_POPF();
    leg = leg-1;
    motion_extra_y(leg, extra);

    return RHOCK_NATIVE_CONTINUE;
}
RHOCK_NATIVE(robot_ez)
{
    int leg = RHOCK_POPF();
    float extra = RHOCK_POPF();
    leg = leg-1;
    motion_extra_z(leg, extra);

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_ea)
{
    int leg = RHOCK_POPF();
    int index = RHOCK_POPF();
    float extra = RHOCK_POPF();
    leg = leg-1;
    motion_extra_angle(leg, index, extra);

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_reset)
{
    motion_init();

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_turn)
{
    ON_ENTER() {
        float turn_speed = RHOCK_POPF();
        float deg = RHOCK_POPF();
        if (deg < 0 && turn_speed > 0) {
            turn_speed = -turn_speed;
        }
        float time = fabs(deg/turn_speed);
        RHOCK_PUSHF(time*1000);

        motion_control(0, 0, turn_speed, context);
        return RHOCK_NATIVE_WAIT;
    }
    ON_ELAPSED() {
        RHOCK_SMASH(1);
        motion_stop();
        return RHOCK_NATIVE_CONTINUE;
    }
}

RHOCK_NATIVE(robot_move_x)
{
    ON_ENTER() {
        float speed = RHOCK_POPF();
        float dist = RHOCK_POPF();
        if (dist < 0 && speed > 0) {
            speed = -speed;
        }
        float time = fabs(dist/speed);
        RHOCK_PUSHF(time*1000);

        motion_control(speed, 0, 0, context);
        return RHOCK_NATIVE_WAIT;
    }
    ON_ELAPSED() {
        RHOCK_SMASH(1);
        motion_stop();
        return RHOCK_NATIVE_CONTINUE;
    }
}

RHOCK_NATIVE(robot_move_y)
{
    ON_ENTER() {
        float speed = RHOCK_POPF();
        float dist = RHOCK_POPF();
        if (dist < 0 && speed > 0) {
            speed = -speed;
        }
        float time = fabs(dist/speed);
        RHOCK_PUSHF(time*1000);

        motion_control(0, speed, 0, context);
        return RHOCK_NATIVE_WAIT;
    }
    ON_ELAPSED() {
        RHOCK_SMASH(1);
        motion_stop();
        return RHOCK_NATIVE_CONTINUE;
    }
}

RHOCK_NATIVE(robot_dist)
{
#ifndef __EMSCRIPTEN__
    RHOCK_PUSHF(distance_get());
#else
    // XXX: Simulate it, with EM ASM:
    //     return EM_ASM_INT({
    //        return simulator_get_distance();
    //     },
    RHOCK_PUSHF(100.0);
#endif

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_beep)
{
    ON_ENTER() {
        float duration = RHOCK_POPF();
        float freq = RHOCK_POPF();
        RHOCK_PUSHF(duration);

        buzzer_beep(freq, duration);
        controllingBuzzer = context;

        return RHOCK_NATIVE_WAIT;
    }
    ON_ELAPSED() {
        RHOCK_SMASH(1);
        buzzer_stop();
        return RHOCK_NATIVE_CONTINUE;
    }
}

RHOCK_NATIVE(robot_yaw)
{
#ifndef __EMSCRIPTEN__
    RHOCK_PUSHF(imu_yaw());
#else
    RHOCK_PUSHF(EM_ASM_DOUBLE({
        return simulator_get_yaw();
    }, 0));
#endif

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_pitch)
{
#ifndef __EMSCRIPTEN__
    RHOCK_PUSHF(imu_pitch());
#else
    RHOCK_PUSHF(0);
#endif

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_roll)
{
#ifndef __EMSCRIPTEN__
    RHOCK_PUSHF(imu_roll());
#else
    RHOCK_PUSHF(0);
#endif

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_motor_start)
{
    motors_enable();
    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_motor_stop)
{
    motors_disable();
    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_motor_position)
{
    int i = RHOCK_POPF();
    i = (i-1);
    if (i < 0) i = 0;
    if (i > 11) i = 11;

    RHOCK_PUSHF(motors_get_position(i));

    return RHOCK_NATIVE_CONTINUE;
}

RHOCK_NATIVE(robot_get_control)
{
    int32_t control = RHOCK_VALUE_TO_INT(RHOCK_POPI());

    if (control < RHOCK_CONTROLS) {
        RHOCK_PUSHF(rhock_controls[control]/100.0);
    } else {
        RHOCK_PUSHF(0);
    }

    return RHOCK_NATIVE_CONTINUE;
}
