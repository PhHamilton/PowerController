#include "mqtt_message_handler.h"
#include "cJSON.h"
#include "mqtt_handler.h"
#include <string.h>


void status_update_handler(const char* topic, const char* message)
{
    cJSON *json = cJSON_Parse(message);

    if(!json)
    {
        fprintf(stderr, "Error parsing JSON %s\n", cJSON_GetErrorPtr());
        return;
    }

    cJSON *request_item = cJSON_GetObjectItem(json, "Request");
    if(!request_item)
    {
        fprintf(stderr, "Missing 'Request' item\n");
        return;
    }

    if(!cJSON_IsNumber(request_item))
    {
        fprintf(stderr, "'Request' item does not contain a number\n");
        return;
    }

    char msg_buffer[10] = {'\0'};
    switch((status_request_type_t)request_item)
    {
        case ALL:
        {
        }
        break;
        case VOLTAGE_3V3:
        {
    sprintf(msg_buffer,"%d",  gui_parameters.measurements[0].output_state);

        }
        break;
        case VOLTAGE_3V3:
        {

        }
        break;
        case VOLTAGE_3V3:
        {

        }
        break;
        case VOLTAGE_3V3:
        {

        }
        break;
        default:
        {

        }
        break;
    }
    publish_message("power_controller/response", msg_buffer);

}


void output_update_handler(const char* topic, const char* message)
{
    printf("Topic: %s, Message: %s\n", topic, message);
}
