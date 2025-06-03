#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <wiringPi.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>

#include "mqtt_handler.h"
#include "mqtt_message_handler.h"
#include "gui_handler.h"
#include "rotary_encoder_handler.h"
#include "ina219.h"
#include "output_handler.h"

#define DISPLAY_REFRESH_RATE 10
#define ROTARY_REFRESH_RATE  50
#define NUMBER_OF_CURRENT_MEASUREMENT_CHANNELS 3

gui_parameters_t gui_parameters = {0};
float default_voltages[NUMBER_OF_CHANNELS] = {3.3, 5, 12, -12};
const uint8_t output_pins[NUMBER_OF_CHANNELS] = {19, 26, 16, 20};

void sleep_ms(uint32_t m_sec);

void Handler(int signo)
{
    printf("\r\nHandler:exit\r\n");
    DEV_ModuleExit();

    deconstruct_mqtt();
    cleanup_outputs();
    exit(0);
}

int main(int argc, char *argv[])
{
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

    printf("Initialixing wiringPi\n");
    if(wiringPiSetup() == -1)
    {
        printf("Failed to initialize wiringPi\n");
        return false;
    }

    printf("Initializing output handler..\n");
    if(!initialize_output_handler(output_pins))
    {
        printf("Failed to output handler\r\n");
        return false;
    }

    gui_parameters.cursor_position = 0;

    // Init measurements
    for (int i = 0; i < NUMBER_OF_CHANNELS; i++) {
        gui_parameters.measurements[i].address = 0x40 + i;
        gui_parameters.measurements[i].voltage = default_voltages[i];
        gui_parameters.measurements[i].current = 0;
        gui_parameters.measurements[i].output_state = OUTPUT_INACTIVE;
    }

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

    /*
    INA219_channel_handler_t ina = {0};

    for (uint8_t i = 0; i < 1; i++) {
        ina.channel[i].config.address = 0x40 + i;
        ina.channel[i].config.shunt_resistance = 0.1f;

        if (ina219_initialize(&ina.channel[i].config) < 0) {
            printf("Failed to initialize INA219 on channel %d\n", i);
            return 0;
        }
    }
    */

    INA219_channel_handler_t ina219[3];
    ina219[0].config.address = INA219_ADDRESS_1;
    ina219[0].config.shunt_resistance = 0.1f;

    ina219[1].config.address = INA219_ADDRESS_2;
    ina219[1].config.shunt_resistance = 0.1f;

    ina219[2].config.address = INA219_ADDRESS_3;
    ina219[2].config.shunt_resistance = 0.1f;

    for(uint8_t i = 0; i < NUMBER_OF_CURRENT_MEASUREMENT_CHANNELS; i++)
    {
        if(ina219_init(&ina219[i].config) != INA219_OK)
        {
            printf("INA219 init failed on channel %i\n", i);
            return 1;
        }
    }

    while (1)
    {
        gui_parameters.cursor_position = get_position();
        int8_t switch_pressed = check_switch();

        if(switch_pressed != SWITCH_NOT_PRESSED)
        {
            gui_parameters.measurements[switch_pressed].output_state ^= 1;
        }

        for (uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++)
        {
            if (gui_parameters.measurements[i].output_state == OUTPUT_INACTIVE &&
                get_output_state(i) == OUTPUT_DISABLED)
                continue;

            if (get_output_state(i) == OUTPUT_ENABLED &&
                gui_parameters.measurements[i].output_state == OUTPUT_INACTIVE)
            {
                printf("Turning off output\n");
                if (!change_output_state(i, OUTPUT_DISABLED))
                    printf("Failed to change output state\n");
            }
            else if (get_output_state(i) == OUTPUT_DISABLED &&
                     gui_parameters.measurements[i].output_state == OUTPUT_ACTIVE)
            {
                printf("Turning on output\n");
                if (!change_output_state(i, OUTPUT_ENABLED))
                    printf("Failed to change output state\n");
            }
            else
            {

            }

            if (gui_parameters.measurements[i].output_state == OUTPUT_ACTIVE)
            {
                if(i < NUMBER_OF_CURRENT_MEASUREMENT_CHANNELS)
                {
                    if(ina219_read(&ina219[i].config, &ina219[i].data) == INA219_OK)
                    {
                        gui_parameters.measurements[i].voltage = ina219[i].data.voltage;
                        gui_parameters.measurements[i].current = ina219[i].data.current;

                        printf("Channel: %i, Voltage: %.3f V, Current: %.3f A, Power: %.3f W\n",
                               i, ina219[i].data.voltage, ina219[i].data.current, ina219[i].data.power);
                    }
                }
            }
            else
            {
                gui_parameters.measurements[i].voltage = default_voltages[i];
                gui_parameters.measurements[i].current = 0.0f;
            }
        }
        update_gui(&gui_parameters);
        sleep_ms(100);
    }

    return 0;
}

void sleep_ms(uint32_t m_sec)
{
    struct timespec ts;
    if(m_sec < 0)
    {
        errno = EINVAL;
        return;
    }
    ts.tv_sec = m_sec / 1000;
    ts.tv_nsec = (m_sec % 1000) * 1000000;
    nanosleep(&ts, NULL);
}
