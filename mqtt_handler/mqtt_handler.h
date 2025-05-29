#ifndef __MQTT_HANDLER_H__
#define __MQTT_HANDLER_H__

#include "mosquitto.h"

#define MAX_SUBSCRIPTION_TOPICS 10
typedef void (*mqtt_message_callback_t)(const char* topic, const char* message);

typedef enum
{
    MQTT_HANDLER_OK,
    MQTT_HANDLER_ERROR
}mqtt_handler_status_t;

typedef struct
{
    const char *topic;
    mqtt_message_callback_t callback;
}mqtt_topic_handler_t;

void mqtt_register_callback(const char* topic, mqtt_message_callback_t callback);

mqtt_handler_status_t initialize_mqtt_handler(const char* config_path);
mqtt_handler_status_t start_mqtt_client(void);
mqtt_handler_status_t publish_message(const char* topic, const char* message);
void deconstruct_mqtt(void);

#endif
