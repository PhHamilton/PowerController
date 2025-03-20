#include "rotary_encoder_handler.h"
#include <wiringPi.h>
#include <time.h>

volatile int counter = 0;
uint8_t last_clk_state = 0;
uint8_t switch_pressed = 0;
uint32_t last_rotary_time = 0;

extern void rotary_isr(void);
extern void button_isr(void);

bool initialize_rotary_encoder()
{
    if(wiringPiSetup() == -1)
    {
        printf("Failed to initialize wiringPi\n");
        return false;
    }
    pinMode(CLK_PIN, INPUT);
    pinMode(DT_PIN, INPUT);
    pinMode(SW_PIN, INPUT);

    pullUpDnControl(SW_PIN, PUD_UP);

    if(wiringPiISR(CLK_PIN, INT_EDGE_BOTH, &rotary_isr) < 0)
    {
        printf("Failed to setup ISR for CLK_PIN\n");
        return false;
    }
    if(wiringPiISR(DT_PIN, INT_EDGE_BOTH,  &rotary_isr) < 0)
    {
        printf("Failed to setup ISR for DT_PIN\n");
        return false;
    }
    if(wiringPiISR(SW_PIN, INT_EDGE_FALLING,  &button_isr) < 0)
    {
        printf("Failed to setup ISR for DT_PIN\n");
        return false;
    }

    return true;
}
uint8_t check_switch(void)
{
    uint8_t tmp = switch_pressed;
    switch_pressed = 0;
    return tmp;
}

uint8_t get_position(void)
{
    return counter;
}

