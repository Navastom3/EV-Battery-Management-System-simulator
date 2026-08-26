//Cell.c includes the implementation of a single cell simulator, including the class and metrics

#include "cell.h"
#include "bms_types.h"

 Cell get_cell_data(void) {  
//Get the data from each sensor  

//v1 Note : arbitrary values are used for simulation purposes. 
//When implemented on v2 with the STM32, these values will be obtained from the sensors thanks to a measurement fucniton using the ADC.

    Cell c;
    c.voltage =  3.3 ;                  //measure_voltage(); 
    c.current =  160.00;             //measure_current();  
    c.temp = 40.0;                          // measure_temp();
    c.state_of_charge = 100.00;          //measure_soc();
    c.state = NORMAL;     
    
    return c; 
          
  }

void update_cell(Cell *c) {
  
//Each time the function is called, battery diminishes by 0.01%  
   c-> state_of_charge -= 0.01f;
  } 