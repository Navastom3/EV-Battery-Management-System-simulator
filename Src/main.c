#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cell.h"
#include "bms_action.h"
#include <windows.h>

void display_the_info(Cell c);

void display_the_info(Cell c) {
    printf("voltage : %.2f V\n", c.voltage);
    printf("current : %.2f mA\n", c.current); 
    printf("temperature : %.2f °C\n", c.temp);    
    printf("charge: %.2f %%\n", c.state_of_charge); 
}

int main(){

  Cell c = get_cell_data();
//Main loop that repeats until the system is active every ten secons, checking the cell data and updating the cell parameters.

//v1 Note: The delay is set to 10 seconds for simulation purposes. In the final implementation, the update rate will be adjusted based on the system requirements
//and the frequency of sensor readings.
//Additionally, on the current windows simulator, SHUTDOWN protocol breaks the loop then returns 0.
//When implemented on v2 with the STM32, a safe shutdown routine will be adapted acordingly 

  while(c.state != SHUTDOWN){
   
    update_cell(&c);
    display_the_info(c);
    c.state = select_mode(c);
    execute_state(&c);

    Sleep(10000); //Delay for 10 seconds 

   } 
   return 0;
}
