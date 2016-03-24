#include <stdio.h>
#include <terminal.h>
#include <wirish.h>
#include "dc.h"

TERMINAL_PARAMETER_INT(kkk, "Debug", 0);

// XXX: Avoid using digitalWrite?
static HardwareTimer timer(4);

#define MOTORS 3

bool dcFlag = false;

struct dc_motor {
    int pinA;
    int pinB;
    float x, y;
};

struct dc_motor_pwm {
    int pin;
    int ts;
};

static struct dc_motor motors[MOTORS] = {
    {9, 27, 0.866, -0.5},
    {15, 16, -0.8660, -0.5},
    {2, 3, 0, 1}
};
static struct dc_motor_pwm motorsPWMFront[MOTORS];
static struct dc_motor_pwm motorsPWM[MOTORS];
static int current;
static int divider;
static bool dirty = false;
static bool irq = false;

bool dc_flag = false;

static void _dc_update();
static void dc_next(int count)
{
    while (current < MOTORS && motorsPWM[current].pin && motorsPWM[current].ts <= count) {
        digitalWrite(motorsPWM[current].pin, LOW);
        current++;
    }
    if (current < MOTORS && motorsPWM[current].pin) {
        timer.setCompare(TIMER_CH1, motorsPWM[current].ts);
        irq = true;
    } else {
        irq = false;
    }
}

static void _dc_update()
{
    if (irq) {
        dc_next(timer.getCount());
    }
}

static void _dc_overflow()
{
    irq = false;
    if (dirty) {
        dirty = false;
        for (int k=0; k<MOTORS; k++) {
            if (motorsPWM[k].pin) {
                digitalWrite(motorsPWM[k].pin, LOW);
            }
        }
        for (int k=0; k<MOTORS; k++) {
            motorsPWM[k] = motorsPWMFront[k];
        }
    }

    // Setting all the pins to HIGH
    for (int k=0; k<MOTORS; k++) {
        if (motorsPWM[k].pin && motorsPWM[k].ts) {
            digitalWrite(motorsPWM[k].pin, HIGH);
        }
    }
    current = 0;
    dc_next(0);

    // Ticking the flag
    divider++;
    if (divider >= 480) {
        divider = 0;
        dcFlag = true;
    }
}

void dc_init()
{
    for (int k=0; k<MOTORS; k++) {
        motorsPWM[k].pin = motorsPWMFront[k].pin = 0;
        digitalWrite(motors[k].pinA, LOW);
        digitalWrite(motors[k].pinB, LOW);
        pinMode(motors[k].pinA, OUTPUT);
        pinMode(motors[k].pinB, OUTPUT);
    }

    timer.pause();
    timer.setPrescaleFactor(1);
    timer.setOverflow(3000);
    timer.setCount(0);
    timer.setMode(TIMER_CH1, TIMER_OUTPUT_COMPARE);
    timer.setMode(TIMER_CH2, TIMER_OUTPUT_COMPARE);
    timer.setMode(TIMER_CH3, TIMER_OUTPUT_COMPARE);
    timer.setMode(TIMER_CH4, TIMER_OUTPUT_COMPARE);
    
    timer.attachCompare1Interrupt(_dc_update);
    timer.setCompare(TIMER_CH1, 0xffff);

    timer.setCompare(TIMER_CH4, 0);
    timer.attachCompare4Interrupt(_dc_overflow);
    timer.refresh();
    timer.resume();
}

void dc_bubble(int a, int b)
{
    if (motorsPWMFront[a].ts > motorsPWMFront[b].ts) {
        struct dc_motor_pwm tmp = motorsPWMFront[a];
        motorsPWMFront[a] = motorsPWMFront[b];
        motorsPWMFront[b] = tmp;
    }
}

void dc_sort()
{
    dc_bubble(1, 2);
    dc_bubble(0, 1);
    dc_bubble(1, 2);
}

int _min(int a, int b)
{
    return (a < b) ? a : b;
}

void dc_command(int m1, int m2, int m3)
{
    /*
    terminal_io()->print(m1); terminal_io()->print(" ");
    terminal_io()->print(m2); terminal_io()->print(" ");
    terminal_io()->print(m3); terminal_io()->print(" ");
    terminal_io()->println();
    */

    motorsPWMFront[0].pin = (m1 < 0) ? motors[0].pinA : motors[0].pinB;
    motorsPWMFront[0].ts = _min(3000, abs(m1));
    motorsPWMFront[1].pin = (m2 < 0) ? motors[1].pinA : motors[1].pinB;
    motorsPWMFront[1].ts = _min(3000, abs(m2));
    motorsPWMFront[2].pin = (m3 < 0) ? motors[2].pinA : motors[2].pinB;
    motorsPWMFront[2].ts = _min(3000, abs(m3));
    dc_sort();

    dirty = true;
}

static int s(int m)
{
    return (m > 0) ? 1 : -1;
}

void dc_xyt(float x, float y, float t)
{
    int m1 = (x*motors[0].x+y*motors[0].y)*80;
    int m2 = (x*motors[1].x+y*motors[1].y)*80;
    int m3 = (x*motors[2].x+y*motors[2].y)*80;

    m1 += t*80;
    m2 += t*80;
    m3 += t*80;

    /*
    if (m1 != 0) m1 += s(m1)*2000;
    if (m2 != 0) m2 += s(m2)*2000;
    if (m3 != 0) m3 += s(m3)*2000;
    */

    dc_command(m1, m2, m3);
}

TERMINAL_COMMAND(dc, "DC test")
{
    if (argc == 3) {
        dc_xyt(atof(argv[0]), atof(argv[1]), atof(argv[2]));
    }
}

TERMINAL_COMMAND(dcr, "DC test")
{
    if (argc == 3) {
        dc_command(atoi(argv[0]), atoi(argv[1]), atoi(argv[2]));
    }
}

TERMINAL_COMMAND(em, "em")
{
    dc_command(0, 0, 0);
}

TERMINAL_COMMAND(test, "Test")
{
    digitalWrite(atoi(argv[0]), HIGH);
    delay(1000);
    digitalWrite(atoi(argv[0]), LOW);
}
