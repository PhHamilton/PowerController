#include "rotary_encoder_handler.h"
#include <wiringPi.h>
#include <time.h>

volatile int counter = 0;
uint8_t last_clk_state = 0;
uint8_t switch_pressed = 0;
uint32_t last_rotary_time = 0;

void rotary_isr(void);
void button_isr(void);

uint8_t constrain_interval(int8_t counter);
static uint32_t last_button_press = 0;
static uint32_t last_update_menu  = 0;

volatile int button_pressed = -1;

bool initialize_rotary_encoder()
{
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
    uint8_t tmp = button_pressed;
    button_pressed = SWITCH_NOT_PRESSED;
    return tmp;
}

uint8_t get_position(void)
{
    return counter;
}

void button_isr(void)
{
    uint32_t current_time = millis();

    if((current_time - last_button_press) > BUTTON_DEBOUNCE)
    {
        button_pressed = counter;
        printf("Button pressed, %i,  %i %i %i\n", counter, current_time, last_button_press, current_time - last_button_press);
    }
    last_button_press = current_time;

    fflush(stdout);
}
void rotary_isr()
{
  // Encoder interrupt routine for both pins. Updates counter
  // if they are valid and have rotated a full indent

  static uint8_t old_AB = 3;  // Lookup table index
  static int8_t encval = 0;   // Encoder value
  static const int8_t enc_states[]  = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0}; // Lookup table

  old_AB <<= 2;  // Remember previous state

  if (digitalRead(CLK_PIN)) old_AB |= 0x02; // Add current state of pin A
  if (digitalRead(DT_PIN))  old_AB |= 0x01; // Add current state of pin B

  encval += enc_states[( old_AB & 0x0f )];

  // Update counter if encoder has rotated a full indent, that is at least 4 steps
  if( encval > 3 ) // Four steps forward
  {
    counter--;               // Decrease counter
    counter = constrain_interval(counter);
    printf("%i\n", counter);
    encval = 0;
  }
  else if( encval < -3 ) // Four steps backwards
  {
    counter++;              // Increase counter
    counter = constrain_interval(counter);
    printf("%i\n", counter);
    encval = 0;
  }

}

uint8_t constrain_interval(int8_t counter)
{
    if(counter > MAX_ROTARY_COUNTER)
    {
       return MAX_ROTARY_COUNTER;
    }

    if(counter <= MIN_ROTARY_COUNTER)
    {
        return MIN_ROTARY_COUNTER;
    }

    return counter;
}
