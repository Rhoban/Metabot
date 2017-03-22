#include <terminal.h>
#include <rc.h>
#include "bt.h"

#define BTCONF_PIN              20

void bt_init()
{
    pinMode(BTCONF_PIN, OUTPUT);
    digitalWrite(BTCONF_PIN, LOW);

#ifdef BT_HC05
    RC.begin(38400);
    for (int k=0; k<5; k++) {
        RC.write("AT+RESET\r\n\r\n");
        RC.write("AT+RESET\r\n");
    }
#endif

#ifdef BT_HM12
    RC.write("AT+RESET"); delay(10);
#endif

    RC.begin(BT_BAUD);
}

static void goToConf()
{
#ifdef BT_HC05
    digitalWrite(BTCONF_PIN, LOW);
    delay(100);
    digitalWrite(BTCONF_PIN, HIGH);
#endif
}

static void bt_conf(char *name, char *pin)
{
#ifdef BT_HC05
    goToConf();
    RC.write("AT\r\r");
    delay(150);
    RC.write("AT+UART=921600,0,0\r\r");
    delay(150);
    RC.write("AT+NAME=");
    RC.write(name);
    RC.write("\r\r");
    delay(150);
    RC.write("AT+PSWD=");
    RC.write(pin);
    delay(150);
    RC.write("\r\r");
    RC.write("AT+RESET\r\r");
    delay(150);
#endif

#ifdef BT_HM12
    RC.write("AT+NAME");
    RC.write(name); delay(100);
    RC.write("AT+NAMB");
    RC.write(name);
    RC.write("-BLE"); delay(100);
    RC.write("AT+PINE");
    RC.write(pin); delay(100);
    RC.write("AT+BAUD7"); delay(100);
    RC.write("AT+RESET"); delay(100);
#endif
}

void bt_set_config(char *name, char *pin)
{
    for (int k=0; k<3; k++) {
        RC.begin(9600);
        for (int n=0; n<3; n++) bt_conf(name, pin);
#ifdef BT_HC05
        RC.begin(38400);
        for (int n=0; n<3; n++) bt_conf(name, pin);
#endif
        RC.begin(BT_BAUD);
        for (int n=0; n<3; n++) bt_conf(name, pin);
    }
}

TERMINAL_COMMAND(btconf, "Bluetooth config")
{
    if (argc != 2) {
        terminal_io()->println("Usage: btconf <name> <pin>");
    } else {
        char *name = argv[0];
        char *pin = argv[1];
        terminal_io()->println("Configuring bluetooth to name:");
        terminal_io()->println(name);
        terminal_io()->println("And pin:");
        terminal_io()->println(pin);

        bt_set_config(name, pin);
    }
}

TERMINAL_COMMAND(btpulse, "BT conf pulse")
{
    goToConf();
}
