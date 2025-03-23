#ifndef __OUTPUT_HANDLER_H__
#define __OUTPUT_HANDLER_H__

#include <stdint.h>
#include <stdbool.h>

#define N_CHANNELS 3

typedef enum
{
    OUTPUT_DISABLED,
    OUTPUT_ENABLED
}output_state_t;

bool initialize_output_handler(uint8_t pin_numbers[N_CHANNELS]);
bool change_output_state(uint8_t output_number, output_state_t state);
bool get_output_state(uint8_t output_number);
bool reset_all_outputs(void);

#endif //__OUTPUT_HANDLER_H__
