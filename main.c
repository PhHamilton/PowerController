#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include <string.h>


#include "gui_handler.h"

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

    initialize_gui();

    gui_parameters_t param;
    param.cursor_position = 0;
    param.measurements[0].voltage = 3.3f;
    param.measurements[0].current = 2;
    param.measurements[0].output_state = OUTPUT_INACTIVE;

    param.measurements[1].voltage = 5;
    param.measurements[1].current = 0.23;
    param.measurements[1].output_state = OUTPUT_ACTIVE;

    param.measurements[2].voltage = 12;
    param.measurements[2].current = 0.02;
    param.measurements[2].output_state = OUTPUT_ACTIVE;

    update_gui(&param);

    return 0;
}
