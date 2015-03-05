#include <stdlib.h>
#include <wirish/wirish.h>
#include <servos.h>
#include <terminal.h>
#include <main.h>
#include <math.h>
#include <dxl.h>
#include <function.h>
#include <commands.h>
#include <rc.h>
#include <rhock/vm.h>
#include <rhock/stream.h>
#include "config.h"
#include "locomotion.h"

bool rhock_mode = false;

TERMINAL_COMMAND(rhock, "Go to Rhock mode")
{
    disable_terminal();
    rhock_mode = true;
}

void rhock_stream_send(uint8_t c)
{
    if (rhock_mode) {
        terminal_io()->write(c);
    }
}

// This is the servo mappings
ui8 mapping[12];

// Time
TERMINAL_PARAMETER_FLOAT(t, "Time", 0.0);

// Speed factor
TERMINAL_PARAMETER_FLOAT(freq, "Time factor gain", 2.0);

TERMINAL_COMMAND(version, "Getting firmware version")
{
    terminal_io()->println(METABOT_VERSION);
}

/**
 * Colorizes the two front legs
 */
static void colorize()
{
    if (started) {
        for (int i=0; i<6; i++) {
            dxl_write_byte(mapping[i], DXL_LED, 4|2|1);
        }
        for (int i=6; i<12; i++) {
            dxl_write_byte(mapping[i], DXL_LED, 0);
        }
    }
}

/**
 * Changes the mapping of the leg, the input is the direction
 * (between 0 and 3)
 */
static void remap(int direction)
{
    for (int i=0; i<12; i++) {
        mapping[i] = servos_order[(i+3*direction)%12];
    }

    colorize();
}

TERMINAL_COMMAND(remap,
        "Changes the mapping")
{
    remap(atoi(argv[0]));
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
    dxl_pidp(64);

    locomotion_init();

    // Initializing the DXL bus
    delay(500);
    dxl_init();

    // Initializing mapping
    remap(0);

    // Initializing config (see config.h)
    config_init();

    // Initializing positions to 0
    for (int i=0; i<12; i++) {
        dxl_set_position(servos_order[i], 0.0);
    }
    for (int i=0; i<4; i++) {
        l1[i] = l2[i] = l3[i] = 0;
    }

    // Initializing rhock
    rhock_vm_init();
}

/**
 * Computing the servo values
 */
void tick()
{
    // Computing average voltage
    static int idToRead = 0;
    static int blink;

    idToRead++;
    if (idToRead >= 12) idToRead = 0;
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
    t += freq*0.02;
    if (t > 1.0) {
        t -= 1.0;
    }
    if (t < 0.0) t += 1.0;

    locomotion_tick(t);
   
    // Sending order to servos
    dxl_set_position(mapping[0], l1[0]);
    dxl_set_position(mapping[3], l1[1]);
    dxl_set_position(mapping[6], l1[2]);
    dxl_set_position(mapping[9], l1[3]);

    dxl_set_position(mapping[1], l2[0]);
    dxl_set_position(mapping[4], l2[1]);
    dxl_set_position(mapping[7], l2[2]);
    dxl_set_position(mapping[10], l2[3]);

    dxl_set_position(mapping[2], l3[0]);
    dxl_set_position(mapping[5], l3[1]);
    dxl_set_position(mapping[8], l3[2]);
    dxl_set_position(mapping[11], l3[3]);
}

void loop()
{
    if (rhock_mode) {
        while (terminal_io()->io->available()) {
            rhock_stream_recv(terminal_io()->io->read());
        }
    }
}
