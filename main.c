#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include <string.h>
#include <wiringPi.h>
#include <unistd.h>

#include <time.h>
#include <errno.h>

#define DISPLAY_REFRESH_RATE 10
#define ROTARY_REFRESH_RATE  50

#include "menu_handler.h"

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

    initialize_menu_handler();

    /*
    initialize_rotary_encoder();
    initialize_gui();
    */

    while(1)
    {
/*
        uint8_t position = get_position();
        param.cursor_position = position;
        if(check_switch())
        {
            param.measurements[position].output_state = param.measurements[position].output_state == OUTPUT_ACTIVE ? OUTPUT_INACTIVE : OUTPUT_ACTIVE;
        }
        update_gui(&param);
*/
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
