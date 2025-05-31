#include "mqtt_message_handler.h"
#include "cJSON.h"
#include "mqtt_handler.h"
#include "main.h"
#include <string.h>
#include <stdlib.h>

cJSON *create_data_item(uint8_t id, uint8_t status, float voltage, float current);
cJSON *create_status_item(uint8_t id, uint8_t status);

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

    status_request_type_t request = (status_request_type_t)request_item->valueint;

    cJSON *root = cJSON_CreateObject();
    cJSON *data_array = cJSON_CreateArray();

    cJSON_AddNumberToObject(root, "Request", request);

    uint8_t start = 0, end = 0;

    switch(request)
    {
        case ALL:
        {
            start = VOLTAGE_3V3;
            end = VOLTAGE_NEG12V;
        }
        break;
        case VOLTAGE_3V3:
        case VOLTAGE_5V:
        case VOLTAGE_12V:
        case VOLTAGE_NEG12V:
        {
            start = end = request;
            break;
        }
        default:
        {
            fprintf(stderr, "Unknown request %d\n", request);
            cJSON_Delete(json);
            cJSON_Delete(root);
            return;
        }
    }

    for(uint8_t i = start; i <= end; i++)
    {
        if(i < NUMBER_OF_CHANNELS)
        {
            cJSON_AddItemToArray(
                                 data_array,
                                 create_data_item(
                                                  i,
                                                  gui_parameters.measurements[i].output_state,
                                                  gui_parameters.measurements[i].voltage,
                                                  gui_parameters.measurements[i].current
                                                 )
                                );
        }
    }

    cJSON_AddItemToObject(root, "Data", data_array);
    char *json_string = cJSON_PrintUnformatted(root);

    if(json_string)
    {
        publish_message("power_controller/response", cJSON_PrintUnformatted(root));
        free(json_string);
    }

    cJSON_Delete(json);
    cJSON_Delete(root);
}


void output_update_handler(const char* topic, const char* message)
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

    cJSON *status_item = cJSON_GetObjectItem(json, "Status");
    if(!status_item)
    {
        fprintf(stderr, "Missing 'Status' item\n");
        return;
    }

    if(!cJSON_IsNumber(status_item))
    {
        fprintf(stderr, "'status' item does not contain a number\n");
        return;
    }

    status_request_type_t request = (status_request_type_t)request_item->valueint;
    uint8_t status = status_item->valueint;

    cJSON *root = cJSON_CreateObject();
    cJSON *data_array = cJSON_CreateArray();

    cJSON_AddNumberToObject(root, "Request", request);

    uint8_t start = 0, end = 0;

    switch(request)
    {
        case ALL:
        {
            start = VOLTAGE_3V3;
            end = VOLTAGE_NEG12V;
        }
        break;
        case VOLTAGE_3V3:
        case VOLTAGE_5V:
        case VOLTAGE_12V:
        case VOLTAGE_NEG12V:
        {
            start = end = request;
            break;
        }
        default:
        {
            fprintf(stderr, "Unknown request %d\n", request);
            cJSON_Delete(json);
            cJSON_Delete(root);
            return;
        }
    }

    for(uint8_t i = start; i <= end; i++)
    {
        if(i < NUMBER_OF_CHANNELS)
        {

            if(status != gui_parameters.measurements[i].output_state)
            {
                gui_parameters.measurements[i].output_state = status;
            }


            cJSON_AddItemToArray(
                                 data_array,
                                 create_status_item(
                                                    i,
                                                    gui_parameters.measurements[i].output_state
                                                   )
                                );
        }
    }

    cJSON_AddItemToObject(root, "Data", data_array);
    char *json_string = cJSON_PrintUnformatted(root);

    if(json_string)
    {
        publish_message("power_controller/response", cJSON_PrintUnformatted(root));
        free(json_string);
    }


    free(json);
    free(root);
}

cJSON *create_data_item(uint8_t id, uint8_t status, float voltage, float current)
{
    cJSON *data_item = cJSON_CreateObject();
    cJSON_AddNumberToObject(data_item, "ID", id);
    cJSON_AddNumberToObject(data_item, "Status", status);
    cJSON_AddNumberToObject(data_item, "Voltage", voltage);
    cJSON_AddNumberToObject(data_item, "Current", current);

    return data_item;
}

cJSON *create_status_item(uint8_t id, uint8_t status)
{
    cJSON *data_item = cJSON_CreateObject();
    cJSON_AddNumberToObject(data_item, "ID", id);
    cJSON_AddNumberToObject(data_item, "Status", status);

    return data_item;
}
