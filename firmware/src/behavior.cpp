#include <math.h>
#include <terminal.h>
#include "imu.h"
#include "distance.h"
#include "motion.h"
#include "buzzer.h"
#include "behavior.h"
#include "leds.h"

TERMINAL_PARAMETER_INT(bhv, "Behavior", 0);

void behavior_set(uint8_t b)
{
    bhv = b;
}

/**
 * Here you can write down your own behaviour, the tick method (at the end of
 * this file) is called with a given dt (in s) that you can use for temporal
 * counters.
 *
 * You will find some examples that you can enable by uncommenting the method
 * calls below. The buzzer, leds, motors and sensors are used in theses examples,
 * allowing you to understand how to recover informations from the different
 * robot hardware features.
 */

/**
 * Behavior example
 * Simply blinks the led green and blue (0.5ms each)
 */
void behavior_blink_leds(float dt)
{
    static float t = 0;
    t += dt;
    if (t > 1) t -= 1;

    if (t < 0.5) {
        led_set_all(LED_G, true);
    } else {
        led_set_all(LED_B, true);
    }
}

/**
 * Behavior example
 * Produce a +-30mm sinus on the height of the robot, it will
 * then appear to breath
 */
void behavior_sinus_height(float dt)
{
    static float t = 0;
    t += dt;

    motion_set_h(30*sin(t));
}

/**
 * Behavior example
 * The robot explore, it walks forward and turn around when there is an
 * obstacle in the front of it
 */
void behavior_explore(float dt)
{
    static int state = 0;
    static float t = 0;
    t += dt;

    if (state == 0) {
        // Walking forward
        motion_set_x_speed(150);
        motion_set_turn_speed(0);

        // There is an obstacle, changing state
        if (distance_get() < 30) {
            t = 0;
            state = 1;
            // Produce a beep
            buzzer_beep(440, 100);
        }
    } else if (state == 1) {
        // Turning around
        motion_set_x_speed(0);
        motion_set_turn_speed(90);

        // 2s have elapsed (at 90°/s), going back to explore
        if (t > 2) {
            state = 0;
        }
    }
}

/**
 * Behavior example
 * The robot walks forward. If it is grabbed and tilted more than 15°, it will
 * stop walking and scream.
 */
void behavior_grab(float dt)
{
    motion_set_x_speed(150);

    if (fabs(imu_pitch()) > 15 || fabs(imu_roll()) > 15) {
        buzzer_beep(200, 100);
        motion_set_x_speed(0);
    }
}

/**
 * Behavior example
 * The robot will turn around scanning for the longest distance, then it will
 * walk a little in this direction and start again.
 */
void behavior_scan_space(float dt)
{
    static int state = 0;
    static float t = 0;
    static float best_yaw = 0;
    static float best_dist = 0;

    t += dt;

    if (state == 0) {
        // Scanning around
        motion_set_x_speed(0);
        motion_set_turn_speed(90);
        float dist = distance_get();
        if (dist > best_dist) {
            buzzer_beep(800, 50);
            best_dist = dist;
            best_yaw = imu_yaw();
        }
        if (t > 4) {
            state = 1;
        }
    } else if (state == 1) {
        // Aligning with the best seen yaw
        float err = imu_yaw()-best_yaw;
        float order = err;
        if (order > 90) order = 90;
        if (order > 0 && order < 30) order = 30;
        if (order < -90) order = -90;
        if (order < 0 && order > -30) order = -30;

        motion_set_x_speed(0);
        motion_set_turn_speed(order);
        if (fabs(err) < 6) {
            state = 2;
            t = 0;
        }
    } else if (state == 2) {
        // Walking a little
        motion_set_x_speed(189);
        motion_set_turn_speed(0);

        if (t > 2) {
            state = 0;
            t = 0;
            best_dist = 0;
        }
    }
}

void behavior_tick(float dt)
{
    static int lastBhv = 0;

    if (bhv == 1) behavior_blink_leds(dt);
    if (bhv == 2) behavior_sinus_height(dt);
    if (bhv == 3) behavior_explore(dt);
    if (bhv == 4) behavior_grab(dt);
    if (bhv == 5) behavior_scan_space(dt);

    if (bhv == 0 && lastBhv != 0) {
        leds_decustom();
        motion_set_x_speed(0);
        motion_set_y_speed(0);
        motion_set_turn_speed(0);
        motion_reset();
        buzzer_stop();
    }
    lastBhv = bhv;
}
