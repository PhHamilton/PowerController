#ifndef __MENU_HANDLER_H__
#define __MENU_HANDLER_H__

#include <stdint.h>
#include <stdbool.h>
#include "gui_handler.h"

#define MIN_ROTARY_COUNTER 0
#define MAX_ROTARY_COUNTER 2
#define BUTTON_DEBOUNCE    1000

void rotary_isr(void);
void button_isr(void);

bool initialize_menu_handler(void);
void update_menu(gui_parameters_t *param);

#endif
