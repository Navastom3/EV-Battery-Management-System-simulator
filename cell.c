//Cell.c includes the implementation of a single cell simulator, including the class and metrics
//v2 note : Measure current, voltage and temp implemented
/*v2 note : state of charge is still being simulated since the system is still running on a 9V battery
 * and not a fully fleshed cell system where I have access to the battery charge
 */

#include "cell.h"
#include "bms_types.h"
#include "measure.h"
#include <stdio.h>

Cell get_cell_data(void)
{
    Cell c;

    c.voltage = measure_voltage();
    c.current = measure_current();
    c.temp = measure_temp();
    c.state_of_charge = 100.00f;
    c.state = NORMAL;

    return c;
}

void update_cell(Cell *c)
{

    c->voltage = measure_voltage();
    c->current = measure_current();
    c->temp = measure_temp();

    c->state_of_charge -= 0.01f;
}
