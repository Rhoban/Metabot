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
#include <rhock.h>
#include "motors.h"
#include "voltage.h"
#include "buzzer.h"
#include "distance.h"
#include "config.h"
#include "motion.h"
#include "leds.h"
#include "mapping.h"
#include "behavior.h"
#include "imu.h"
#include "bt.h"

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
    RC.begin(BT_BAUD);
    terminal_init(&RC);
    isUSB = false;
}

// Enabling/disabling move
TERMINAL_PARAMETER_BOOL(move, "Enable/Disable move", true);

// This destroys the fuse (used in dev)
/*
TERMINAL_COMMAND(suicide, "Lit the fuse")
{
    digitalWrite(LIT, HIGH);
}
*/

// Setting the flag, called @50hz
bool flag = false;
void setFlag()
{
    flag = true;
}

/**
 * Checks wether there is enough voltage to start the motors
 */
bool can_start()
{
    if (voltage_current() < 6 || voltage_error()) {
        buzzer_play(MELODY_WARNING);
        return false;
    }

    return true;
}

/**
 * Initializing
 */
void setup()
{
    // Input button
    pinMode(BOARD_BUTTON_PIN, INPUT);

    // Initializing terminal on the RC port
    RC.begin(BT_BAUD);
    terminal_init(&RC);

    // Lit pin is output low
    digitalWrite(LIT, LOW);
    pinMode(LIT, OUTPUT);

    // Initializing bluetooth module
    bt_init();

    // Initializing
    motion_init();

    // Initializing voltage measurement
    voltage_init();

    // Initializing the DXL bus
    delay(500);
    dxl_init();

    // Initializing config (see config.h)
    config_init();

    // initializing distance
    distance_init();

    // Initializing the IMU
    imu_init();

    // Initializing positions to 0
    for (int i=0; i<12; i++) {
        dxl_set_position(i+1, 0.0);
    }
    for (int i=0; i<4; i++) {
        l1[i] = l2[i] = l3[i] = 0;
    }

    // Configuring board LED as output
    pinMode(BOARD_LED_PIN, OUTPUT);
    digitalWrite(BOARD_LED_PIN, LOW);

    // Initializing the buzzer, and playing the start-up melody
    buzzer_init();
    buzzer_play(MELODY_BOOT);

    // Enable 50hz ticking
    servos_init();
    servos_attach_interrupt(setFlag);

    RC.begin(BT_BAUD);
}

/**
 * Computing the servo values
 */
void tick()
{
    static bool wasMoving = false;

    if (voltage_error()) {
        // If there is a voltage error, blinks the LEDs orange and
        // stop any motor activity
        dxl_disable_all();
#ifdef RHOCK
        rhock_killall();
#endif
        if (t < 0.5) {
            led_set_all(LED_R|LED_G);
        } else {
            led_set_all(0);
        }
        t += 0.02;
        if (t > 1.0) t -= 1.0;
        return;
    }

    // The robot is disabled
    if (!move || !started) {
        motors_read();
        t = 0.0;
        return;
    }

    // Incrementing and normalizing t
    t += motion_get_f()*0.02;
    if (t > 1.0) {
        t -= 1.0;
        colorize();
    }
    if (t < 0.0) t += 1.0;

    if (!wasMoving && motion_is_moving()) {
        t = 0.0;
    }
    wasMoving = motion_is_moving();

    // Robot behavior
    behavior_tick(0.02);

    motion_tick(t);

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

static int btnLast = 0;
static bool btn = false;
static int id = 13;

void buttonPress()
{
    btnLast = millis();
}

void idAudio()
{
    for (int k=0; k<id; k++) {
        buzzer_beep(400, 50);
        buzzer_wait_play();
        delay(150);
    }
}

/**
 * The button is used to do some configurations, for more information
 * please refer to the official documentation
 */
void buttonRelease()
{
    if (millis()-btnLast > 6000) {
        // Pressed more than 6s, configuring bluetooth
        buzzer_play(MELODY_BEGIN);
        buzzer_wait_play();
        bt_set_config("Metabot", "1234");
        buzzer_play(MELODY_BEGIN);
        buzzer_wait_play();
    } else if (millis()-btnLast > 2000) {
        // Pressed more than 2s, entering ID sequence
        id = 1;
        buzzer_play(MELODY_BEGIN);
        buzzer_wait_play();
    } else {
        // Next id sequence
        if (id <= 12) {
            int success = 0;

            // Checking that there is exactly one servo on the bus
            int discover = 0;
            for (int k=1; k<=12; k++) {
                bool ok = false;
                for (int t=0; t<3; t++) {
                    if (dxl_ping(k)) {
                        ok = true;
                    }
                    delay(3);
                }
                if (ok) {
                    discover++;
                }
            }

            // Configuring the servo on the bus to the target ID
            // (using broadcast as address)
            if (discover == 1) {
                for (int k=0; k<3; k++) {
                    dxl_configure(DXL_BROADCAST, id);
                    dxl_write_byte(id, DXL_LED, LED_G);
                    delay(30);
                    // Checking that the servo now is correct
                    if (dxl_ping(id)) {
                        success++;
                    }
                }
            }

            // Next ID
            if (success > 0) {
                idAudio();
                id++;

                if (id > 12) {
                    // It's over
                    delay(500);
                    buzzer_play(MELODY_BEGIN);
                    buzzer_wait_play();
                }
            } else {
                buzzer_play(MELODY_WARNING);
                buzzer_wait_play();
            }
        }
    }
}

void loop()
{
    bool btnNew = digitalRead(BOARD_BUTTON_PIN);
    if (btnNew != btn) {
        if (btnNew) buttonPress();
        else buttonRelease();
        btn = btnNew;
    }

    // Buzzer update
    buzzer_tick();
    // IMU ticking
    imu_tick();
    // Sampling the voltage
    voltage_tick();
    // Sampling the distance
    distance_tick();

    // Updating the terminal
    terminal_tick();
#if defined(RHOCK)
    rhock_tick();
#endif
    if (SerialUSB.available() && !isUSB) {
        isUSB = true;
        terminal_init(&SerialUSB);
    }
    if (RC.available() && isUSB) {
        isUSB = false;
        terminal_init(&RC);
    }

    // Calling user motion tick
    if (flag) {
        flag = false;
        tick();
        dxl_flush();
    }
}
