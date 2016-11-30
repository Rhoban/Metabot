#ifndef _METABOT_MOTORS_H
#define _METABOT_MOTORS_H

/**
 * Enables all the motors torque
 */
void motors_enable();

/**
 * Disables all the motors torque
 */
void motors_disable();

/**
 * Are the motors enabled?
 */
bool motors_enabled();

/**
 * Update the motors colors
 */
void motors_colorize();

/**
 * Read all the motors values
 */
void motors_read();

/**
 * Position of the ith motor
 * @param  i The index of the motor (start at 0)
 */
float motors_get_position(int i);

#endif
