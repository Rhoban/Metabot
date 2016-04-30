#include <terminal.h>
#include "mux.h"

#define OPTICAL_EN1     13
#define OPTICAL_EN2     12

void opticals_init()
{
    digitalWrite(OPTICAL_EN1, LOW);
    digitalWrite(OPTICAL_EN2, LOW);
    pinMode(OPTICAL_EN1, OUTPUT);
    pinMode(OPTICAL_EN2, OUTPUT);
}

TERMINAL_COMMAND(opt, "Test opticals")
{
    mux_set_addr(6);
    digitalWrite(OPTICAL_EN1, HIGH);
    digitalWrite(OPTICAL_EN2, HIGH);
    while (!SerialUSB.available()) {
        delay(1);
        terminal_io()->println(mux_sample(1));
        delay(10);
    }
}
