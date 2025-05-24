#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include <string.h>
#include <wiringPi.h>
#include <unistd.h>

#include <time.h>
#include <errno.h>

#define DISPLAY_REFRESH_RATE 10
#define ROTARY_REFRESH_RATE  50

#include "gui_handler.h"
#include "rotary_encoder_handler.h"
#include "ina219.h"
#include "output_handler.h"

gui_parameters_t param = {0};


float rand_vec[3] = {2.54, 2.34, 2.55};
const uint8_t output_pins[NUMBER_OF_CHANNELS] = {25, 24, 27, 28};

void sleep_ms(uint32_t m_sec);

void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:exit\r\n");
    DEV_ModuleExit();

    exit(0);
}

int main(int argc, char *argv[])
{
    // Exception handling:ctrl + c
    signal(SIGINT, Handler);

    INA219_t ina_data = {0};


    if(wiringPiSetup() == -1)
    {
        printf("Failed to initialize wiringPi\n");
        return false;
    }

    initialize_output_handler(output_pins);

    param.cursor_position = 0;
    param.measurements[0].address = 0x40;
    param.measurements[0].voltage = 3.3f;
    param.measurements[0].current = 2;
    param.measurements[0].output_state = OUTPUT_INACTIVE;

    param.measurements[1].address = 0x41;
    param.measurements[1].voltage = 5;
    param.measurements[1].current = 0.23;
    param.measurements[1].output_state = OUTPUT_INACTIVE;

    param.measurements[2].address = 0x44;
    param.measurements[2].voltage = 12;
    param.measurements[2].current = 0.02;
    param.measurements[2].output_state = OUTPUT_INACTIVE;

    param.measurements[3].address = 0x45;
    param.measurements[3].voltage = -12;
    param.measurements[3].current = 0.02;
    param.measurements[3].output_state = OUTPUT_INACTIVE;

    if(!initialize_rotary_encoder())
    {
        printf("Failed to initialize rotary encoder\r\n");
        return 0;
    }

    initialize_gui();

    uint8_t i = 0;

    while(1)
    {
        param.cursor_position = get_position();
        int8_t switch_pressed = check_switch();

        if(switch_pressed != SWITCH_NOT_PRESSED)
        {
            param.measurements[switch_pressed].output_state = param.measurements[switch_pressed].output_state == OUTPUT_ACTIVE ? OUTPUT_INACTIVE : OUTPUT_ACTIVE;
            ina_data.address = 0x40;
            ina219_measure(&ina_data);
            printf("Voltage: %f, Current: %f\n", ina_data.voltage, ina_data.current);
        }

        // Read measurements
        for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++)
        {
            if(param.measurements[i].output_state == OUTPUT_INACTIVE) continue;

            if(get_output_state(i) == OUTPUT_ENABLED) continue;

            printf("Changing output State");

            if(!change_output_state(i, OUTPUT_ENABLED))
            {
                printf("Failed to change output state");
            }

        }
        param.measurements[0].current = rand_vec[i];
        update_gui(&param);
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
