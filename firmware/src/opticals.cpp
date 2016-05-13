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
    digitalWrite(OPTICAL_EN1, HIGH);
    digitalWrite(OPTICAL_EN2, HIGH);
    while (!SerialUSB.available()) {
        delay(1);
        mux_set_addr(5);
        terminal_io()->print(mux_sample(1));
        terminal_io()->print(" ");
        mux_set_addr(6);
        terminal_io()->print(mux_sample(1));
        terminal_io()->print(" ");
        mux_set_addr(7);
        terminal_io()->print(mux_sample(1));
        terminal_io()->println();
        delay(10);
    }
}
