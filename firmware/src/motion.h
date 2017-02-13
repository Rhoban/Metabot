#ifndef _METABOT_MOTION_H
#define _METABOT_MOTION_H

// Output angles
extern float l1[4], l2[4], l3[4];

/**
 * Initializes the motion
 */
void motion_init();

/**
 * Updates the motion
 * @param t The time (between 0 and 1) of the motion
 */
void motion_tick(float t);

/**
 * Resets the motion
 */
void motion_reset();

/**
 * Is the robot moving?
 */
bool motion_is_moving();

/**
 * Sets the target frequency (steps per second)
 * @param f The frequency (Hz)
 */
void motion_set_f(float f);

/**
 * Sets the target legs height
 * @param h Height (mm) of the legs
 */
void motion_set_h(float h);

/**
 * Sets the legs radius
 * @param r Legs radius (mm)
 */
void motion_set_r(float r);

/**
 * Sets the X speed
 * @param x_speed x target speed (mm/s)
 */
void motion_set_x_speed(float x_speed);

/**
 * Sets the Y speed
 * @param y_speed y target speed (mm/s)
 */
void motion_set_y_speed(float y_speed);

/**
 * Sets the turn speed
 * @param turn_speed turn target speed (°/s)
 */
void motion_set_turn_speed(float turn_speed);

/**
 * Add extra X value
 * @param index The index of the leg
 * @param x     Extra x (mm)
 */
void motion_extra_x(int index, float x);

/**
 * Add extra Y value
 * @param index The index of the leg
 * @param y     Extra y (mm)
 */
void motion_extra_y(int index, float y);

/**
 * Add extra Z value
 * @param index The index of the leg
 * @param z     Extra z (mm)
 */
void motion_extra_z(int index, float z);

/**
 * Adds extra angle
 * @param index Index of the leg (0-3, +: all)
 * @param motor Index of the motor (0-2)
 * @param angle Extra angle
 */
void motion_extra_angle(int index, int motor, float angle);

/**
 * The current frequency
 */
float motion_get_f();

/**
 * Gets the target angle for a specific motor
 * @param  idx The index of the motor
 */
float motion_get_motor(int idx);

#endif
