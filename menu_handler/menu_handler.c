#include "menu_handler.h"
#include "rotary_encoder_handler.h"
#include <wiringPi.h>
#include <time.h>

static volatile int16_t counter = 0;
static gui_parameters_t param = {0};

static uint32_t last_button_press = 0;
uint8_t constrain_interval(uint8_t counter);

bool initialize_menu_handler(void)
{
    if(!initialize_rotary_encoder())
        return false;

    initialize_gui();

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

    return true;
}

void update_menu(gui_parameters_t *param)
{
//    param->cursor_position = get_position();
    update_gui(param);
}

void button_isr(void)
{

    uint32_t current_time = millis();

    if((current_time - last_button_press) > BUTTON_DEBOUNCE)
    {
        int8_t position = get_position();
        param.measurements[counter].output_state = param.measurements[counter].output_state == OUTPUT_ACTIVE ? OUTPUT_INACTIVE : OUTPUT_ACTIVE;
        update_gui(&param);
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
    counter++;              // Increase counter
    //counter = constrain_interval(counter);
    param.cursor_position = counter;
    update_menu(&param);
    printf("%i\n", counter);
    encval = 0;
  }
  else if( encval < -3 ) // Four steps backwards
  {
    counter--;               // Decrease counter
    //counter = constrain_interval(counter);
    param.cursor_position = counter;
    update_menu(&param);
    printf("%i\n", counter);
    encval = 0;
  }

}

uint8_t constrain_interval(uint8_t counter)
{
    if(counter > MAX_ROTARY_COUNTER)
    {
       return MAX_ROTARY_COUNTER;
    }

    if(counter < MIN_ROTARY_COUNTER)
    {
        return MIN_ROTARY_COUNTER;
    }

    return counter;
}
