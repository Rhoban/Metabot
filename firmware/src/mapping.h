#ifndef _METABOT_MAPPING
#define _METABOT_MAPPING

#include <stdint.h>

extern uint8_t mapping[12];

/**
 * Changes the servo mapping (the "front" of the robot)
 * @param map An int between 0 and 3
 */
void remap(int map);

/**
 * Colorize the legs
 */
void colorize();

#endif
