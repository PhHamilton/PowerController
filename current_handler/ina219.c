#include "ina219.h"

INA219_STATUS_t ina219_get_voltage(INA219_t *conf, uint8_t channel);
INA219_STATUS_t ina219_get_current(INA219_t *conf, uint8_t channel);
INA219_STATUS_t ina219_get_power(INA219_t *conf, uint8_t channel);

static uint16_t default_config = 0x399F;

typedef enum
{
    CONFIGURATION = 0x00,
    SHUNT_VOLTAGE = 0x01,
    BUS_VOLTAGE   = 0x02,
    POWER         = 0x03,
    CURRENT       = 0x04,
    CALIBRATION   = 0x05
}INA219_REG_ADDR_t;

INA219_STATUS_t ina219_calibrate(INA219_t *conf, uint8_t channel)
{
    conf->shunt_resistance = SHUNT_RESISTANCE;

    float current_lsb = conf->channel_config[channel].max_current / 32767;
    float power_lsb = current_lsb * 20.0f;
    uint16_t calibration_value = (uint16_t)(0.04096f / (current_lsb * SHUNT_RESISTANCE));


    conf->channel_config[channel].current_lsb = current_lsb;
    conf->channel_config[channel].power_lsb = power_lsb;
    conf->channel_config[channel].calibration_value = calibration_value;

    if(i2c_write_register(conf->channel_config[channel].address, CALIBRATION, calibration_value) != 0)
    {
        return INA219_ERROR_I2C;
    }

    return INA219_OK;
}

INA219_STATUS_t ina219_turn_on_readings(INA219_t *conf, uint8_t channel)
{
    uint16_t raw = default_config | 0x07;

    if(i2c_write_register(conf->channel_config[channel].address, CONFIGURATION, raw) != 0)
    {
        return INA219_ERROR_I2C;
    }

    return INA219_OK;
}

INA219_STATUS_t ina219_turn_off_readings(INA219_t *conf, uint8_t channel)
{
    uint16_t raw = default_config | 0x00;

    if(i2c_write_register(conf->channel_config[channel].address, CONFIGURATION, raw) != 0)
    {
        return INA219_ERROR_I2C;
    }

    return INA219_OK;
}

INA219_STATUS_t ina219_measure(INA219_t *conf, uint8_t channel)
{
    INA219_STATUS_t rc = INA219_OK;
    rc = ina219_get_voltage(conf, channel);

    if(rc != INA219_OK)
    {
        return rc;
    }

    rc = ina219_get_current(conf, channel);

    if(rc != INA219_OK)
    {
        return rc;
    }

    return ina219_get_power(conf, channel);
}

INA219_STATUS_t ina219_reset(INA219_t *conf)
{
    for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++)
    {
        if(i2c_write_register(conf->channel_config[i].address, CONFIGURATION, 0x8000) != 0)
            return INA219_ERROR_I2C;
    }
    return INA219_OK;
}

INA219_STATUS_t ina219_get_voltage(INA219_t *conf, uint8_t channel)
{
    uint16_t raw = 0;
    if(i2c_read_register(conf->channel_config[channel].address, BUS_VOLTAGE, &raw) != 0)
    {
        return INA219_ERROR_I2C;
    }

    conf->channel_data[channel].voltage = ((uint8_t)(raw >> 3) * 0.004f);

    if(raw & 0x01)
        return INA219_WARNING_MATH_OVERFLOW;

    if(raw & 0x02)
        return INA219_WARNING_OLD_MEASUREMENT;

    return INA219_OK;
}

INA219_STATUS_t ina219_get_current(INA219_t *conf, uint8_t channel)
{
    uint16_t raw = 0;
    if(i2c_read_register(conf->channel_config[channel].address, CURRENT, &raw) != 0)
    {
        return INA219_ERROR_I2C;
    }

    conf->channel_data[channel].current = (int16_t)raw * conf->channel_config[channel].current_lsb;

    return INA219_OK;
}

INA219_STATUS_t ina219_get_power(INA219_t *conf, uint8_t channel)
{
    uint16_t raw = 0;
    if(i2c_read_register(conf->channel_config[channel].address, POWER, &raw) != 0)
    {
        return INA219_ERROR_I2C;
    }

    conf->channel_data[channel].power = conf->channel_config[channel].power_lsb;

    return INA219_OK;
}
