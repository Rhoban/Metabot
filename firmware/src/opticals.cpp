#include <terminal.h>

#define OPTICALS 10
#define EN 18
#define ADDR1 20
#define ADDR2 21
#define ADDR3 22

void opticals_addr(int addr)
{
    digitalWrite(20, (addr>>0)&0x1);
    digitalWrite(21, (addr>>1)&0x1);
    digitalWrite(22, (addr>>2)&0x1);
}

void opticals_init()
{
    opticals_addr(0);
    pinMode(ADDR1, OUTPUT);
    pinMode(ADDR2, OUTPUT);
    pinMode(ADDR3, OUTPUT);
    digitalWrite(EN, LOW);
    pinMode(EN, OUTPUT);
    pinMode(OPTICALS, INPUT);
}

TERMINAL_COMMAND(opt, "Test opticals")
{
    while (!SerialUSB.available()) {
        digitalWrite(EN, HIGH);
        delay(1);
        for (int k=0; k<6; k++) {
            opticals_addr(k);
            terminal_io()->print(analogRead(OPTICALS));
            terminal_io()->print("\t");
        }
        terminal_io()->println();
        digitalWrite(EN, LOW);
        delay(100);
    }
}
