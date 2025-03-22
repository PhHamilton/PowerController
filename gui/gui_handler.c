#include "gui_handler.h"
#include <stdlib.h>

UBYTE *black_image;
UWORD image_size = (OLED_1in5_RGB_WIDTH * 2) * OLED_1in5_RGB_HEIGHT;

gui_state_t gui_state = GUI_NOT_UPDATING;

void update_parameter(gui_parameters_t *param);
void compose_string(float voltage, float current, char str[MAX_STRING_SIZE]);
void compose_voltage_string(float voltage, char v_string[MAX_STRING_SIZE/2]);
void compose_current_string(float current, char c_string[MAX_STRING_SIZE/2]);
void draw_arrow(uint8_t pos);

gui_error_codes_t initialize_gui(void)
{

    if(DEV_ModuleInit() != 0)
    {
        return GUI_ERROR;
    }

    if(USE_IIC)
    {
        return GUI_ERROR;
    }

    OLED_1in5_rgb_Init();

    if((black_image = (UBYTE *)malloc(image_size + 300)) == NULL)
    {
        printf("Failed to apply for black memory...\r\n");
        return GUI_ERROR;
    }

    return GUI_OK;
}
gui_state_t get_gui_state()
{
    return gui_state;
}

gui_error_codes_t update_gui(gui_parameters_t *param)
{
    gui_state = GUI_UPDATING;
    Paint_NewImage(black_image, OLED_1in5_RGB_WIDTH, OLED_1in5_RGB_HEIGHT, 0, BLACK);
    Paint_SetScale(65);
    Paint_SelectImage(black_image);
    Paint_Clear(BLACK);

    Paint_DrawString_EN(50, 0, "PSU", &Font16, BLACK, HEADING_PSU_COLOR);
    Paint_DrawString_EN(10, 20, "Controller", &Font16, BLACK, HEADING_CONTROLLER_COLOR);

    update_parameter(param);

    OLED_1in5_rgb_Display(black_image);

    gui_state = GUI_NOT_UPDATING;

    return GUI_OK;
}

void update_parameter(gui_parameters_t *param)
{
    char str[MAX_STRING_SIZE];
    uint16_t color;

    draw_arrow(param->cursor_position);

    for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++)
    {
        compose_string(param->measurements[i].voltage,
                       param->measurements[i].current,
                       str);

        if(param->measurements[i].output_state == OUTPUT_ACTIVE)
        {
            color = GREEN;
        }
        else
        {
            color = RED;
        }

        Paint_DrawCircle(CIRCLE_X, START_Y + i * PADY, CIRCLE_RADIUS, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
        Paint_DrawCircle(CIRCLE_X, START_Y + i * PADY, CIRCLE_RADIUS, CIRCLE_COLOR, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
        Paint_DrawString_EN(TEXT_X, START_Y - CIRCLE_RADIUS + i * PADY, str, &Font12, BLACK, TEXT_COLOR);
    }

}

void compose_string(float voltage, float current, char str[MAX_STRING_SIZE])
{
    char voltage_str[MAX_STRING_SIZE/2];
    char current_str[MAX_STRING_SIZE/2];

    compose_voltage_string(voltage, voltage_str);
    compose_current_string(current, current_str);

    sprintf(str, "%s %s", voltage_str, current_str);
}

void draw_arrow(uint8_t pos)
{
    Paint_DrawLine(ARROW_X, START_Y - ARROW_HEIGHT + PADY * pos, ARROW_X + ARROW_LENGTH, START_Y + PADY * pos, ARROW_COLOR, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(ARROW_X + ARROW_LENGTH, START_Y + PADY * pos, ARROW_X, START_Y + ARROW_HEIGHT + PADY * pos, ARROW_COLOR, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
}

void compose_voltage_string(float voltage, char v_string[MAX_STRING_SIZE/2])
{
    if(voltage < 10)
    {
        sprintf(v_string, "%0.1f V", voltage);
    }
    else
    {
        sprintf(v_string, "%i  V", (uint8_t) voltage);
    }
}

void compose_current_string(float current, char c_string[MAX_STRING_SIZE/2])
{
    if(current >= 10)
    {
        sprintf(c_string, "%.1f A", current);
    }
    else if(current < 10 && current >= 1)
    {
        sprintf(c_string, "%.2f A", current);
    }
    else
    {
        if(current*1000 < 100)
        {
            sprintf(c_string, "%i   mA", (uint8_t)(current * 1000));
        }
        else
        {
            sprintf(c_string, "%i  mA", (uint8_t)(current * 1000));
        }
    }

}
