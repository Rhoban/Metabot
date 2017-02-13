#ifndef _VOLTAGE_H
#define _VOLTAGE_H

#define VOLTAGE_PIN     7
#define VOLTAGE_R1      20
#define VOLTAGE_R2      10
#define VOLTAGE_LIMIT   6.9
#define VOLTAGE_SHUT    5.3

#define LIT     22

/**
 * Is there any voltage error?
 */
bool voltage_error();

/**
 * Initializes the voltage
 */
void voltage_init();

/**
 * Update the voltage
 */
void voltage_tick();

/**
 * Returns the current system voltage
 */
float voltage_current();

#endif
