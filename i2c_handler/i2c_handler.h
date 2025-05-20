#ifndef __I2C_HANDLER_H__
#define __I2C_HANDLER_H__

#include <stdint.h>

int i2c_write_register(uint8_t addr, uint8_t reg, uint16_t value);
int i2c_read_register(uint8_t addr, uint8_t reg, uint16_t *value);

#endif //__I2C_HANDLER_H__
