#ifndef __ROTARY_ENCODER_H__
#define __ROTARY_ENCODER_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define BUTTON_DEBOUNCE    1000
#define MENU_UPDATE        1000
#define MIN_ROTARY_COUNTER 0
#define MAX_ROTARY_COUNTER 2
#define SW_PIN  4
#define DT_PIN  3
#define CLK_PIN 5

typedef enum
{
    SWITCH_NOT_PRESSED = -1,
    SWITCH_PRESSED
}switch_state_t;

bool initialize_rotary_encoder();

uint8_t check_switch(void);
uint8_t get_position(void);
uint8_t check_encoder(void);

#endif
