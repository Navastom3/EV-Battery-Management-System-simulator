#ifndef CONTROL_H
#define CONTROL_H

#include <stdint.h>

typedef enum
{
    FULL_POWER,
    LOW_POWER

} PowerMode;

/*
 * Updates the current control mode based on button input.
 * A single click switches control between the motor and LEDs.
 * Returns 0 for motor control and 1 for LED control.
 */
uint8_t update_control(void);

/*
 * Gets the current power mode of the system.
 * Power mode is toggled between FULL_POWER and LOW_POWER
 * through a double click of the user button.
 */
PowerMode get_power_mode(void);

#endif
