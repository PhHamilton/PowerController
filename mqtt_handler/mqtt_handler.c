#include "mqtt_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "cJSON.h"

#define MAX_PARAMETER_SIZE 100

mqtt_handler_status_t cleanup_and_exit(char *data, cJSON *json, mqtt_handler_status_t status);
mqtt_handler_status_t initialize_mqtt(void);
mqtt_handler_status_t read_config(const char* config_path);

void on_connect(struct mosquitto *mosq, void *obj, int rc);
void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);

static mqtt_message_callback_t registered_callback = NULL;

typedef struct
{
    char **topics;
    uint8_t size;
}pub_sub_t;

typedef struct
{
    char client_id[MAX_PARAMETER_SIZE];
    char address[MAX_PARAMETER_SIZE];
    uint16_t port;
    pub_sub_t publish_topics;
    pub_sub_t subscription_topics;
    uint8_t QOS;
    uint16_t keep_alive;
    struct
    {
        char message_online[MAX_PARAMETER_SIZE];
        char message_offline[MAX_PARAMETER_SIZE];
        char topic[MAX_PARAMETER_SIZE];
        uint8_t QOS;
        bool retain;
    }last_will;
}mqtt_parameters_t;

static mqtt_parameters_t mqtt_parameters = {"\0"};
static struct mosquitto *mqtt_client = NULL;

mqtt_handler_status_t initialize_mqtt_handler(const char* config_path)
{
    if(read_config(config_path) != MQTT_HANDLER_OK)
    {
        return MQTT_HANDLER_ERROR;
    }

    return initialize_mqtt();
}

mqtt_handler_status_t read_config(const char* config_path)
{
    FILE *file = fopen(config_path, "rb");
    if(!file)
    {
        perror("Failed to open file mqtt config");
        return MQTT_HANDLER_ERROR;
    }

    fseek(file, 0, SEEK_END);
    uint32_t length = ftell(file);
    rewind(file);

    char *data = (char *)malloc(length + 1);
    fread(data, 1, length, file);
    data[length] = '\0';
    fclose(file);

    cJSON *json = cJSON_Parse(data);
    if(!json)
    {
        printf("Error parsing JSON %s\n", cJSON_GetErrorPtr());
        return cleanup_and_exit(data, json, MQTT_HANDLER_ERROR);
    }

    cJSON *mqtt = cJSON_GetObjectItem(json, "MQTT");
    if(!mqtt)
    {
        printf("Missing 'MQTT' object\n");
        return cleanup_and_exit(data, json, MQTT_HANDLER_ERROR);
    }

    cJSON *client_id_item = cJSON_GetObjectItem(mqtt, "client_id");
    if(!client_id_item || !cJSON_IsString(client_id_item))
    {
        printf("Missing 'client_id' item\n");
        return cleanup_and_exit(data, json, MQTT_HANDLER_ERROR);
    }
    strncpy(mqtt_parameters.client_id, client_id_item->valuestring, MAX_PARAMETER_SIZE);

    cJSON *address_item = cJSON_GetObjectItem(mqtt, "address");
    if(!address_item || !cJSON_IsString(address_item))
    {
        printf("Missing 'address' item\n");
        return cleanup_and_exit(data, json, MQTT_HANDLER_ERROR);
    }
    strncpy(mqtt_parameters.address, address_item->valuestring, MAX_PARAMETER_SIZE);

    cJSON *port_item = cJSON_GetObjectItem(mqtt, "port");
    if(!port_item || !cJSON_IsNumber(port_item))
    {
        printf("Missing 'port' item\n");
        return cleanup_and_exit(data, json, MQTT_HANDLER_ERROR);
    }
    mqtt_parameters.port = port_item->valueint;

    cJSON *publish_topics = cJSON_GetObjectItem(mqtt, "publish_topics");

    if(!publish_topics || !cJSON_IsArray(publish_topics))
    {
        printf("Missing 'publish_topics' object\n");
        cJSON_Delete(publish_topics);
        return cleanup_and_exit(data, mqtt, MQTT_HANDLER_ERROR);
    }
    uint8_t publish_size = cJSON_GetArraySize(publish_topics);
    mqtt_parameters.publish_topics.topics = malloc(sizeof(char*) * publish_size);
    mqtt_parameters.publish_topics.size = publish_size;

    for(uint8_t i = 0; i < publish_size; i++)
    {
        cJSON *item = cJSON_GetArrayItem(publish_topics, i);
        mqtt_parameters.publish_topics.topics[i] = strdup(item->valuestring);
    }

    cJSON *subscription_topics = cJSON_GetObjectItem(mqtt, "subscription_topics");

    if(!subscription_topics || !cJSON_IsArray(subscription_topics))
    {
        printf("Missing 'subscription_topics' object\n");
        cJSON_Delete(publish_topics);
        cJSON_Delete(subscription_topics);
        return cleanup_and_exit(data, mqtt, MQTT_HANDLER_ERROR);
    }
    uint8_t subscription_size = cJSON_GetArraySize(subscription_topics);
    mqtt_parameters.subscription_topics.topics = malloc(sizeof(char*) * subscription_size);
    mqtt_parameters.subscription_topics.size = subscription_size;

    for(uint8_t i = 0; i < subscription_size; i++)
    {
        cJSON *item = cJSON_GetArrayItem(subscription_topics, i);
        mqtt_parameters.subscription_topics.topics[i] = strdup(item->valuestring);
    }

    cJSON *QOS_item = cJSON_GetObjectItem(mqtt, "QOS");
    if(!QOS_item || !cJSON_IsNumber(QOS_item))
    {
        perror("Missing 'QOS' item\n");
        return cleanup_and_exit(data, json, MQTT_HANDLER_ERROR);
    }
    mqtt_parameters.QOS = QOS_item->valueint;

    cJSON *keep_alive_item = cJSON_GetObjectItem(mqtt, "keep_alive");
    if(!keep_alive_item || !cJSON_IsNumber(keep_alive_item))
    {
        perror("Missing 'keep_alive' item\n");
        return cleanup_and_exit(data, json, MQTT_HANDLER_ERROR);
    }
    mqtt_parameters.keep_alive = keep_alive_item->valueint;

    cJSON *last_will = cJSON_GetObjectItem(mqtt, "last_will");
    if(!last_will)
    {
        printf("Missing 'last_will' object\n");
        return cleanup_and_exit(data, json, MQTT_HANDLER_ERROR);
    }

    cJSON *message_online_item = cJSON_GetObjectItem(last_will, "message_online");
    if(!message_online_item || !cJSON_IsString(message_online_item))
    {
        printf("Missing 'last_will->message_online' item\n");
        cJSON_Delete(publish_topics);
        cJSON_Delete(subscription_topics);
        return cleanup_and_exit(data, json, MQTT_HANDLER_ERROR);
    }
    strncpy(mqtt_parameters.last_will.message_online, message_online_item->valuestring, MAX_PARAMETER_SIZE);

    cJSON *message_offline_item = cJSON_GetObjectItem(last_will, "message_offline");
    if(!message_offline_item || !cJSON_IsString(message_offline_item))
    {
        printf("Missing 'last_will->message_offline' item\n");
        cJSON_Delete(publish_topics);
        cJSON_Delete(subscription_topics);
        return cleanup_and_exit(data, json, MQTT_HANDLER_ERROR);
    }
    strncpy(mqtt_parameters.last_will.message_offline, message_offline_item->valuestring, MAX_PARAMETER_SIZE);

    cJSON *last_will_topic_item = cJSON_GetObjectItem(last_will, "topic");
    if(!last_will_topic_item || !cJSON_IsString(last_will_topic_item))
    {
        printf("Missing 'last_will->topic' item\n");
        cJSON_Delete(publish_topics);
        cJSON_Delete(subscription_topics);
        return cleanup_and_exit(data, json, MQTT_HANDLER_ERROR);
    }
    strncpy(mqtt_parameters.last_will.topic, last_will_topic_item->valuestring, MAX_PARAMETER_SIZE);

    QOS_item = cJSON_GetObjectItem(last_will, "QOS");
    if(!QOS_item || !cJSON_IsNumber(QOS_item))
    {
        printf("Missing 'last_will->QOS' item\n");
        cJSON_Delete(publish_topics);
        cJSON_Delete(subscription_topics);
        return cleanup_and_exit(data, json, MQTT_HANDLER_ERROR);
    }
    mqtt_parameters.last_will.QOS = QOS_item->valueint;

    cJSON *retain_item = cJSON_GetObjectItem(last_will, "retain");
    if(!retain_item || !cJSON_IsBool(retain_item))
    {
        printf("Missing 'last_will->retain' item\n");
        cJSON_Delete(publish_topics);
        cJSON_Delete(subscription_topics);
        return cleanup_and_exit(data, json, MQTT_HANDLER_ERROR);
    }
    mqtt_parameters.last_will.retain = retain_item->valueint;

    return cleanup_and_exit(data, json, MQTT_HANDLER_OK);
}
mqtt_handler_status_t cleanup_and_exit(char *data, cJSON *json, mqtt_handler_status_t status)
{
        if(data)
            free(data);

        if(json)
            cJSON_Delete(json);

        return status;
}

mqtt_handler_status_t initialize_mqtt(void)
{
    mosquitto_lib_init();

    mqtt_client = mosquitto_new(mqtt_parameters.client_id, true, NULL);

    if(!mqtt_client)
    {
        fprintf(stderr, "Failed to create mosquitto client\n");
        return MQTT_HANDLER_ERROR;
    }

    mosquitto_connect_callback_set(mqtt_client, on_connect);
    mosquitto_message_callback_set(mqtt_client, on_message);

    if(mosquitto_will_set(mqtt_client,
                          mqtt_parameters.last_will.topic,
                          strlen(mqtt_parameters.last_will.message_offline),
                          mqtt_parameters.last_will.message_offline,
                          mqtt_parameters.last_will.QOS,
                          mqtt_parameters.last_will.retain) != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "Failed to set last will\n");
        mosquitto_destroy(mqtt_client);
        return MQTT_HANDLER_ERROR;
    }


    int8_t ret = mosquitto_connect(
                                    mqtt_client,
                                    mqtt_parameters.address,
                                    mqtt_parameters.port,
                                    mqtt_parameters.keep_alive
                                  );
    if(ret != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "Failed to connect %s\n", mosquitto_strerror(ret));
        mosquitto_destroy(mqtt_client);
        mqtt_client = NULL;
        return MQTT_HANDLER_ERROR;
    }

    for(uint8_t i = 0; i < mqtt_parameters.subscription_topics.size; i++)
    {
        const char *topic = mqtt_parameters.subscription_topics.topics[i];
        int qos = mqtt_parameters.QOS;

        mosquitto_subscribe(mqtt_client, NULL, topic, qos) != MOSQ_ERR_SUCCESS
            ? fprintf(stderr, "Failed to subscribe to topic %s\n", topic)
            : printf("Subscribed to topic %s\n", topic);

    }

/*
    if(mosquitto_publish(mqtt_client,
                         NULL,
                         mqtt_parameters.last_will.topic,
                         strlen(mqtt_parameters.last_will.message_online),
                         mqtt_parameters.last_will.message_online,
                         mqtt_parameters.last_will.QOS,
                         mqtt_parameters.last_will.retain) != MOSQ_ERR_SUCCESS)
    {
        printf("Failed to publish initial message\n");
        mosquitto_destroy(mqtt_client);
        mqtt_client = NULL;
        return MQTT_HANDLER_ERROR;
    }
*/

    return MQTT_HANDLER_OK;
}

mqtt_handler_status_t start_mqtt_client(void)
{
    int8_t rc = mosquitto_loop_start(mqtt_client);
    if(rc != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "Failed to start mqtt thread: %s\n", mosquitto_strerror(rc));
        mosquitto_destroy(mqtt_client);
        return MQTT_HANDLER_ERROR;
    }

    return MQTT_HANDLER_OK;
}

mqtt_handler_status_t publish_message(const char* topic, const char* message)
{
    if(!mqtt_client || !topic || !message)
    {
        fprintf(stderr, "Invalid publish parameters\n");
        return MQTT_HANDLER_ERROR;
    }

    bool topic_exist = false;
    for(uint8_t i = 0; i < mqtt_parameters.publish_topics.size; i++)
    {
        if(strcmp(topic, mqtt_parameters.publish_topics.topics[i]) == 0)
        {
            topic_exist = true;
            break;
        }
    }

    if(!topic_exist)
    {
        printf("Error: Requested publish topic '%s', does not exist in config!\n", topic);
        return MQTT_HANDLER_ERROR;
    }

    int16_t rc = mosquitto_publish(
                                    mqtt_client,
                                    NULL,
                                    topic,
                                    (int)strlen(message),
                                    message,
                                    mqtt_parameters.QOS,
                                    true);

    if(rc != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "Publish failed %s\n", mosquitto_strerror(rc));
        return MQTT_HANDLER_ERROR;
    }

    return MQTT_HANDLER_OK;
}

void mqtt_register_callback(mqtt_message_callback_t callback)
{
    registered_callback = callback;
}

void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    if(registered_callback)
    {
        registered_callback(message->topic, message->payload);
    }
}

void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
    if(rc == MOSQ_ERR_SUCCESS)
    {
        publish_message(
                         mqtt_parameters.last_will.topic,
                         mqtt_parameters.last_will.message_online
                       );
    }
    else
    {
        fprintf(stderr, "Failed to connect to broker: %s\n", mosquitto_strerror(rc));
    }
}

void deconstruct_mqtt(void)
{
    if(mqtt_client)
    {
        if(publish_message(mqtt_parameters.last_will.topic, mqtt_parameters.last_will.message_offline) != MQTT_HANDLER_OK)
        {
            printf("Error: Failed to publish last will message on shutdown!");
        }

        mosquitto_disconnect(mqtt_client);
        mosquitto_loop_stop(mqtt_client, true); // true = vänta tills tråden avslutas
        mosquitto_destroy(mqtt_client);
        mosquitto_lib_cleanup();
    }
}
