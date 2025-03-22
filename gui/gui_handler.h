#ifndef __GUI_HANDLER_H__
#define __GUI_HANDLER_H__

#include "OLED_1in5_rgb.h"
#include "GUI_Paint.h"

#define NUMBER_OF_CHANNELS 3
#define MAX_STRING_SIZE    50

#define PADX 10
#define PADY 20

#define CIRCLE_RADIUS 5
#define ARROW_HEIGHT  CIRCLE_RADIUS
#define ARROW_LENGTH  CIRCLE_RADIUS

#define START_X 5
#define START_Y 60

#define HEADING_PSU_COLOR        YELLOW
#define HEADING_CONTROLLER_COLOR YELLOW
#define TEXT_COLOR               YELLOW
#define ARROW_COLOR              YELLOW
#define CIRCLE_COLOR             YELLOW

#define ARROW_X  START_X
#define CIRCLE_X ARROW_X  + 2 * PADX
#define TEXT_X   CIRCLE_X + PADX

typedef enum
{
    GUI_ERROR,
    GUI_OK
}gui_error_codes_t;

typedef enum
{
    GUI_NOT_UPDATING,
    GUI_UPDATING
}gui_state_t;

typedef struct
{
    uint8_t cursor_position;
    struct
    {
        float voltage;
        float current;
        enum
        {
            OUTPUT_ACTIVE,
            OUTPUT_INACTIVE
        }output_state;

    }measurements[NUMBER_OF_CHANNELS];
}gui_parameters_t;

gui_error_codes_t initialize_gui(void);
gui_error_codes_t update_gui(gui_parameters_t *param);
gui_state_t get_gui_state();

#endif
