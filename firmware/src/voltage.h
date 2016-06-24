#ifndef _VOLTAGE_H
#define _VOLTAGE_H

#define VOLTAGE_R1      10
#define VOLTAGE_R2      10
#define VOLTAGE_LIMIT   7.5
#define VOLTAGE_SHUT    5.3

bool voltage_error();
void voltage_init();
void voltage_tick();
float voltage_current();

#endif
