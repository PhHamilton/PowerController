#ifndef __MQTT_MESSAGE_HANDLER_H__
#define __MQTT_MESSAGE_HANDLER_H__

#include "stdio.h"
#include "gui_handler.h"
extern gui_parameters_t gui_parameters;

typedef enum
{
    VOLTAGE_3V3,
    VOLTAGE_5V,
    VOLTAGE_12V,
    VOLTAGE_NEG12V,
    ALL
}status_request_type_t;

void status_update_handler(const char* topic, const char* message);
void output_update_handler(const char* topic, const char* message);

#endif //__MQTT_MESSAGE_HANDLER_H__
