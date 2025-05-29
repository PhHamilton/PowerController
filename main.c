#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include <string.h>
#include <wiringPi.h>
#include <unistd.h>
#include "mqtt_handler.h"
#include "mqtt_message_handler.h"

#include <time.h>
#include <errno.h>

#define DISPLAY_REFRESH_RATE 10
#define ROTARY_REFRESH_RATE  50

#include "gui_handler.h"
#include "rotary_encoder_handler.h"
#include "ina219.h"
#include "output_handler.h"

gui_parameters_t gui_parameters = {0};
float default_voltages[NUMBER_OF_CHANNELS] = {3.3, 5, 12, -12};

float rand_vec[3] = {2.54, 2.34, 2.55};
const uint8_t output_pins[NUMBER_OF_CHANNELS] = {24, 25, 27, 28};


INA219_t ina_data = {0};

void sleep_ms(uint32_t m_sec);

void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:exit\r\n");
    DEV_ModuleExit();

    deconstruct_mqtt();
    exit(0);
}



int main(int argc, char *argv[])
{
    // Exception handling:ctrl + c
    signal(SIGINT, Handler);

    const char* config_path = "./mqtt_handler/config.json";
    printf("Initializing mqtt handler..\n");
    if(initialize_mqtt_handler(config_path) != MQTT_HANDLER_OK)
    {
        printf("Failed to initialize mqtt handler");
        return false;
    }

    mqtt_register_callback("power_controller/status_update", status_update_handler);
    mqtt_register_callback("power_controller/output_update", output_update_handler);

    printf("Starting mqtt handler\n");
    if(start_mqtt_client() != MQTT_HANDLER_OK)
    {
        printf("Failed to start mqtt handler");
        return false;
    }

/*
    while(1);
    return false;
*/

    if(wiringPiSetup() == -1)
    {
        printf("Failed to initialize wiringPi\n");
        return false;
    }

    /*
    printf("Initializing output handler..\n");
    if(!initialize_output_handler(output_pins))
    {
        printf("Failed to output handler\r\n");
        return 0;
    }
    */

    gui_parameters.cursor_position = 0;
    gui_parameters.measurements[0].address = 0x40;
    gui_parameters.measurements[0].voltage = 3.3f;
    gui_parameters.measurements[0].current = 2;
    gui_parameters.measurements[0].output_state = OUTPUT_INACTIVE;

    gui_parameters.measurements[1].address = 0x41;
    gui_parameters.measurements[1].voltage = 5;
    gui_parameters.measurements[1].current = 0.23;
    gui_parameters.measurements[1].output_state = OUTPUT_INACTIVE;

    gui_parameters.measurements[2].address = 0x44;
    gui_parameters.measurements[2].voltage = 12;
    gui_parameters.measurements[2].current = 0.02;
    gui_parameters.measurements[2].output_state = OUTPUT_INACTIVE;

    gui_parameters.measurements[3].address = 0x45;
    gui_parameters.measurements[3].voltage = -12;
    gui_parameters.measurements[3].current = 0.02;
    gui_parameters.measurements[3].output_state = OUTPUT_INACTIVE;


    if(initialize_gui() != GUI_OK)
    {
        printf("Failed to initialize GUI\r\n");
        return 0;
    }

    if(!initialize_rotary_encoder())
    {
        printf("Failed to initialize rotary encoder\r\n");
        return 0;
    }

    ina_data.channel_config[0].address = 0x40;
    ina_data.channel_config[0].max_current = 1.0f;


    if(ina219_calibrate(&ina_data, 0) != INA219_OK)
    {
        printf("Failed to initialize ina219\r\n");
        return 0;
    }

    uint8_t i = 0;

    while(1)
    {
        gui_parameters.cursor_position = get_position();
        int8_t switch_pressed = check_switch();

        if(switch_pressed != SWITCH_NOT_PRESSED)
        {
            gui_parameters.measurements[switch_pressed].output_state = gui_parameters.measurements[switch_pressed].output_state == OUTPUT_ACTIVE ? OUTPUT_INACTIVE : OUTPUT_ACTIVE;
            ina219_measure(&ina_data, 0);
            printf("Voltage: %f, Current: %f, Power: %f\n", ina_data.channel_data[0].voltage, ina_data.channel_data[0].current, ina_data.channel_data[0].power);
        }

        // Read measurements
        for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++)
        {
            if(gui_parameters.measurements[i].output_state == OUTPUT_INACTIVE)
            {
                continue;
            }

            if(get_output_state(i) == OUTPUT_ENABLED &&
                  gui_parameters.measurements[i].output_state == OUTPUT_INACTIVE)
               {
                    if(!change_output_state(i, OUTPUT_DISABLED))
                    {
                        printf("Failed to change output state");
                    }
               }
               else if(get_output_state(i) == OUTPUT_DISABLED &&
                  gui_parameters.measurements[i].output_state == OUTPUT_ACTIVE)
               {
                    if(!change_output_state(i, OUTPUT_ENABLED))
                    {
                        printf("Failed to change output state");
                    }
               }
               else
               {
                    continue;
               }
        }

        gui_parameters.measurements[0].current = rand_vec[i];
        update_gui(&gui_parameters);
        i++;

        if(i > 2)
         i = 0;

        sleep_ms(100);
    }

    return 0;
}

void sleep_ms(uint32_t m_sec)
{
    struct timespec ts;
    int8_t res;

    if(m_sec < 0)
    {
        errno = EINVAL;
        return;
    }

    ts.tv_sec = m_sec/1000;
    ts.tv_nsec = (m_sec % 1000) * 1000000;

    do
    {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);
}
