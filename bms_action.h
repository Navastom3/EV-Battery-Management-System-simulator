#ifndef BMS_ACTION_H
#define BMS_ACTION_H

#include "cell.h"
#include "bms_types.h"



/*

 * Determines the current battery state based on the sensor readings

 */

BMS_State select_mode(Cell cell);

/*

 * Executes the actions/behaviors based on the state of the cell.

 * Adjusts limits or disables operation if necessary.

 */

void execute_state(Cell *cell);

#endif