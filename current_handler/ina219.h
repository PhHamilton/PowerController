#ifndef __INA219_H__
#define __INA219_H__

#include <stdint.h>
#include "i2c_handler.h"
#include "main.h"

#define SHUNT_RESISTANCE 0.1

//extern int i2c_write_register(uint8_t addr, uint8_t reg, uint16_t value);
//extern int i2c_read_register(uint8_t addr, uint8_t reg, uint16_t *value);

typedef struct
{
    float shunt_resistance;

    struct
    {
        uint8_t address;
        float max_current;
        float current_lsb;
        float power_lsb;
        uint16_t calibration_value;
   }channel_config[NUMBER_OF_CHANNELS];

    struct
    {
        float voltage;
        float current;
        float power;
    }channel_data[NUMBER_OF_CHANNELS];
}INA219_t;

typedef enum
{
    INA219_OK = 0,
    INA219_ERROR_I2C = -1,
    INA219_WARNING_OLD_MEASUREMENT = -2,
    INA219_WARNING_MATH_OVERFLOW = -3
}INA219_STATUS_t;

INA219_STATUS_t ina219_calibrate(INA219_t *conf, uint8_t channel);
INA219_STATUS_t ina219_measure(INA219_t *conf, uint8_t channel);
INA219_STATUS_t ina219_turn_on_readings(INA219_t *conf, uint8_t channel);
INA219_STATUS_t ina219_turn_off_readings(INA219_t *conf, uint8_t channel);
INA219_STATUS_t ina219_reset(INA219_t *conf);

#endif //__INA219_H__
