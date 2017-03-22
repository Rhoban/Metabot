#ifndef _BT_H
#define _BT_H

#define BT_HC05
// #define BT_HM12

#if defined(BT_HC05)
#define BT_BAUD 921600
#elif defined(BT_HM12)
#define BT_BAUD 230400
#else
#error "You must choose a BT chip"
#endif

/**
 * Initializes the bluetooth
 */
void bt_init();

/**
 * Configures the bluetooth (name=device name, pin=pair pin)
 */
void bt_set_config(char *name, char *pin);

#endif
