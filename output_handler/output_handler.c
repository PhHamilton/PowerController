#include "output_handler.h"
#include "wiringPi.h"

typedef struct
{
    uint8_t pin_number;
    output_state_t state;
}output_settings_t;

bool turn_on_output(uint8_t output_number);
bool turn_off_output(uint8_t output_number);

output_settings_t outputs[NUMBER_OF_CHANNELS];

bool initialize_output_handler(const uint8_t pin_numbers[NUMBER_OF_CHANNELS])
{


    printf("Initializing pins:\n");
    for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++)
    {
        outputs[i].pin_number = pin_numbers[i];

        pinMode(i, OUTPUT);

        printf(" Pin %i .. ", outputs[i].pin_number);
        if(!turn_off_output(i))
        {
            printf("FAILURE\n");
            return false;
        }
        printf("OK\n");
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
    digitalWrite(outputs[output_number].pin_number, OUTPUT_DISABLED);
    if(digitalRead(outputs[output_number].pin_number) == OUTPUT_DISABLED)
    {
        outputs[output_number].state = OUTPUT_DISABLED;
        return true;
    }
    else
    {
        return false;
    }

}

bool turn_on_output(uint8_t output_number)
{
    digitalWrite(outputs[output_number].pin_number, OUTPUT_ENABLED);

    if(digitalRead(outputs[output_number].pin_number) == OUTPUT_ENABLED)
    {
        outputs[output_number].state = OUTPUT_ENABLED;
        return true;
    }
    else
    {
        return false;
    }
}
