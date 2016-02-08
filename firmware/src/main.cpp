#include <stdlib.h>
#include <wirish/wirish.h>
#include <terminal.h>
#include <main.h>
#include <math.h>
#include <dxl.h>
#include <function.h>
#include <commands.h>
#include <rc.h>
#include <rhock.h>
#include <servos.h>
#include "config.h"
#include "motion.h"
#include "leds.h"
#include "mapping.h"

bool flag = false;
bool isUSB = false;

// Time
TERMINAL_PARAMETER_FLOAT(t, "Time", 0.0);

TERMINAL_COMMAND(version, "Getting firmware version")
{
    terminal_io()->print("version=");
    terminal_io()->println(METABOT_VERSION);
}

TERMINAL_COMMAND(started, "Is the robot started?")
{
    terminal_io()->print("started=");
    terminal_io()->println(started);
}

TERMINAL_COMMAND(rc, "Go to RC mode")
{
    RC.begin(921600);
    terminal_init(&RC);
    isUSB = false;
}

void setFlag()
{
    flag = true;
}

// Enabling/disabling move
TERMINAL_PARAMETER_BOOL(move, "Enable/Disable move", true);

// Average voltage
TERMINAL_PARAMETER_INT(voltage, "Average voltage (dV)", 75);

/**
 * Initializing
 */
void setup()
{
    RC.begin(921600);
    terminal_init(&RC);

    motion_init();

    // Initializing the DXL bus
    delay(500);
    dxl_init();

    // Initializing config (see config.h)
    config_init();

    // Initializing positions to 0
    for (int i=0; i<18; i++) {
        dxl_set_position(servos_order[i], 0.0);
    }
    for (int i=0; i<6; i++) {
        l1[i] = l2[i] = l3[i] = 0;
    }

    // Enabling 50hz ticking
    servos_init();
    servos_attach_interrupt(setFlag);
}

TERMINAL_PARAMETER_INT(step, "Number of step to do", -1);

/**
 * Computing the servo values
 */
void tick()
{
    // Computing average voltage
    static int idToRead = 0;
    static int blink;

    idToRead++;
    if (idToRead >= 18) idToRead = 0;
    bool success;
    int voltageOnce = dxl_read_byte(idToRead+1, DXL_VOLTAGE, &success);
    if (success) {
        if (voltageOnce < voltage) voltage--;
        if (voltageOnce > voltage) voltage++;
    }

    if (voltage < 60) {
        dxl_write_word(DXL_BROADCAST, DXL_GOAL_TORQUE, 0);
        blink++;
        if (blink > 10) {
            blink = 0;
        }
        dxl_write_byte(DXL_BROADCAST, DXL_LED, blink<5);
        return;
    }

    if (!move || !started) {
        t = 0.0;
        return;
    }

    // Incrementing and normalizing t
    t += motion_get_f()*0.02;
    if (t > 1.0) {
        t -= 1.0;
        if (step > 0) step--;
        colorize();
    }
    if (t < 0.0) t += 1.0;
    if (step == 0) t = 0.0;

    motion_tick(t, step!=0 ? 1.0 : 0.0);
   
    // Sending order to servos
    dxl_set_position(mapping[0], l1[0]);
    dxl_set_position(mapping[3], l1[1]);
    dxl_set_position(mapping[6], l1[2]);
    dxl_set_position(mapping[9], l1[3]);
    dxl_set_position(mapping[12], l1[4]);
    dxl_set_position(mapping[15], l1[5]);

    dxl_set_position(mapping[1], l2[0]);
    dxl_set_position(mapping[4], l2[1]);
    dxl_set_position(mapping[7], l2[2]);
    dxl_set_position(mapping[10], l2[3]);
    dxl_set_position(mapping[13], l2[4]);
    dxl_set_position(mapping[16], l2[5]);

    dxl_set_position(mapping[2], l3[0]);
    dxl_set_position(mapping[5], l3[1]);
    dxl_set_position(mapping[8], l3[2]);
    dxl_set_position(mapping[11], l3[3]);
    dxl_set_position(mapping[14], l3[4]);
    dxl_set_position(mapping[17], l3[5]);
}

void loop()
{
    // Updating the terminal
    terminal_tick();
#if defined(RHOCK)
    rhock_tick();
#endif
    if (SerialUSB.available() && !isUSB) {
        isUSB = true;
        terminal_init(&SerialUSB);
    }

    // Calling user motion tick
    if (flag) {
        flag = false;
        tick();
        dxl_flush();
    }
}
