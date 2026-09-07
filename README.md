## EV Battery Management System simulator v2.0- Hardware Integration

The STM32 version uses a NUCLEO-F401RE.

The TMP102 is read over I2C and provides the temperature value used by the BMS state logic.

The INA219 is also connected over I2C. Bus voltage is read directly from the bus voltage register. Current is calculated from the shunt voltage register using the onboard 0.1 ohm shunt resistor and Ohm's law.

A potentiometer is connected to the STM32 ADC and represents the requested motor or LED output with a PWM.

The onboard user button is configured as an external interrupt. A single click switches potentiometer control between the motor and LEDs, while a double click toggles between FULL_POWER and LOW_POWER.

UART output is used to monitor values such as voltage, current, temperature, state of charge, ADC input, control mode, throttle limit, and PWM output during testing.

Example:

Voltage: 7.84 V
Current: 0.327 A
Temperature: 24.31 C
Charge: 99.94 %
Power mode: FULL POWER
ADC: 3086
Control mode: MOTOR
Throttle limit: 1.00
PWM: 3086

## Program Structure

The project is separated into several modules:
─ bms_action.c
─ cell.c
─ control.c
─ measure.c
─ main.c

cell.c stores and updates the battery data.

measure.c handles the TMP102 and INA219 measurements.

bms_action.c contains the BMS state selection and output limiting logic.

control.c handles the button interrupt, single/double-click detection, power modes, and control switching.

main.c connects the different parts together and updates the ADC, PWM outputs, UART telemetry, sensors, and BMS state.

## Development Process

I intentionally developed the project in stages.

V1 was a desktop C simulation with hard-coded values and placeholder output limits. Its main purpose was to make sure the state logic and program structure worked before hardware was introduced.

V2 moved the same logic onto the STM32 and replaced the simulated values with sensor readings, ADC input, PWM output, interrupts, and UART debugging and mapped the values onto hardware outputs. It also allowed the user to control the intensity of LEDs and the motor and added a low power mode to further save battery.

Developing the software and hardware separately made it easier to isolate problems during integration instead of debugging the entire system at once.

Additionally, I changed the temperature thresholds that I had used in the previous software version into temperatures I could replicate on my own. 30C now caused fault and I could trigger it by putting my finger on the sensor for a couple seconds. 40C was the temperature it reached when I put the hot battery on top of the sensor so I used that to trigger a shutdown.

## Current Status

The temperature sensor, voltage measurement, ADC input, PWM generation, UART telemetry, state logic, button interrupt, control switching, and low-power mode were implemented and tested.

The INA219 current-measurement software was also completed, but the physical current path still produced inconsistent results compared with a multimeter.

During final motor integration, the MOSFET switching stage failed. The motor and STM32 PWM output had both been tested separately in my project before this, but the complete motor-driver stage was not successfully validated before the hardware stopped operating reliably.

Because of this, the final demo focuses on the parts of the STM32 system that were successfully validated.

## Challenges

### Challenge #1 : Motor Stage
One of the hardest parts of the project was figuring out whether a problem was coming from the code or the hardware since there was room for a lot of integration induced bugs.

The motor stage caused the most trouble because the PWM signal, MOSFET, wiring, and power supply all affected the final behavior. I tested the motor and PWM separately to narrow it down, but the MOSFET stage failed during the final integration.

### Challenge #2 : Current Sensor
The INA219 current reading was another issue. Voltage readings were working, but the current measurement did not agree with the multimeter. I went back through the datasheet and switched to their alternative approach, calculating current directly from the shunt-voltage register using Ohm's law.

## What I Learned

I think my favorite part of this project was going back to the datasheet and applying it directly to my project. It was oddly satisfying to find the solution to a problem directly from the manufacturer, almost like they had already anticipated the exact issue I was running into.

Also, integrating the `execute_state` and control logic really forced me to think about the order in which things needed to happen, especially since the logic was spread across different files. It taught me when separating functions makes the code easier to manage and when it can actually make things more complicated. I also had to think more carefully about which variables were being passed between functions and where they could be accessed.

Additionally, this project taught me to separate measurements from control values. For example, current represents what the sensor actually measures, while the throttle limit represents what the BMS allows the system to request.

The final motor-driver failure also reinforced the importance of validating hardware subsystems separately before full integration.

## Future Improvements

If I continue the project, the next priorities would be replacing the motor-driver MOSFET stage, using a more suitable motor power supply, validating the INA219 current path, improving the timing structure, and replacing the demonstration BMS thresholds with values based on a real battery system.

## Disclaimer

This is an educational prototype for learning embedded systems and battery-management concepts.

The state thresholds, state-of-charge model, and output limits are demonstration values and have not been validated for real battery protection or safety-critical use.

