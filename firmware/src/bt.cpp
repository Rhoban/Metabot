#include <terminal.h>
#include <rc.h>

#define BTCONF_PIN              20

void bt_init()
{
    pinMode(BTCONF_PIN, OUTPUT);
    digitalWrite(BTCONF_PIN, LOW);

    RC.begin(38400);
    RC.write("AT+RESET\r\n");
    RC.write("AT+RESET\r\n");
    RC.write("AT+RESET\r\n");
    RC.begin(921600);
}

static void goToConf()
{
    digitalWrite(BTCONF_PIN, HIGH);
    delay(100);
    digitalWrite(BTCONF_PIN, LOW);
}

static void bt_conf(char *name, char *pin)
{
    goToConf();
    RC.write("AT+UART=921600,0,0\r\n");
    RC.write("AT\r\n");
    RC.write("AT+NAME=");
    RC.write(name);
    RC.write("\r\n");
    RC.write("AT+PSWD=");
    RC.write(pin);
    RC.write("\r\n");
    RC.write("AT+RESET\r\n");
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

TERMINAL_COMMAND(btpulse, "BT conf pulse")
{
    goToConf();
}
