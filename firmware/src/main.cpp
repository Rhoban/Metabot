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
#include "buzzer.h"
#include "distance.h"
#include "motion.h"
#include "leds.h"
#include "imu.h"
#include "bt.h"
#include "dc.h"
#include "mux.h"
#include "opticals.h"
#include "wheels.h"
#include "voltage.h"
#include "pulse.h"

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

// Enabling/disabling move
TERMINAL_PARAMETER_BOOL(move, "Enable/Disable move", true);

/**
 * Initializing
 */
void setup()
{
    init();

    // Pulse
    pulse_init();

    // This disable Serial2 from APB1
    RCC_BASE->APB1ENR &= ~RCC_APB1ENR_USART2EN;

    // Initializing terminal on the RC port
    RC.begin(921600);
    terminal_init(&RC);

    // Initializing bluetooth module
    bt_init();

    // Leds
    leds_init();

    // Initializing
    // motion_init();
    
    // Mux
    mux_init();

    // initializing distance
    distance_init();

    // Initializing the IMU
    imu_init();

    // Configuring board LED as output
    // TODO TEMP pinMode(BOARD_LED_PIN, OUTPUT);
    // TODO TEMP digitalWrite(BOARD_LED_PIN, LOW);
    
    // Enabling opticals
    opticals_init();

    // Initializing the buzzer, and playing the start-up melody
    buzzer_init();
    buzzer_play(MELODY_BOOT);
    RC.begin(921600);

    // Initizaliting DC
    dc_init();

    wheel_init();
    started = 1;
    // Voltage
    voltage_init();
}

/**
 * Computing the servo values
 */
void tick()
{
    leds_tick();
    if (!move || !started || voltage_error()) {
        dc_command(0, 0, 0);
        if (voltage_error()) {
            t += 0.01;
            if (t > 0.5) {
                t = 0;
            }
            if (t > 0.25) {
                led_set_mode(LEDS_OFF);
            } else {
                led_set_all(0);
                led_set_mode(LEDS_CUSTOM);
            }
        } else {
            led_set_mode(LEDS_OFF);
        }
        return;
    }

    // Incrementing and normalizing t
    t += motion_get_f()*0.01;
    if (t > 1.0) {
        t -= 1.0;
        // led_set_mode(LEDS_FRONT);
    }
    if (t < 0.0) t += 1.0;
    // motion_tick(t);
}

TERMINAL_COMMAND(mot, "Motor test")
{
    pinMode(15, PWM);
    pwmWrite(15, atoi(argv[0]));
    delay(1000);
    pwmWrite(15, 0);
}

void loop()
{
    // Buzzer update
    buzzer_tick();

    // IMU ticking
    imu_tick();

    // Updating the terminal
    terminal_tick();
    opticals_tick();
    // Updating voltage
    // XXX: Can be do less frequently
    voltage_tick();
    
    wheel_tick();

#if defined(RHOCK)
    rhock_tick();
#endif
    if (SerialUSB.available() && !isUSB) {
        isUSB = true;
        terminal_init(&SerialUSB);
    }
    if (!SerialUSB.getDTR() && isUSB) {
        isUSB = false;
        terminal_init(&RC);
    }

    // Calling user motion tick
    if (dcFlag) {
        dcFlag = false;
        // TODO TEMPS tick();
    }
}
