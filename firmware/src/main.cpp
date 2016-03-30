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
#include "opticals.h"

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
    RC.begin(115200);
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
    // This disable Serial2 from APB1
    RCC_BASE->APB1ENR &= ~RCC_APB1ENR_USART2EN;

    // Initializing terminal on the RC port
    RC.begin(115200);
    terminal_init(&RC);

    // Initializing bluetooth module
    bt_init();

    // Leds
    leds_init();

    // Initializing
    motion_init();

    // Initializing the DXL bus
    delay(500);

    // initializing distance
    distance_init();

    // Initializing the IMU
    //imu_init();

    // Configuring board LED as output
    pinMode(BOARD_LED_PIN, OUTPUT);
    digitalWrite(BOARD_LED_PIN, LOW);
    
    // Enabling opticals
    opticals_init();

    // Initializing the buzzer, and playing the start-up melody
    buzzer_init();
    buzzer_play(MELODY_BOOT);

    // Initizaliting DC
    dc_init();

    RC.begin(115200);
}

/**
 * Computing the servo values
 */
void tick()
{
    if (!move || !started) {
        led_set_all(LED_R);
        dc_command(0, 0, 0);
        t = 0.0;
        return;
    }

    // Incrementing and normalizing t
    t += motion_get_f()*0.02;
    if (t > 1.0) {
        t -= 1.0;
        led_set_all(LED_G);
    }
    if (t < 0.0) t += 1.0;

    motion_tick(t);
}

TERMINAL_COMMAND(mot, "Motor test")
{
    pinMode(15, PWM);
    pwmWrite(15, atoi(argv[0]));
    delay(1000);
    pwmWrite(15, 0);

    /*
#define MOTA 9
#define MOTB 27
    pwmWrite(MOTA, 0);
    pwmWrite(MOTB, 0);
    pinMode(MOTA, PWM);
    pinMode(MOTB, PWM);
    int o = atoi(argv[0]);
    if (o < 0) {
        pwmWrite(MOTA, -o);
    } else {
        pwmWrite(MOTB, o);
    }
    while (!SerialUSB.available()) {
        terminal_io()->println(timer.getCount());
        delay(100);
    }
    delay(1000);
    pwmWrite(MOTA, 0);
    pwmWrite(MOTB, 0);
    */
}

void loop()
{
    // Buzzer update
    buzzer_tick();
    // IMU ticking
    //imu_tick();

    // Updating the terminal
    terminal_tick();
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
        tick();
    }
}
