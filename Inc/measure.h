#ifndef MEASURE_H
#define MEASURE_H

/*
 * Measures the current flowing through the battery system.
 * Uses the INA219 current sensor through I2C.
 * Returns the measured current in amperes.
 */
float measure_current(void);

/*
 * Measures the temperature of the battery system.
 * Uses the TMP102 temperature sensor through I2C.
 * Returns the measured temperature in degrees Celsius.
 */
float measure_temp(void);

/*
 * Measures the battery voltage.
 * Uses the INA219 bus voltage measurement.
 * Returns the measured voltage in volts.
 */
float measure_voltage(void);

#endif
