#include "rotary_encoder_handler.h"
#include <wiringPi.h>

static uint8_t counter = 0;
uint8_t clkLastState = 0;

void initialize_rotary_encoder()
{
    wiringPiSetup();
    pinMode(CLK_PIN, INPUT);
    pinMode(DT_PIN, INPUT);
    pinMode(SW_PIN, INPUT);

    pullUpDnControl(SW_PIN, PUD_UP);
}
uint8_t check_switch(void)
{
    
}

uint8_t get_position(void)
{
    return counter;
}

uint8_t check_encoder(void)
{
    uint8_t clkState = digitalRead(CLK_PIN);
    uint8_t dtState =  digitalRead(DT_PIN);

    if(clkLastState != clkState)
    {
        if(dtState != clkState)
        {
            counter++;
        }
        else
        {
            counter--;
        }

        if(counter > 2)
        {
            counter = 2;
        }
        if(counter < 0)
        {
            counter = 0;
        }

    }

    clkLastState = clkState;
    return 0;
}
