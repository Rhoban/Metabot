#include <terminal.h>
#include <rc.h>

#define BTCONF_PIN              20

void bt_init()
{
    pinMode(BTCONF_PIN, OUTPUT);
    digitalWrite(BTCONF_PIN, LOW);

    RC.begin(38400);
    RC.print("AT+RESET\r\r");
    RC.begin(921600);
}

static void bt_conf(char *name, char *pin)
{
    RC.print("AT\r\r");
    RC.print("AT+UART=921600,0,0\r\r");
    RC.print("AT+NAME=");
    RC.print(name);
    RC.print("\r\r");
    RC.print("AT+PSWD=");
    RC.print(pin);
    RC.print("\r\r");
    RC.print("AT+RESET\r\r");
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

        digitalWrite(BTCONF_PIN, HIGH);
        delay(100);
        digitalWrite(BTCONF_PIN, LOW);

        RC.begin(9600);
        bt_conf(name, pin);
        RC.begin(38400);
        bt_conf(name, pin);
        RC.begin(57600);
        bt_conf(name, pin);
        RC.begin(115200);
        bt_conf(name, pin);
        RC.begin(921600);
        bt_conf(name, pin);
    }
}
