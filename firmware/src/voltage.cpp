#include "voltage.h"
#include <terminal.h>
#include "buzzer.h"

bool voltage_is_error;
int voltage_now;
int voltage_limit_l;
int voltage_limit_h;
int voltage_limit_s;

float voltage_spv()
{
    float stepPerVolt = 4095/3.3;
    stepPerVolt *= VOLTAGE_R2/(float)(VOLTAGE_R1+VOLTAGE_R2);

    return stepPerVolt;
}

void voltage_init()
{
    pinMode(VOLTAGE_PIN, INPUT_FLOATING);
    
    float stepPerVolt = voltage_spv();

    voltage_limit_l = VOLTAGE_LIMIT*stepPerVolt;
    voltage_limit_h = VOLTAGE_LIMIT*stepPerVolt + stepPerVolt;
    voltage_limit_s = VOLTAGE_SHUT*stepPerVolt;
    voltage_now = analogRead(VOLTAGE_PIN);
    voltage_is_error = false;
}

void voltage_tick()
{
    int newSample = analogRead(VOLTAGE_PIN);
    if (newSample < voltage_now) voltage_now--;
    if (newSample > voltage_now) voltage_now++;

    if (voltage_is_error) {
        if (voltage_now > voltage_limit_h || voltage_now < voltage_limit_s) {
            voltage_is_error = false;
            buzzer_stop();
        }
    } else {
        if (voltage_now < voltage_limit_l && voltage_now > voltage_limit_s) {
            voltage_is_error = true;
            buzzer_play(MELODY_ALERT, true);
        }
    }
}

float voltage_current()
{
    return voltage_now / voltage_spv();
}

TERMINAL_COMMAND(voltage, "Get the voltage")
{
    terminal_io()->print("voltage=");
    terminal_io()->println(voltage_current());
}
