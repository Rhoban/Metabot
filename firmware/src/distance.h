#ifndef __DISTANCE_H
#define __DISTANCE_H

/**
 * Initializes the distance sensor
 */
void distance_init();

/**
 * Reads the distance sensor (returns a value in cm)
 */
float distance_get();

/**
 * Updates the value
 */
void distance_tick();

#endif
