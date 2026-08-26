#include "bms_action.h"
#include <stdio.h>


//v1 Note : The following functions are designed to determine the current battery state based on the sensor readings and execute the actions/behaviors based on the state of the cell.
//The temperature thresholds will have to be updated based on the temperature sensor reading baselines
BMS_State select_mode(Cell c) {

    if(c.temp >= 60 || c.state_of_charge == 0.0f)
        return SHUTDOWN;

    else if(c.temp >= 55)
        return FAULT;

    else if(c.temp >= 45 || c.state_of_charge <= 20)
        return WARNING;

    else 

        return NORMAL;

}

void execute_state(Cell *c) {

 int current_limit;
 // Sets the allowable current limit based on the current BMS state.
// Future STM32 integration will scale the 12-bit ADC potentiometer reading (0-4095) to this limit.
    switch(c->state)

    {
        case NORMAL:
            current_limit = 300;
            printf("State : NORMAL\n");
            break;

        case WARNING:
            current_limit = 200;
            printf("State : WARNING\n");
            if(c->state_of_charge <= 20){
                printf("Warning: Battery is low. Please recharge the battery soon.\n");
            }
            else{
                printf("Warning: Battery temperature is high. Please check the cooling system or reduce the current with the potentiometer.\n");
            }
            break;

        case FAULT:
            current_limit = 50;
            printf("State : FAULT\n");
            break;

         case SHUTDOWN:
            current_limit = 0;
          if(c->state_of_charge == 0){
            printf("Battery is fully discharged. Please recharge the battery before attempting new operation.\n");
          }
          else{
            printf("The system shutdown due to critical temperature conditions\n");
          }

            break;
    }
}                       