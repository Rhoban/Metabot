#ifdef HAS_DXL
#include <dxl.h>
#endif

/**
 * This is the dimension of the legs, in mm
 */
#define L0      55
#define L1      40
#define L2      62
#define L3_1    85
#define L3_2    35

/**
 * Servos are supposed to be like this:
 *
 *   2nd leg   3
 *     |       2 <--- 1st leg
 *     v       1
 *       6-5-4   10-11-11
 *             7    ^
 *   3rd leg-> 8    |
 *             9   4th leg
 *
 * But you can change these IDs using the below array:
 */
static int servos_order[] = {
    // First leg
    1, 2, 3,            
    // Second leg
    4, 5, 6,            
    // Third leg
    7, 8, 9,          
    // Fourth leg
    10, 11, 12,
    // Fifth leg
    13, 14, 15,
    // Sixth leg
    16, 17, 18
};

/**
 * You can also change the signs of the servos if you mount
 * it backward for instance
 *
 * Change the 1 to -1 if the servos are backwards
 */
static int signs[] = {
    1,          // Signs of the body motors
    1,          // Sign of the middle-leg motors
    1           // Sign of the end-legs motors
};

#ifdef HAS_DXL
/**
 * This is the servos configuration, you can change the zero 
 * or the min/max limits here
 */
static void config_init()
{
    // First leg
    dxl_set_zero    (servos_order[0], 0.00);
    dxl_set_min_max (servos_order[0], -90, 90);
    dxl_set_zero    (servos_order[1], 0.00);
    dxl_set_min_max (servos_order[1], -100, 100);
    dxl_set_zero    (servos_order[2], 0.00);
    dxl_set_min_max (servos_order[2], -145, 145);

    // Second leg
    dxl_set_zero    (servos_order[3], 0.00);
    dxl_set_min_max (servos_order[3], -90, 90);
    dxl_set_zero    (servos_order[4], 0.00);
    dxl_set_min_max (servos_order[4], -100, 100);
    dxl_set_zero    (servos_order[5], 0.00);
    dxl_set_min_max (servos_order[5], -145, 145);

    // Third leg
    dxl_set_zero    (servos_order[6], 0.00);
    dxl_set_min_max (servos_order[6], -90, 90);
    dxl_set_zero    (servos_order[7], 0.00);
    dxl_set_min_max (servos_order[7], -100, 100);
    dxl_set_zero    (servos_order[8], 0.00);
    dxl_set_min_max (servos_order[8], -145, 145);

    // Fourth leg
    dxl_set_zero    (servos_order[9], 0.00);
    dxl_set_min_max (servos_order[9], -90, 90);
    dxl_set_zero    (servos_order[10], 0.00);
    dxl_set_min_max (servos_order[10], -100, 100);
    dxl_set_zero    (servos_order[11], 0);
    dxl_set_min_max (servos_order[11], -145, 145);
    
    // Fifth leg
    dxl_set_zero    (servos_order[12], 0.00);
    dxl_set_min_max (servos_order[12], -90, 90);
    dxl_set_zero    (servos_order[13], 0.00);
    dxl_set_min_max (servos_order[13], -100, 100);
    dxl_set_zero    (servos_order[14], 0);
    dxl_set_min_max (servos_order[14], -145, 145);
    
    // Sixth leg
    dxl_set_zero    (servos_order[15], 0.00);
    dxl_set_min_max (servos_order[15], -90, 90);
    dxl_set_zero    (servos_order[16], 0.00);
    dxl_set_min_max (servos_order[16], -100, 100);
    dxl_set_zero    (servos_order[17], 0);
    dxl_set_min_max (servos_order[17], -145, 145);
}
#endif
