#include "bms_action.h"
#include "tim.h"
#include "control.h"
#include "adc.h"
#include <stdio.h>
#include <stdint.h>
#include "main.h"

//v1 Note : The following functions are designed to determine the current battery state based on the sensor readings and execute the actions/behaviors based on the state of the cell.
//The temperature thresholds will have to be updated based on the temperature sensor reading baselines
BMS_State select_mode(Cell c) {

    if(c.temp > 40 || c.state_of_charge <= 0.0f) //v2 note : the temperatures were changed so that
        return SHUTDOWN;                          //They can be recreated intentionally by placing my finger (around 30C)
                                                  //OR placing the battery on top of the INA219 (40C)

    else if(c.temp >= 30)
        return FAULT;

    else if(c.temp >= 35 || c.state_of_charge <= 20)
        return WARNING;

    else 

        return NORMAL;

}

float execute_state(Cell *c) {
 // v2 note : Since I have no actual way of controlling the current from the battery based on the mode, I decided to control
 //the motor throttle instead
 //This function will give a throttle percentage for each state that will then be multiplied by the ADC raw value taken in main
 //Additionally, the introduction of Power Modes in v2, also allows the user to manually regulate the flow by activating low power mode

	float throttle_state = 0.0f;

    switch(c->state)

    {
        case NORMAL:
        	throttle_state = 1.0f;
            printf("State : NORMAL\n");
            break;

        case WARNING:
        	throttle_state = 2.0f/3.0f;
            printf("State : WARNING\n");
            if(c->state_of_charge <= 20){
                printf("Warning: Battery is low. Please recharge the battery soon.\n");
            }
            else{
                printf("Warning: Battery temperature is high. Please check the cooling system or reduce the throttle with the potentiometer.\n");
            }
            break;

        case FAULT:
        	throttle_state = 1.0f/6.0f;
            printf("State : FAULT\n");
            break;

         case SHUTDOWN:
         //Print the reason for shutdown, turn everything off, and exit the program

            throttle_state = 0;
          if(c->state_of_charge <= 0.0f){
            printf("Battery is fully discharged. Please recharge the battery before attempting new operation.\n");
          }
          else{
            printf("The system shutdown due to critical temperature conditions\n");
          }

          //Shutdown animation
          int shutdown_animation_done = 0;
          if (!shutdown_animation_done)
                     {
                         for (uint8_t i = 0; i < 2; i++)
                         {
                             __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 4095);
                             HAL_Delay(150);

                             __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
                             HAL_Delay(150);
                         }

                         shutdown_animation_done = 1;
                     }

          __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0); // Motor off
          __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0); // Red LED off
          __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0); // Green LED off

        return 0;

    }

           return throttle_state;
    }

