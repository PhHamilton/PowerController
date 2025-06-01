#include "output_handler.h"
#include "gpiod.h"

#define CHIP_NAME "gpiochip0"
struct gpiod_chip *chip;
struct gpiod_line *line[NUMBER_OF_CHANNELS];

typedef struct
{
    uint8_t pin_number;
    output_state_t state;
}output_settings_t;

bool turn_on_output(uint8_t output_number);
bool turn_off_output(uint8_t output_number);
void cleanup_outputs(void);

output_settings_t outputs[NUMBER_OF_CHANNELS];

bool initialize_output_handler(const uint8_t pin_numbers[NUMBER_OF_CHANNELS])
{
    printf("Initializing pins:\n");
    chip = gpiod_chip_open_by_name(CHIP_NAME);

    if(!chip)
    {
        printf("Unable to open %s\n", CHIP_NAME);
        return false;
    }

    for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++)
    {
        line[i] = gpiod_chip_get_line(chip, pin_numbers[i]);

        if(!line[i])
        {
            fprintf(stderr, "Failed to get line for GPIO %d\n", pin_numbers[i]);
            return false;
        }

        if(gpiod_line_request_output(line[i], "output_handler", 0) < 0)
        {
            perror("gpiod_line_request_output");
            return false;
        }

        outputs[i].pin_number = pin_numbers[i];
        outputs[i].state = OUTPUT_DISABLED;
    }

    return true;
}

bool change_output_state(uint8_t output_number, output_state_t state)
{
    switch(state)
    {
        case OUTPUT_DISABLED:
        {
            return turn_off_output(output_number);
        }
        break;
        case OUTPUT_ENABLED:
        {
            return turn_on_output(output_number);
        }
        break;
        default:
        {
            return false;
        }
        break;
    }

}

output_state_t get_output_state(uint8_t output_number)
{
    return outputs[output_number].state;
}


bool reset_all_outputs(void)
{
    for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++)
    {
        if(!turn_off_output(i))
        {
            return false;
        }
    }

    return true;
}

bool turn_off_output(uint8_t output_number)
{
    if(output_number > NUMBER_OF_CHANNELS)
    {
        printf("output number out of range!\n");
        return false;
    }

    if(gpiod_line_set_value(line[output_number], 0) < 0)
    {
        perror("gpiod_line_set_value (OFF)");
        return false;
    }

    outputs[output_number].state = OUTPUT_DISABLED;

    return true;
}

bool turn_on_output(uint8_t output_number)
{
    if(output_number > NUMBER_OF_CHANNELS)
    {
        printf("output number out of range!\n");
        return false;
    }

    if(gpiod_line_set_value(line[output_number], 1) < 0)
    {
        perror("gpiod_line_set_value (ON)");
        return false;
    }

    outputs[output_number].state = OUTPUT_ENABLED;

    return true;
}
void cleanup_outputs(void)
{
    for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++)
    {
        if(line[i])
        {
            gpiod_line_release(line[i]);
        }
    }
    gpiod_chip_close(chip);
}
