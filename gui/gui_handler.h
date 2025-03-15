#ifndef __GUI_HANDLER_H__
#define __GUI_HANDLER_H__

#include "OLED_1in5_rgb.h"
#include "GUI_Paint.h"

#define NUMBER_OF_CHANNELS 3
#define MAX_STRING_SIZE    50

typedef enum
{
    GUI_ERROR,
    GUI_OK
}gui_error_codes_t;

typedef struct
{
    float voltage;
    float current;
    enum
    {
        OUTPUT_ACTIVE,
        OUTPUT_INACTIVE
    }output_state;

}measurement_data_t;

typedef struct
{
    uint8_t cursor_position;
    measurement_data_t measurements[NUMBER_OF_CHANNELS];
}gui_parameters_t;

gui_error_codes_t initialize_gui(void);
gui_error_codes_t update_gui(gui_parameters_t *param);

#endif
