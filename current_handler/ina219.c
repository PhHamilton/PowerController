#include "ina219.h"

INA219_STATUS_t ina219_calibrate(INA219_t *conf);
INA219_STATUS_t ina219_get_voltage(INA219_t *conf);
INA219_STATUS_t ina219_get_current(INA219_t *conf);
INA219_STATUS_t ina219_get_power(INA219_t *conf);

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

INA219_STATUS_t ina219_initialize(INA219_t *conf)
{
    return INA219_OK;
}

INA219_STATUS_t ina219_turn_on_readings(INA219_t *conf)
{
    uint16_t raw = default_config | 0x07;

    if(i2c_write_register(conf->address, CONFIGURATION, raw) != 0)
    {
        return INA219_ERROR_I2C;
    }

    return INA219_OK;
}

INA219_STATUS_t ina219_turn_off_readings(INA219_t *conf)
{
    uint16_t raw = default_config | 0x00;

    if(i2c_write_register(conf->address, CONFIGURATION, raw) != 0)
    {
        return INA219_ERROR_I2C;
    }

    return INA219_OK;
}

INA219_STATUS_t ina219_measure(INA219_t *conf)
{
    INA219_STATUS_t rc = INA219_OK;
    rc = ina219_get_voltage(conf);

    if(rc != INA219_OK)
    {
        return rc;
    }

    rc = ina219_get_current(conf);

    if(rc != INA219_OK)
    {
        return rc;
    }

    return ina219_get_power(conf);
}

INA219_STATUS_t ina219_reset(void)
{
    return INA219_OK;
}

INA219_STATUS_t ina219_get_voltage(INA219_t *conf)
{
    uint16_t raw = 0;
    if(i2c_read_register(conf->address, BUS_VOLTAGE, &raw) != 0)
    {
        return INA219_ERROR_I2C;
    }

    conf->voltage = ((uint8_t)(raw >> 3) * 0.004f);

    if(raw & 0x01)
        return INA219_WARNING_MATH_OVERFLOW;

    if(raw & 0x02)
        return INA219_WARNING_OLD_MEASUREMENT;

    return INA219_OK;
}

INA219_STATUS_t ina219_get_current(INA219_t *conf)
{
    uint16_t raw = 0;
    if(i2c_read_register(conf->address, CURRENT, &raw) != 0)
    {
        return INA219_ERROR_I2C;
    }

    conf->current = (int16_t)raw * 1000.0f;

    return INA219_OK;
}

INA219_STATUS_t ina219_get_power(INA219_t *conf)
{
    uint16_t raw = 0;
    if(i2c_read_register(conf->address, POWER, &raw) != 0)
    {
        return INA219_ERROR_I2C;
    }

    conf->power = raw;

    return INA219_OK;
}
