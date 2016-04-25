#include <wirish/wirish.h>
#include "mux.h"

#define ADDR1   20
#define ADDR2   21
#define ADDR3   22
#define MUX1    5
#define MUX2    6

void mux_init()
{
    mux_set_addr(0);
    pinMode(ADDR1, OUTPUT);
    pinMode(ADDR2, OUTPUT);
    pinMode(ADDR3, OUTPUT);
    pinMode(MUX1, INPUT);
    pinMode(MUX2, INPUT);
}

void mux_set_addr(int addr)
{
    digitalWrite(ADDR1, (addr>>0)&1);
    digitalWrite(ADDR2, (addr>>1)&1);
    digitalWrite(ADDR3, (addr>>2)&1);
}

int mux_sample(int mux)
{
    if (mux == 0) return analogRead(MUX1);
    else return analogRead(MUX2);
}
