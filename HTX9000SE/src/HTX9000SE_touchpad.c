/**************************************************************************
*                                                                         *
* Touchpad                                                                *
*                                                                         *
***************************************************************************/

#include "HTX9000SE_touchpad.h"
/****************************************************************************
*                        Touch Pad Variables                                *
*****************************************************************************/
touchpad_key_t BACKSPACE_key = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t NINE_key      = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t EIGHT_key     = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t SEVEN_key     = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t mA_key        = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t HI_LO_key     = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t SIX_key       = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t FIVE_key      = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t FOUR_key      = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t THREE_key     = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t TWO_key       = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t ONE_key       = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t AMPS_key      = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t DECIMAL_key   = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t ZERO_key      = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t uA_key        = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};

uint16_t    debounce_time           = 0;
uint16_t    keypad_timeout          = 0;
uint8_t     key_pending_counter     = 0;
uint8_t     hot_key                 = BACKSPACE;
uint8_t     button                  = BACKSPACE;
uint8_t     last_button             = uA;
bool        key_pending             = false;
bool        key_encountered         = false;
bool        key_acknowledged        = true;
extern      uint16_t keypad_timer;
/****************************************************************************
*    Kickoff the keypad key for timing measurement                          *
*    Each button pin is high coming in here. First it is changed            *
*    from low-Z hi to pull-up resistor mode with the DDR bit. Then with the *
*    "PORTx" bit it's set to pure hi Z.                                     *
*****************************************************************************/
void kickoff_pin(uint8_t button)
{
    KEYPAD_ADDR_PORT    &= 0x0F;                            // Switch the key address selection to 'this' key.
    KEYPAD_ADDR_PORT    |= button;                          // The wiring is such that the bits line up with PF7,PF6,PF5,PF4 and the button enums are pre-shifted to line up.
    KEYPAD_SENSOR_PORT  |= KEYPAD_SENSOR_PIN;               // Set the pin to high (resistive pull up only after this line).
    KEYPAD_SENSOR_DDR   |= KEYPAD_SENSOR_PIN;               // Set the direction to output (low impedance pull up).
    _delay_us(100);
    KEYPAD_SENSOR_DDR   &= ~KEYPAD_SENSOR_PIN;              // Set the DDR to input. (Resistive pull up only after this line about 35kOhm, should drop about 35mV from 5V).
    TIFR1               |= _BV(ICF1);                       // Clear the input capture flag by writing to "1" (which makes it zero presumably).
    cli();                                                  // Critical timing, no interrupts allowed here.
    TCCR1B               = 0;                               // Halt the timer before clearing.
    TCNT1                = 0;                               // Clear out the counter.
    TCCR1B               = _BV(CS10);                       // Enable Timer 1 prescaling /8 1us per tick. See Table 16-5 in the ATMEGA32u4 datasheet. 8MHz, 1us * 65,536 = 65.536ms overflow.
    KEYPAD_SENSOR_PORT  &= ~KEYPAD_SENSOR_PIN;              // Set the value to 0, pin is now completely open with only the external 10M on the board to pull down.
    sei();                                                  // OK to allow interrupts after here, timer started and voltage on its way down to 0V.
    key_pending          = true;                            // Let the system know there is a measurement pending.
}
/****************************************************************************
*            Routine for measuring the touch buttons                        *
*    Each button pin is high coming in here. First it is changed            *
*    from low-Z hi to pull-up resistor mode with the DDR bit. Then with the *
*    "PORTx" bit it's set to pure hi Z.                                     *
*****************************************************************************/
bool process_key (touchpad_key_t *the_key, IO_pointers_t IO)
{   
    uint16_t current_falltime;                                      //
    TCCR1B = 0;                                                     // Found the fall time (capacitance), halt the timer.
    current_falltime = ICR1;                                        // Pick up the value that is presumably ready
    // if (( current_falltime < the_key->fastest_falltime ) &&      // Check if this falltime is the fastest on record and
        // ( the_key->fastest_falltime > FALLTIME_DECREMENT ))      // for underflow if we plan to decrment.
        // the_key->fastest_falltime -= FALLTIME_DECREMENT;         // Push the key's fastest_falltime toward min ever seen in steps of FALLTIME_DECREMENT
    if ( current_falltime < the_key->fastest_falltime )             // Set the fastest fall time to the fastest ever seen.
        the_key->fastest_falltime = current_falltime;               // Set the fastest fall time to the fastest ever seen.
                                                                    //
    if ( the_key->leaktimer > LEAKAGE_TIME )                        // Now check to see of the pin's leaktimer exceeds LEAKAGE_TIME.
        { the_key->fastest_falltime++;                              // Increment the fastest_falltime once every LEAKAGE_TIME expiration.
          the_key->leaktimer = 0;                                   // Clear the leakage timer for the next leaktimer cycle.
        }                                                           //
                                                                    //
    else                                                            // 
        the_key->leaktimer++;                                       // If the timer didn't trip on this cycle increment the slow leakage timer
    key_pending = false;                                            // Let the system know the key is done.
    
    return (current_falltime >  the_key->fastest_falltime * 1.15);   // 20% longer than fastest on record.
    
    // return (current_falltime >  (the_key->fastest_falltime +        // Return True if current_falltime > 1.0625 times or
                                // (the_key->fastest_falltime >> 3) +  // +1/8  again or 25% longer than fastest on record.
                                // (the_key->fastest_falltime >> 4))); // +1/16 again or an additional 12.5% longer than fastest on record.
}
/****************************************************************************
*   Determine if the timer value has been recorded due to an input capture  *
*****************************************************************************/
void process_keypad(IO_pointers_t IO)
{
    if (!key_pending)           // if there's no key currently pending kick one off.
    {
        if      (last_button == BACKSPACE) {button = NINE       ;kickoff_pin(NINE     );}
        else if (last_button == NINE     ) {button = EIGHT      ;kickoff_pin(EIGHT    );}
        else if (last_button == EIGHT    ) {button = SEVEN      ;kickoff_pin(SEVEN    );}
        else if (last_button == SEVEN    ) {button = mA         ;kickoff_pin(mA       );}
        else if (last_button == mA       ) {button = HI_LO      ;kickoff_pin(HI_LO    );}
        else if (last_button == HI_LO    ) {button = SIX        ;kickoff_pin(SIX      );}
        else if (last_button == SIX      ) {button = FIVE       ;kickoff_pin(FIVE     );}
        else if (last_button == FIVE     ) {button = FOUR       ;kickoff_pin(FOUR     );}
        else if (last_button == FOUR     ) {button = THREE      ;kickoff_pin(THREE    );}
        else if (last_button == THREE    ) {button = TWO        ;kickoff_pin(TWO      );}
        else if (last_button == TWO      ) {button = ONE        ;kickoff_pin(ONE      );}
        else if (last_button == ONE      ) {button = AMPS       ;kickoff_pin(AMPS     );}
        else if (last_button == AMPS     ) {button = DECIMAL    ;kickoff_pin(DECIMAL  );}
        else if (last_button == DECIMAL  ) {button = ZERO       ;kickoff_pin(ZERO     );}
        else if (last_button == ZERO     ) {button = uA         ;kickoff_pin(uA       );}
        else if (last_button == uA       ) {button = BACKSPACE  ;kickoff_pin(BACKSPACE);}
        last_button = button;
        key_pending_counter = 0;
    }
    else if (TIFR1 & _BV(ICF1)) // There is a pending key, see if has completed its fall through VTH yet.
    {                           // ICF1 in TIFR1 is the input capture event flag, something happened.
        if      (button == NINE      ) {NINE_key.button_down      = process_key(&NINE_key     , IO);}
        else if (button == EIGHT     ) {EIGHT_key.button_down     = process_key(&EIGHT_key    , IO);}
        else if (button == SEVEN     ) {SEVEN_key.button_down     = process_key(&SEVEN_key    , IO);}
        else if (button == mA        ) {mA_key.button_down        = process_key(&mA_key       , IO);}
        else if (button == HI_LO     ) {HI_LO_key.button_down     = process_key(&HI_LO_key    , IO);}
        else if (button == SIX       ) {SIX_key.button_down       = process_key(&SIX_key      , IO);}
        else if (button == FIVE      ) {FIVE_key.button_down      = process_key(&FIVE_key     , IO);}
        else if (button == FOUR      ) {FOUR_key.button_down      = process_key(&FOUR_key     , IO);}
        else if (button == THREE     ) {THREE_key.button_down     = process_key(&THREE_key    , IO);}
        else if (button == TWO       ) {TWO_key.button_down       = process_key(&TWO_key      , IO);}
        else if (button == ONE       ) {ONE_key.button_down       = process_key(&ONE_key      , IO);}
        else if (button == AMPS      ) {AMPS_key.button_down      = process_key(&AMPS_key     , IO);}
        else if (button == DECIMAL   ) {DECIMAL_key.button_down   = process_key(&DECIMAL_key  , IO);}
        else if (button == ZERO      ) {ZERO_key.button_down      = process_key(&ZERO_key     , IO);}
        else if (button == uA        ) {uA_key.button_down        = process_key(&uA_key       , IO);}
        else if (button == BACKSPACE ) {BACKSPACE_key.button_down = process_key(&BACKSPACE_key, IO);}
        
        // This code is handy if the kaypad is ill behaved. It's a key monitor printed to the terminal. Needs IO passed in here.
        // if (NINE_key.button_down     ) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // if (EIGHT_key.button_down    ) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // if (SEVEN_key.button_down    ) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // if (mA_key.button_down       ) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // if (HI_LO_key.button_down    ) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // if (SIX_key.button_down      ) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // if (FIVE_key.button_down     ) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // if (FOUR_key.button_down     ) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // if (THREE_key.button_down    ) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // if (TWO_key.button_down      ) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // if (ONE_key.button_down      ) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // if (AMPS_key.button_down     ) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // if (DECIMAL_key.button_down  ) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // if (ZERO_key.button_down     ) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // if (uA_key.button_down       ) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // if (BACKSPACE_key.button_down) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // fprintf(IO.USB_stream, ":%d \r", (button >> 4));
        update_keys();                          // Deal with these keys.
    }
    else                                        // We must be waiting for a key to finish.
    {                                           // If it takes too long assume we're stuck!
        key_pending_counter++;                  // Empirically this only gets to about 4 max value
        if (key_pending_counter >= 100)
        {
            key_pending = false;                   // Kick start this bitch
            key_pending_counter = 0;
        }
    }
}
/****************************************************************************
*  Now that the last key has been processed look for the next key           *
*****************************************************************************/
void update_keys()
{    
    if (!key_encountered)
        {
        if      (BACKSPACE_key.button_down  )   {key_encountered = true; key_acknowledged = false; hot_key = BACKSPACE  ; keypad_timeout = 0;}
        else if (NINE_key.button_down       )   {key_encountered = true; key_acknowledged = false; hot_key = NINE       ; keypad_timeout = 0;}
        else if (EIGHT_key.button_down      )   {key_encountered = true; key_acknowledged = false; hot_key = EIGHT      ; keypad_timeout = 0;}
        else if (SEVEN_key.button_down      )   {key_encountered = true; key_acknowledged = false; hot_key = SEVEN      ; keypad_timeout = 0;}
        else if (mA_key.button_down         )   {key_encountered = true; key_acknowledged = false; hot_key = mA         ; keypad_timeout = 0;}
        else if (HI_LO_key.button_down      )   {key_encountered = true; key_acknowledged = false; hot_key = HI_LO      ; keypad_timeout = 0;}
        else if (SIX_key.button_down        )   {key_encountered = true; key_acknowledged = false; hot_key = SIX        ; keypad_timeout = 0;}
        else if (FIVE_key.button_down       )   {key_encountered = true; key_acknowledged = false; hot_key = FIVE       ; keypad_timeout = 0;}
        else if (FOUR_key.button_down       )   {key_encountered = true; key_acknowledged = false; hot_key = FOUR       ; keypad_timeout = 0;}
        else if (THREE_key.button_down      )   {key_encountered = true; key_acknowledged = false; hot_key = THREE      ; keypad_timeout = 0;}
        else if (TWO_key.button_down        )   {key_encountered = true; key_acknowledged = false; hot_key = TWO        ; keypad_timeout = 0;}
        else if (ONE_key.button_down        )   {key_encountered = true; key_acknowledged = false; hot_key = ONE        ; keypad_timeout = 0;}
        else if (AMPS_key.button_down       )   {key_encountered = true; key_acknowledged = false; hot_key = AMPS       ; keypad_timeout = 0;}
        else if (DECIMAL_key.button_down    )   {key_encountered = true; key_acknowledged = false; hot_key = DECIMAL    ; keypad_timeout = 0;}
        else if (ZERO_key.button_down       )   {key_encountered = true; key_acknowledged = false; hot_key = ZERO       ; keypad_timeout = 0;}
        else if (uA_key.button_down         )   {key_encountered = true; key_acknowledged = false; hot_key = uA         ; keypad_timeout = 0;}
        }
    else if (debounce_time < 0xFFFE)
        debounce_time++;

    if ((debounce_time > DEBOUNCE_TIME) &&
        !(  BACKSPACE_key.button_down   ||
            NINE_key.button_down        ||
            EIGHT_key.button_down       ||
            SEVEN_key.button_down       ||
            mA_key.button_down          ||
            HI_LO_key.button_down       ||
            SIX_key.button_down         ||
            FIVE_key.button_down        ||
            FOUR_key.button_down        ||
            THREE_key.button_down       ||
            TWO_key.button_down         ||
            ONE_key.button_down         ||
            AMPS_key.button_down        ||
            DECIMAL_key.button_down     ||
            ZERO_key.button_down        ||
            uA_key.button_down)       
        )
    {
        key_encountered = false;
        debounce_time   = 0;
    }
    if (keypad_timeout < 0xFFFE) keypad_timeout++;
}
/**************************************************************************
*  Respond to gesture command.                                            *
***************************************************************************/
void acquire_command(IO_pointers_t IO)
{
    static char input_string[30];
    static uint8_t index = 0;
    if (!key_acknowledged)
    {
        if (hot_key == HI_LO)
            toggle_range(IO);
        else
        {
            if (!get_entry_mode_screen_active())
                setup_entry_mode_screen(IO);
            switch(hot_key)
            {   
                case BACKSPACE: if (index >  0)               {LCD_Backspace();                        index--;                    }  break;
                case ZERO     : if (index < MAX_ENTRY_CHARS)  {fprintf(IO.LCD_stream, "%s", "0");      input_string[index++] = '0';}  break;
                case ONE      : if (index < MAX_ENTRY_CHARS)  {fprintf(IO.LCD_stream, "%s", "1");      input_string[index++] = '1';}  break;
                case TWO      : if (index < MAX_ENTRY_CHARS)  {fprintf(IO.LCD_stream, "%s", "2");      input_string[index++] = '2';}  break;
                case THREE    : if (index < MAX_ENTRY_CHARS)  {fprintf(IO.LCD_stream, "%s", "3");      input_string[index++] = '3';}  break;
                case FOUR     : if (index < MAX_ENTRY_CHARS)  {fprintf(IO.LCD_stream, "%s", "4");      input_string[index++] = '4';}  break;
                case FIVE     : if (index < MAX_ENTRY_CHARS)  {fprintf(IO.LCD_stream, "%s", "5");      input_string[index++] = '5';}  break;
                case SIX      : if (index < MAX_ENTRY_CHARS)  {fprintf(IO.LCD_stream, "%s", "6");      input_string[index++] = '6';}  break;
                case SEVEN    : if (index < MAX_ENTRY_CHARS)  {fprintf(IO.LCD_stream, "%s", "7");      input_string[index++] = '7';}  break;
                case EIGHT    : if (index < MAX_ENTRY_CHARS)  {fprintf(IO.LCD_stream, "%s", "8");      input_string[index++] = '8';}  break;
                case NINE     : if (index < MAX_ENTRY_CHARS)  {fprintf(IO.LCD_stream, "%s", "9");      input_string[index++] = '9';}  break;
                case DECIMAL  : if (index < MAX_ENTRY_CHARS)  {fprintf(IO.LCD_stream, "%s", ".");      input_string[index++] = '.';}  break;
                case uA       : if (index < MAX_ENTRY_CHARS)  {fprintf(IO.LCD_stream, "%s", "uA");     input_string[index++] = 'U';}  break;
                case mA       : if (index < MAX_ENTRY_CHARS)  {fprintf(IO.LCD_stream, "%s", "mA");     input_string[index++] = 'M';}  break;
                case AMPS     : if (index < MAX_ENTRY_CHARS)  {fprintf(IO.LCD_stream, "%s", "A");      input_string[index++] = 'A';}  break;
            }
            clear_keypad_timer();
            input_string[index] = NUL;
            if ((hot_key == AMPS) || (hot_key == mA) || (hot_key == uA))
            {
                set_current_from_keypad(input_string, IO);
                set_entry_mode_screen_active(false);
                index = 0;
            }
        }
        key_acknowledged = true;
    }
    if (get_keypad_timer() > KEYPAD_DWELL_TIME)
    {
        set_entry_mode_screen_active(false);
        index = 0;
        clear_keypad_timer();  // Added on 4/9/2019 by FL. Untested, but if we don't have it, I think this if-clause will fire again the next time this function is called and we'll be forced out of the entry mode screen prematurely.
    }    
}
/****************************************************************************
*                                                                           *
*****************************************************************************/
void clear_keypad_timer()
{
    keypad_timer = 0;
}
/****************************************************************************
*                                                                           *
*****************************************************************************/
long get_keypad_timer()
{
    return keypad_timer;
}