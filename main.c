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

gui_parameters_t param = {0};

float rand_vec[3] = {2.54, 2.34, 2.55};

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

    param.cursor_position = 0;
    param.measurements[0].voltage = 3.3f;
    param.measurements[0].current = 2;
    param.measurements[0].output_state = OUTPUT_INACTIVE;

    param.measurements[1].voltage = 5;
    param.measurements[1].current = 0.23;
    param.measurements[1].output_state = OUTPUT_INACTIVE;

    param.measurements[2].voltage = 12;
    param.measurements[2].current = 0.02;
    param.measurements[2].output_state = OUTPUT_INACTIVE;

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
