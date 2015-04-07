#include <math.h>
#include "kinematic.h"

/**
 * Solves the al Kashi problem
 *
 * This gives the angles of a triangle side, knowing
 * all the lengths of its sides.
 */
float alKashi(float a, float b, float c)
{
    return acos(((a*a)+(b*b)-(c*c))/(2*a*b));
}

/**
 * Compute the kinematics of the robot
 */
bool computeIK(float x, float y, float z,
        float *a, float *b, float *c,
        float l1, float l2, float l3)
{
    // Alpha is simply the angle of the leg in the X/Y plane
    float alpha = atan2(y, x);

    // Distance between end of the leg and arm of the second motor,
    // in the X/Y plane
    float xp = sqrt(x*x+y*y)-l1;
    if (xp < 0) {
        xp = 0;
    }

    // Distance between second motor arm and the end of the leg,
    // in the plane of the leg
    float d = sqrt(pow(xp,2) + pow(z,2));
    if (d > l2+l3) {
        d = l2+l3;
    }

    // Knowing l2, l3 and d beta and gamma can be computed using
    // the Al Kashi law
    float beta = alKashi(l2, d, l3) - atan2(-z, xp);
    float gamma = M_PI - alKashi(l2, l3, d);

    if (!isnan(alpha) && !isnan(beta) && !isnan(gamma)) {
        *a = RAD2DEG(alpha);
        *b = RAD2DEG(beta);
        *c = RAD2DEG(gamma);
        return true;
    } else {
        return false;
    }
}

void legFrame(float X, float Y, float *x, float *y, int leg, float l0)
{
    switch (leg) {
        case 0:
            *x = cos(0*M_PI/2-M_PI/4)*X - sin(0*M_PI/2-M_PI/4)*Y - l0;
            *y = sin(0*M_PI/2-M_PI/4)*X + cos(0*M_PI/2-M_PI/4)*Y;
        break;
        case 1:
            *x = cos(1*M_PI/2-M_PI/4)*X - sin(1*M_PI/2-M_PI/4)*Y - l0;
            *y = sin(1*M_PI/2-M_PI/4)*X + cos(1*M_PI/2-M_PI/4)*Y;
        break;
        case 2:
            *x = cos(2*M_PI/2-M_PI/4)*X - sin(2*M_PI/2-M_PI/4)*Y - l0;
            *y = sin(2*M_PI/2-M_PI/4)*X + cos(2*M_PI/2-M_PI/4)*Y;
        break;
        case 3:
            *x = cos(3*M_PI/2-M_PI/4)*X - sin(3*M_PI/2-M_PI/4)*Y - l0;
            *y = sin(3*M_PI/2-M_PI/4)*X + cos(3*M_PI/2-M_PI/4)*Y;
        break;
    }
}
