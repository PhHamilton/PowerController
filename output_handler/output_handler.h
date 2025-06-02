#ifndef __OUTPUT_HANDLER_H__
#define __OUTPUT_HANDLER_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "main.h"

typedef enum
{
    OUTPUT_ENABLED,
    OUTPUT_DISABLED
}output_state_t;

bool initialize_output_handler(const uint8_t pin_numbers[NUMBER_OF_CHANNELS]);

bool change_output_state(uint8_t output_number, output_state_t state);
output_state_t get_output_state(uint8_t output_number);
bool reset_all_outputs(void);
void cleanup_outputs(void);

#endif //__OUTPUT_HANDLER_H__
