#ifndef __CURRENT_HANDLER_H__
#define __CURRENT_HANDLER_H__

#include <stdint.h>
#include <stdbool.h>
#include "ina219.h"
#include "main.h"

typedef struct
{
    uint8_t address;
    INA219_t value;
}current_parameters_t;

int32_t get_current(uint8_t addr);

#endif //__CURRENT_HANDLER_H__
