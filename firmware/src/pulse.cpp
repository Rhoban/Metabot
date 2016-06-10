#include <terminal.h>
#include "pulse.h"

#define PULSE 31

void pulse_init()
{
    digitalWrite(PULSE, 0);
    pinMode(PULSE, OUTPUT);
}

void pulse()
{
    digitalWrite(PULSE, HIGH);
    delay(100);
    digitalWrite(PULSE, LOW);
}

TERMINAL_COMMAND(pul, "Pulse")
{
    pulse();
}
