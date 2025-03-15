#include "gui_handler.h"
#include <stdlib.h>

UBYTE *black_image;
UWORD image_size = (OLED_1in5_RGB_WIDTH * 2) * OLED_1in5_RGB_HEIGHT;

typedef struct
{
    uint8_t vertical_spacing;
    uint8_t horizontal_spacing;
    uint8_t pad_x;
    uint8_t pad_y;
    uint8_t radius;
}gui_settings_t;

gui_settings_t settings;

void update_parameter(gui_parameters_t *param);
void compose_string(measurement_data_t *data, char str[MAX_STRING_SIZE]);
void compose_voltage_string(float voltage, char v_string[MAX_STRING_SIZE/2]);
void compose_current_string(float current, char c_string[MAX_STRING_SIZE/2]);

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

gui_error_codes_t update_gui(gui_parameters_t *param)
{
    Paint_NewImage(black_image, OLED_1in5_RGB_WIDTH, OLED_1in5_RGB_HEIGHT, 0, BLACK);
    Paint_SetScale(65);
    Paint_SelectImage(black_image);
    Paint_Clear(BLACK);

    Paint_DrawString_EN(10, 0, "Power Supply Controller", &Font8, BLACK, YELLOW);

    update_parameter(param);

    OLED_1in5_rgb_Display(black_image);

    return GUI_OK;
}

void update_parameter(gui_parameters_t *param)
{
    char str[MAX_STRING_SIZE];
    uint16_t color;

    for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++)
    {
        compose_string(&param->measurements[i], str);

        if(param->measurements[i].output_state == OUTPUT_ACTIVE)
        {
            color = GREEN;
        }
        else
        {
            color = RED;
        }

        Paint_DrawCircle(10, 50 + i * 20, 5, color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
        Paint_DrawCircle(10, 50 + i * 20, 5, YELLOW, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
        Paint_DrawString_EN(20, 45 + i* 20, str, &Font12, BLACK, YELLOW);
    }

}

void compose_string(measurement_data_t *data, char str[MAX_STRING_SIZE])
{
    char voltage_str[MAX_STRING_SIZE/2];
    char current_str[MAX_STRING_SIZE/2];

    compose_voltage_string(data->voltage, voltage_str);
    compose_current_string(data->current, current_str);

    sprintf(str, "%s %s", voltage_str, current_str);
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
