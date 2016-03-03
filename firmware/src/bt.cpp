#include <terminal.h>
#include <rc.h>

#define BTCONF_PIN              20

void bt_init()
{
    pinMode(BTCONF_PIN, OUTPUT);
    digitalWrite(BTCONF_PIN, LOW);

    RC.begin(38400);
    for (int k=0; k<10; k++) {
        RC.write("AT+RESET\r\n\r\n");
        RC.write("AT+RESET\r\n");
    }
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
    delay(10);
    RC.write("AT+NAME=");
    RC.write(name);
    RC.write("\r\n");
    delay(10);
    RC.write("AT+PSWD=");
    RC.write(pin);
    delay(10);
    RC.write("\r\n");
    RC.write("AT+RESET\r\n");
    delay(10);
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

        for (int k=0; k<3; k++) {
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
}

TERMINAL_COMMAND(btpulse, "BT conf pulse")
{
    goToConf();
}
