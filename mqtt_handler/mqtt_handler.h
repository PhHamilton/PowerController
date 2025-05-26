#ifndef __MQTT_HANDLER_H__
#define __MQTT_HANDLER_H__

typedef enum
{
    MQTT_HANDLER_OK,
    MQTT_HANDLER_ERROR
}mqtt_handler_status_t;

mqtt_handler_status_t initialize_mqtt_handler(const char* config_path);

#endif
