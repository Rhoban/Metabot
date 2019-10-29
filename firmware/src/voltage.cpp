#include "voltage.h"
#include <terminal.h>
#include "buzzer.h"

bool voltage_is_error;
bool voltage_is_error_fast;
int voltage_error_start = 0;
int voltage_now;
int voltage_limit_l;
int voltage_limit_h;
int voltage_limit_s;

bool voltage_error()
{
    return voltage_is_error;
}

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
    voltage_limit_h = VOLTAGE_LIMIT*stepPerVolt + stepPerVolt*0.4;
    voltage_limit_s = VOLTAGE_SHUT*stepPerVolt;
    voltage_now = analogRead(VOLTAGE_PIN);
    voltage_is_error = false;
}

int voltage_last_ts = 0;

void voltage_tick()
{
    if (millis() - voltage_last_ts > 10) {
        voltage_last_ts = millis();
        int newSample = analogRead(VOLTAGE_PIN);
        if (newSample < voltage_now) voltage_now--;
        if (newSample > voltage_now) voltage_now++;

        if (voltage_is_error) {
            if (voltage_now > voltage_limit_h || voltage_now < voltage_limit_s) {
                voltage_is_error = false;
                buzzer_stop();
            } else {
                int duration = (millis() - voltage_error_start);

                if (duration > 45000) {
                    digitalWrite(LIT, HIGH);
                } else if (duration > 30000) {
                    if (!voltage_is_error_fast) {
                        voltage_is_error_fast = true;
                        buzzer_play(MELODY_ALERT_FAST, true);
                    }
                }
            }
        } else {
            if (voltage_now < voltage_limit_l && voltage_now > voltage_limit_s) {
                voltage_is_error = true;
                voltage_is_error_fast = false;
                voltage_error_start = millis();
                buzzer_play(MELODY_ALERT, true);
            }
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
    terminal_io()->println((int)(10*voltage_current()));
}
