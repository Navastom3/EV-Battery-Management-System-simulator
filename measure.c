
#include "measure.h"
#include "i2c.h"
#include <stdint.h>
#include <math.h>

//INA219 sensor defines
#define INA219_ADDR        (0x40 << 1)
#define INA219_REG_SHUNT   0x01
#define INA219_REG_BUS     0x02
#define INA219_SHUNT_LSB  0.00001f   // 10 uV/bit
#define INA219_R_SHUNT    0.1f       // R100 = 0.1 ohm
#define INA219_BUS_VOLTAGE_LSB    0.004f    // 4 mV/bit


//TMP102 sensor defines
#define TMP102_ADDR      (0x48 << 1)
#define TMP102_REG_TEMP  0x00

float measure_current(void)
{
    // INA219 takes Shunt voltage
    // With a 0.1-ohm shunt, each count corresponds to 0.0001 A.

    uint8_t data[2];

    //Return NAN if the measurement fails
    if (HAL_I2C_Mem_Read(&hi2c1, INA219_ADDR, INA219_REG_SHUNT, I2C_MEMADD_SIZE_8BIT, data, 2, 100) != HAL_OK){
        return NAN;
    }

    // Shunt voltage is signed because current can flow in either direction.
    int16_t raw_reading = (int16_t)(((uint16_t)data[0] << 8) | data[1]);

    float shunt_voltage = raw_reading*INA219_SHUNT_LSB;

    return shunt_voltage/INA219_R_SHUNT; //I_SHUNT = V_SHUNT/R_SHUNT
}

float measure_voltage(void)
{
    // INA219 bus voltage is stored in bits 15:3.
    // Each voltage count corresponds to 4 mV.
    uint8_t data[2];

    //Return NAN if the measurement fails
    if (HAL_I2C_Mem_Read(&hi2c1, INA219_ADDR, INA219_REG_BUS, I2C_MEMADD_SIZE_8BIT, data, 2, 100) != HAL_OK){
        return NAN;
    }

    uint16_t raw_reading =
        ((uint16_t)data[0] << 8) | data[1];

    // Discard the lower three non-voltage bits since they are not storing voltage.
    raw_reading >>= 3;

    return raw_reading * INA219_BUS_VOLTAGE_LSB;
}

float measure_temp(void)
{
    uint8_t data[2];

    if (HAL_I2C_Mem_Read(&hi2c1, TMP102_ADDR, TMP102_REG_TEMP, I2C_MEMADD_SIZE_8BIT, data, 2, 100) != HAL_OK){
        return NAN;
    }

    // Combine the two bytes into one signed 16-bit value
    int16_t raw_reading =
        (int16_t)(((uint16_t)data[0] << 8) | data[1]);

    // TMP102 temperature data occupies bits 15:4 in normal mode
    raw_reading >>= 4;

    // Sign-extend the 12-bit value for negative temperatures
    if (raw_reading & 0x0800) {
        raw_reading |= 0xF000;
    }

    // Each TMP102 temperature count = 0.0625 C
    return raw_reading * 0.0625f;
}

