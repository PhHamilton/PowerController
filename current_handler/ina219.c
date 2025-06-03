#include "ina219.h"
#include <stdio.h>
#include <wiringPiI2C.h>

#define INA219_REG_CONFIG      0x00
#define INA219_REG_SHUNT_VOLT  0x01
#define INA219_REG_BUS_VOLT    0x02
#define INA219_REG_POWER       0x03
#define INA219_REG_CURRENT     0x04
#define INA219_REG_CALIB       0x05

#define INA219_CONFIG_DEFAULT  0x399F

static int write_register(uint8_t addr, uint8_t reg, uint16_t value) {
    int fd = wiringPiI2CSetup(addr);
    if (fd < 0) return -1;
    int hi = (value >> 8) & 0xFF;
    int lo = value & 0xFF;
    return wiringPiI2CWriteReg16(fd, reg, (lo << 8) | hi);
}

static int read_register(uint8_t addr, uint8_t reg, uint16_t* value) {
    int fd = wiringPiI2CSetup(addr);
    if (fd < 0) return -1;
    int raw = wiringPiI2CReadReg16(fd, reg);
    if (raw < 0) return -1;
    *value = ((raw & 0xFF) << 8) | (raw >> 8);
    return 0;
}

int ina219_init(INA219_config_t* conf) {
    if (!conf) return INA219_ERROR;

    // default config
    if (write_register(conf->address, INA219_REG_CONFIG, INA219_CONFIG_DEFAULT) < 0)
        return INA219_ERROR_I2C;

    conf->current_lsb = conf->shunt_resistance > 0 ? 1.0 / 1000.0 : 0.0001;
    conf->power_lsb = conf->current_lsb * 20;
    conf->calibration_value = (uint16_t)(0.04096f / (conf->current_lsb * conf->shunt_resistance));

    if (write_register(conf->address, INA219_REG_CALIB, conf->calibration_value) < 0)
        return INA219_ERROR_I2C;

    return INA219_OK;
}

int ina219_read(INA219_config_t* conf, INA219_data_t* data) {
    if (!conf || !data) return -1;

    uint16_t raw_bus, raw_current, raw_power;
    if (read_register(conf->address, INA219_REG_BUS_VOLT, &raw_bus) < 0) return -1;
    if (read_register(conf->address, INA219_REG_CURRENT, &raw_current) < 0) return -1;
    if (read_register(conf->address, INA219_REG_POWER, &raw_power) < 0) return -1;

    data->voltage = ((raw_bus >> 3) * 0.004f); // 4mV per bit
    data->current = (int16_t)raw_current * conf->current_lsb;
    data->power = raw_power * conf->power_lsb;

    return 0;
}
/*
static int read_register(INA219_config_t *conf, uint8_t reg, uint16_t *value);
static int write_register(INA219_config_t *conf, uint8_t reg, uint16_t value);

INA219_STATUS_t ina219_initialize(INA219_config_t *conf)
{
    if (!conf) return -1;

    conf->fd = wiringPiI2CSetup(conf->address);
    if (conf->fd < 0) return -1;

    conf->current_lsb = (conf->shunt_resistance > 0) ? 1.0f / 1000.0f : 0.0001f;
    conf->power_lsb = conf->current_lsb * 20.0f;
    conf->calibration_value = (uint16_t)(0.04096f / (conf->current_lsb * conf->shunt_resistance));

    if (write_register(conf, INA219_REG_CONFIG, INA219_CONFIG_DEFAULT) < 0)
        return -1;
    if (write_register(conf, INA219_REG_CALIB, conf->calibration_value) < 0)
        return -1;

    return 0;
}

INA219_STATUS_t ina219_read(INA219_channel_handler_t *channel_handler, uint8_t channel)
{
    if (!channel_handler) return -1;

    INA219_config_t *conf = &channel_handler->channel[channel].config;
    INA219_data_t *data = &channel_handler->channel[channel].data;

    uint16_t raw_bus, raw_current, raw_power;

    if (read_register(conf, INA219_REG_BUS_VOLT, &raw_bus) < 0) return -1;
    if (read_register(conf, INA219_REG_CURRENT, &raw_current) < 0) return -1;
    if (read_register(conf, INA219_REG_POWER, &raw_power) < 0) return -1;

    data->voltage = ((raw_bus >> 3) * 0.004f);
    data->current = (int16_t)raw_current * conf->current_lsb;
    data->power   = raw_power * conf->power_lsb;

    return 0;
}

static int write_register(INA219_config_t *conf, uint8_t reg, uint16_t value)
{
    int hi = (value >> 8) & 0xFF;
    int lo = value & 0xFF;
    return wiringPiI2CWriteReg16(conf->fd, reg, (lo << 8) | hi);
}

static int read_register(INA219_config_t *conf, uint8_t reg, uint16_t* value)
{
    int raw = wiringPiI2CReadReg16(conf->fd, reg);
    if (raw < 0) return -1;
    *value = ((raw & 0xFF) << 8) | (raw >> 8);
    return 0;
}
*/
