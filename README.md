# EV-Battery-Management-System logic simulator
C-based battery management system simulator with state monitoring, protection logic, and planned STM32 sensor and motor-control integration.

## Overview

This project is a C-based battery management system simulator that I am developing before moving the logic onto an STM32.

The goal of the current version is to build and test the basic BMS logic separately from the hardware. The program represents a battery cell using voltage, current, temperature, state of charge, and an operating state, then decides how the system should react based on those values.

The current version is a **desktop simulation**. Sensor measurements are still represented using arbitrary values. The next step is to move the project into STM32CubeIDE and start replacing those values with real sensor and ADC readings.

## Current Version

**V1 – Desktop C Simulation**

The current version includes:

* A battery cell represented using a C `struct`
* Voltage, current, temperature, state of charge, and BMS state
* Four operating states:

  * `NORMAL`
  * `WARNING`
  * `FAULT`
  * `SHUTDOWN`
* State transitions based on temperature and state of charge
* Different output-limit values depending on the BMS state
* Console messages showing how the system reacts
* Simulated state-of-charge decrease over time
* Separate `.c` and `.h` files for the different parts of the program

The sensor values are currently hard-coded for testing. They will later be replaced by functions that read the STM32 hardware.

The output limits in V1 are also only software placeholders. They are not currently controlling real current or hardware. In the STM32 version, I plan to use these states to limit the allowed motor output and control external LEDs.

## BMS State Logic

For now, I am using temporary thresholds so that I can test the state machine:

* `NORMAL` – Temperature below 45°C and SOC above 20%
* `WARNING` – Temperature is 45°C or higher, or SOC is 20% or lower
* `FAULT` – Temperature is 55°C or higher
* `SHUTDOWN` – Temperature is 60°C or higher, or SOC reaches 0%

These are **demonstration values**, not real battery safety limits.

The thresholds are currently chosen so that the different states can be tested easily. Once the project uses an actual battery system, they would need to be changed based on the battery chemistry, cell specifications, and sensor calibration.

Each state also has a different placeholder output limit:

* `NORMAL` – 300
* `WARNING` – 200
* `FAULT` – 50
* `SHUTDOWN` – 0

These values are only being used to represent progressively tighter limits between the different states. They are not calibrated current values yet.

## Program Structure

I split the project into multiple files to keep the main parts separate:

* `main.c` runs the main loop
* `cell.c / cell.h` stores the cell data and updates values such as state of charge
* `bms_action.c / bms_action.h` handles the state logic and the behavior for each state
* `bms_types.h` defines the different BMS states
* `measure.c / measure.h` will later contain the hardware measurement functions

The program repeatedly updates the cell, checks its current conditions, selects a BMS state, and then runs the behavior for that state.

For V1, the cell begins with temporary values such as:

```c
c.voltage = 3.3;
c.current = 160.0;
c.temp = 40.0;
c.state_of_charge = 100.0;
c.state = NORMAL;
```

The state of charge is then reduced slightly during each loop iteration to simulate battery discharge.

A 10-second delay is used in the desktop version so that the changing values are easier to observe in the terminal. This is only for simulation and will be changed when the project moves onto the STM32.

## Planned STM32 Integration

The next major step is moving the project into STM32CubeIDE.

The planned prototype will use:

* STM32 NUCLEO-F401RE
* Temperature sensor
* Potentiometer connected to the STM32 ADC
* LEDs for visual state indication
* PWM output
* DC motor controlled through a transistor
* Flyback diode
* Separate 9 V motor supply

For the first hardware version, the temperature sensor will be used as a **controllable test input** rather than as a real battery-temperature measurement.

For example, I can manually warm the sensor to test whether the software correctly moves through:

NORMAL to WARNING to FAULT to SHUTDOWN with some manually changeable temperatures I can achieve in my own environment.

The potentiometer will also be used as an adjustable ADC input. Its 12-bit ADC reading will represent the requested motor output, and the current BMS state will limit how much output is allowed. It will simulate the "Acceleration of the motor" as well since it allows us to manually control it. This way, when the current limit is set, it will consequently also limit the acceleration of the motor.


This means the first STM32 version will demonstrate the BMS protection logic through motor PWM rather than real closed-loop battery current limiting. Actual current measurement would require additional sensing hardware.

The motor-control circuit is being developed separately first so that I can test the PWM, ADC, transistor, and motor behavior before connecting it to the BMS logic.

The STM32 version will also handle `SHUTDOWN` differently from the desktop simulator. Instead of ending the program, the MCU will remain running while the controlled output is disabled so that the system can continue monitoring its inputs.

## Next Steps

* Port the current C logic into STM32CubeIDE
* Replace the simulated temperature with a real sensor reading
* Read the potentiometer using the STM32 12-bit ADC
* Connect the BMS states to PWM output limits
* Add LED indication for the different states
* Add blinking behavior for warning and shutdown
* Integrate the STM32 motor-control circuit
* Test each state transition using controlled inputs
* Determine a more appropriate update rate for the embedded version
* Replace the demonstration thresholds with values based on an actual battery system

## What I Learned

So far, this project has helped me get more comfortable splitting a C program across multiple source and header files instead of keeping everything inside `main.c`.

One thing that gave me trouble was passing the `Cell` struct between functions. I kept mixing up when I needed `.` and when I needed `->`, and debugging that helped pointers and structs make a lot more sense to me.

I also learned how separate `.c` files are compiled and linked together, which I had not really dealt with before this project.

Another thing I had to think about was the difference between a measurement and a control value. A measured current should represent what a sensor actually reads, while an output limit represents what the BMS allows the rest of the system to do. That distinction is something I plan to keep clearer when I move the project onto the STM32.

## Disclaimer

This is an educational prototype for learning embedded systems and battery-management concepts.

It has not been designed or validated for real battery protection or safety-critical use.
