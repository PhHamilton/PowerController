#ifndef __INA219_H__
#define __INA219_H__
#include <math.h>
#include <wiringPiI2C.h>
#include <unistd.h>
#include <stdint.h>

#define INA219_ADDRESS_1 0x40
#define INA219_ADDRESS_2 0x41
#define INA219_ADDRESS_3 0x44

typedef struct {
    uint8_t address;
    float shunt_resistance;
    float current_lsb;
    float power_lsb;
    uint16_t calibration_value;
} INA219_config_t;

typedef struct {
    float voltage; // in volts
    float current; // in amperes
    float power;   // in watts
} INA219_data_t;

typedef struct
{
    INA219_config_t config;
    INA219_data_t data;
}INA219_channel_handler_t;

typedef enum
{
    INA219_OK = 0,
    INA219_ERROR_I2C = -1,
    INA219_WARNING_OLD_MEASUREMENT = -2,
    INA219_WARNING_MATH_OVERFLOW = -3,
    INA219_ERROR = -4
}INA219_STATUS_t;

INA219_STATUS_t ina219_init(INA219_config_t *conf);
INA219_STATUS_t ina219_read(INA219_config_t *conf, INA219_data_t *data);

/*
#include <stdint.h>
#include <stddef.h>
#include "i2c_handler.h"
#include "main.h"

#define SHUNT_RESISTANCE 0.1f

typedef struct {
    uint8_t address;
    float shunt_resistance;
    float current_lsb;
    float power_lsb;
    uint16_t calibration_value;
}INA219_config_t;

typedef struct {
    float voltage; // in volts
    float current; // in amperes
    float power;   // in watts
}INA219_data_t;

typedef struct
{
    struct
    {
       INA219_config_t config;
       INA219_data_t data;
    }channel[NUMBER_OF_CHANNELS];
}INA219_channel_handler_t;


INA219_STATUS_t ina219_initialize(INA219_config_t *conf);
INA219_STATUS_t ina219_read(INA219_channel_handler_t *channel_handler, uint8_t channel);
*/

#endif //__INA219_H__
