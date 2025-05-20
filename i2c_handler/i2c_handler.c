#include "i2c_handler.h"
#include <wiringPiI2C.h>

int i2c_write_register(uint8_t addr, uint8_t reg, uint16_t value)
{
    int fd = wiringPiI2CSetup(addr);

    if(fd < 0) return -1;

    if(wiringPiI2CWriteReg16(fd, reg, value) < 0) return -1;

    return 0;
}
int i2c_read_register(uint8_t addr, uint8_t reg, uint16_t *value)
{
    int fd = wiringPiI2CSetup(addr);

    if(fd < 0) return -1;

    *value = wiringPiI2CReadReg16(fd, reg);
    return 0;
}
