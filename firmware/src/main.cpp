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
#include "kinematic.h"
#include "config.h"

// Amplitude multiplier
#define AMPLITUDE 30

// This is the servo mappings
ui8 mapping[12];

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

// Legs bacakward mode
TERMINAL_PARAMETER_BOOL(backLegs, "Legs backwards", false);

// Body backward mode
TERMINAL_PARAMETER_BOOL(back, "Mode back", false);

// Enabling/disabling move
TERMINAL_PARAMETER_BOOL(move, "Enable/Disable move", true);

// Time
TERMINAL_PARAMETER_FLOAT(t, "Time", 0.0);

// Speed factor
TERMINAL_PARAMETER_FLOAT(freq, "Time factor gain", 2.0);

// Amplitude & altitude of the robot
TERMINAL_PARAMETER_FLOAT(alt, "Height of the steps", 15.0);

// Static position
TERMINAL_PARAMETER_FLOAT(r, "Robot size", 90.0);
TERMINAL_PARAMETER_FLOAT(h, "Robot height", -55.0);

// Direction vector
TERMINAL_PARAMETER_FLOAT(dx, "Dx", 0.0);
TERMINAL_PARAMETER_FLOAT(dy, "Dy", 0.0);
TERMINAL_PARAMETER_FLOAT(crab, "Crab", 0.0);

// Turning, in ° per step
TERMINAL_PARAMETER_FLOAT(turn, "Turn", 0.0);

// Front delta h
TERMINAL_PARAMETER_FLOAT(frontH, "Front delta H", 0.0);

TERMINAL_PARAMETER_FLOAT(Ax, "Ax", 0.0);
TERMINAL_PARAMETER_FLOAT(Ay, "Ay", 0.0);
TERMINAL_PARAMETER_FLOAT(Az, "Az", 0.0);

TERMINAL_COMMAND(version, "Getting firmware version")
{
    terminal_io()->println(METABOT_VERSION);
}

TERMINAL_COMMAND(toggleBackLegs, "Toggle back legs")
{
    if (backLegs == 0) backLegs = 1;
    else if (backLegs == 1) backLegs = 0;
}

TERMINAL_COMMAND(toggleCrab, "Toggle crab mode")
{
    if (crab == 0) crab = 30;
    else if (crab == 30) crab = 0;
}

// Gait selector
#define GAIT_WALK       0
#define GAIT_TROT       1
TERMINAL_PARAMETER_INT(gait, "Gait (0:walk, 1:trot)", GAIT_TROT);

// Functions
Function rise;
Function step;

/**
 * Initializing functions
 */
void setup_functions()
{
    rise.clear();
    step.clear();
    
    if (gait == GAIT_WALK) {
        // Rising the legs
        rise.addPoint(0.0, 0.0);
        rise.addPoint(0.1, 1.0);
        rise.addPoint(0.3, 1.0);
        rise.addPoint(0.35, 0.0);
        rise.addPoint(1.0, 0.0);

        // Taking the leg forward
        step.addPoint(0.0, -0.5);
        step.addPoint(0.12, -0.5);
        step.addPoint(0.3, 0.5);
        step.addPoint(0.35, 0.5);
        step.addPoint(1.0, -0.5);
    }

    if (gait == GAIT_TROT) {
        // Rising the legs
        rise.addPoint(0.0, 0.0);
        rise.addPoint(0.1, 1.0);
        rise.addPoint(0.4, 1.0);
        rise.addPoint(0.5, 0.0);
        rise.addPoint(1.0, 0.0);

        // Taking the leg forward
        step.addPoint(0.0, -0.5);
        step.addPoint(0.1, -0.5);
        step.addPoint(0.4, 0.5);
        step.addPoint(1.0, -0.5);
    }
}

TERMINAL_PARAMETER_FLOAT(kkk, "", 0.0);
TERMINAL_PARAMETER_FLOAT(smoothBackLegs, "Smooth 180", 0.0);
TERMINAL_PARAMETER_FLOAT(smoothBack, "Smooth Back", -1.0);

TERMINAL_PARAMETER_INT(voltage, "Average voltage (dV)", 75);

float l1[4];
float l2[4];
float l3[4];

/**
 * Initializing
 */
void setup()
{
    RC.begin(921600);
    dxl_pidp(64);

    back = (initialOrientation != 0);
    if (back) smoothBack = 1;

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

    // Setting up functions
    setup_functions();

    // Incrementing and normalizing t
    t += freq*0.02;
    if (t > 1.0) {
        t -= 1.0;
        colorize();
    }
    if (t < 0.0) t += 1.0;

    // Smoothing 180
    if (backLegs && smoothBackLegs < 1) {
        smoothBackLegs += 0.02;
    }
    if (!backLegs && smoothBackLegs > 0) {
        smoothBackLegs -= 0.02;
    }
    if (back && smoothBack < 1) {
        smoothBack += 0.04;
    }
    if (!back && smoothBack > -1) {
        smoothBack -= 0.04;
    }
        
    for (int i=0; i<4; i++) {
        // Defining in which group of opposite legs this leg is
        bool group = ((i&1)==1);

        // This defines the phase of the gait
        float legPhase;
       
        if (gait == GAIT_WALK) {
            float phases[] = {0.0, 0.5, 0.75, 0.25};
            legPhase = t + phases[i];
        }
        if (gait == GAIT_TROT) {
            legPhase = t + group*0.5;
        }

        float x, y, z, a, b, c;

        // Computing the order in the referencial of the body
        float xOrder = step.getMod(legPhase)*dx;
        float yOrder = step.getMod(legPhase)*dy;

        // Computing the order in the referencial of the leg
        float bodyAngle = -(i*M_PI/2.0 - (M_PI/4.0))*smoothBack;
        if (group) {
            bodyAngle -= DEG2RAD(crab*(-smoothBack));
        } else {
            bodyAngle += DEG2RAD(crab*(-smoothBack));
        }
        float vx = xOrder*cos(bodyAngle)-yOrder*sin(bodyAngle);
        float vy = xOrder*sin(bodyAngle)+yOrder*cos(bodyAngle);

        float enableRise = (abs(dx)>0.5 || abs(dy)>0.5 || abs(turn)>5) ? 1 : 0;

        // This is the x,y,z order in the referencial of the leg
        x = r + vx;
        y = vy;
        z = h + rise.getMod(legPhase)*alt*enableRise;
        if (i < 2) z += frontH;
    
        // Computing inverse kinematics
        if (computeIK(x, y, z, &a, &b, &c, L1, L2, backLegs ? L3_2 : L3_1)) {
            if (group) {
                a += crab*(-smoothBack);
            } else {
                a -= crab*(-smoothBack);
            }

            l1[i] = signs[0]*smoothBack*(a + step.getMod(legPhase)*turn);
            l2[i] = signs[1]*smoothBack*(b);
            l3[i] = signs[2]*smoothBack*(c - 180*smoothBackLegs);
        }

        if (i == 0) {
            Ax = x;
            Ay = y;
            Az = z;
        }
    }
   
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
}
