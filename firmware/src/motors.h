#ifndef _METABOT_MOTORS_H
#define _METABOT_MOTORS_H

void motors_enable();
void motors_disable();
bool motors_enabled();
void motors_colorize();
void motors_read();
float motors_get_position(int i);

#endif
