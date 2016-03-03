#ifndef _VOLTAGE_H
#define _VOLTAGE_H

#define VOLTAGE_PIN     7
#define VOLTAGE_R1      20
#define VOLTAGE_R2      10
#define VOLTAGE_LIMIT   6.9
#define VOLTAGE_SHUT    5.3

bool voltage_error();
void voltage_init();
void voltage_tick();
float voltage_current();

#endif
