/**************************************************************************
*                                                                         *
* Touchpad                                                                *
*                                                                         *
***************************************************************************/

#include "HTX9000SE_touchpad.h"
/****************************************************************************
*                        Touch Pad Variables                                *
*****************************************************************************/
touchpad_key_t static BACKSPACE_key = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t static NINE_key      = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t static EIGHT_key     = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t static SEVEN_key     = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t static mA_key        = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t static HI_LO_key     = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t static SIX_key       = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t static FIVE_key      = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t static FOUR_key      = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t static THREE_key     = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t static TWO_key       = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t static ONE_key       = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t static AMPS_key      = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t static DECIMAL_key   = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t static ZERO_key      = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};
touchpad_key_t static uA_key        = {.button_down = false, .fastest_falltime = FALLTIME_INIT, .leaktimer = 0};

uint16_t    static debounce_time        = 0;
uint16_t    static keypad_timeout       = 0;
uint8_t     static key_pending_counter  = 0;
uint8_t     static hot_key              = KEYPAD_uA;
uint8_t     static this_key             = KEYPAD_uA;
uint8_t     static last_key             = KEYPAD_uA;
bool        static key_pending          = false;
bool        static key_encountered      = false;
bool        static key_acknowledged     = true;
extern      uint16_t keypad_timer;
/****************************************************************************
*    Kickoff the keypad key for timing measurement                          *
*    Each this_key pin is high coming in here. First it is changed            *
*    from low-Z hi to pull-up resistor mode with the DDR bit. Then with the *
*    "PORTx" bit it's set to pure hi Z.                                     *
*****************************************************************************/
void kickoff_pin(uint8_t this_key)
{
    KEYPAD_ADDR_PORT    &= 0x0F;                            // Switch the key address selection to 'this' key.
    KEYPAD_ADDR_PORT    |= this_key;                          // The wiring is such that the bits line up with PF7,PF6,PF5,PF4 and the this_key enums are pre-shifted to line up.
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
/************************************************************************************
*           Routine for Measuring the Touch Buttons                                 *
*           ---------------------------------------                                 *
*                                                                                   *
*    Each this_key pin is high coming in here.                                      *
*    First it is changed from low-Z hi to pull-up resistor mode with the DDR bit.   *
*    Then with the "PORTx" bit it's set to pure hi Z.                               *
*************************************************************************************/
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
        // if      (last_key == KEYPAD_BACKSPACE) {this_key = NINE       ;kickoff_pin(KEYPAD_NINE     );}
        // else if (last_key == KEYPAD_NINE     ) {this_key = EIGHT      ;kickoff_pin(KEYPAD_EIGHT    );}
        // else if (last_key == KEYPAD_EIGHT    ) {this_key = SEVEN      ;kickoff_pin(KEYPAD_SEVEN    );}
        // else if (last_key == KEYPAD_SEVEN    ) {this_key = mA         ;kickoff_pin(KEYPAD_mA       );}
        // else if (last_key == KEYPAD_mA       ) {this_key = HI_LO      ;kickoff_pin(KEYPAD_HI_LO    );}
        // else if (last_key == KEYPAD_HI_LO    ) {this_key = SIX        ;kickoff_pin(KEYPAD_SIX      );}
        // else if (last_key == KEYPAD_SIX      ) {this_key = FIVE       ;kickoff_pin(KEYPAD_FIVE     );}
        // else if (last_key == KEYPAD_FIVE     ) {this_key = FOUR       ;kickoff_pin(KEYPAD_FOUR     );}
        // else if (last_key == KEYPAD_FOUR     ) {this_key = THREE      ;kickoff_pin(KEYPAD_THREE    );}
        // else if (last_key == KEYPAD_THREE    ) {this_key = TWO        ;kickoff_pin(KEYPAD_TWO      );}
        // else if (last_key == KEYPAD_TWO      ) {this_key = ONE        ;kickoff_pin(KEYPAD_ONE      );}
        // else if (last_key == KEYPAD_ONE      ) {this_key = AMPS       ;kickoff_pin(KEYPAD_AMPS     );}
        // else if (last_key == KEYPAD_AMPS     ) {this_key = DECIMAL    ;kickoff_pin(KEYPAD_DECIMAL  );}
        // else if (last_key == KEYPAD_DECIMAL  ) {this_key = ZERO       ;kickoff_pin(KEYPAD_ZERO     );}
        // else if (last_key == KEYPAD_ZERO     ) {this_key = uA         ;kickoff_pin(KEYPAD_uA       );}
        // else if (last_key == KEYPAD_uA       ) {this_key = KEYPAD_BACKSPACE  ;kickoff_pin(KEYPAD_BACKSPACE);}
        if (last_key == KEYPAD_uA       ) {this_key = KEYPAD_uA  ;kickoff_pin(KEYPAD_uA);}
        last_key = this_key;
        key_pending_counter = 0;
    }
    else if (TIFR1 & _BV(ICF1)) // There is a pending key, see if has completed its fall through VTH yet.
    {                           // ICF1 in TIFR1 is the input capture event flag, something happened.
        // if      (this_key == KEYPAD_NINE      ) {NINE_key.button_down      = process_key(&NINE_key     , IO);}
        // else if (this_key == KEYPAD_EIGHT     ) {EIGHT_key.button_down     = process_key(&EIGHT_key    , IO);}
        // else if (this_key == KEYPAD_SEVEN     ) {SEVEN_key.button_down     = process_key(&SEVEN_key    , IO);}
        // else if (this_key == KEYPAD_mA        ) {mA_key.button_down        = process_key(&mA_key       , IO);}
        // else if (this_key == KEYPAD_HI_LO     ) {HI_LO_key.button_down     = process_key(&HI_LO_key    , IO);}
        // else if (this_key == KEYPAD_SIX       ) {SIX_key.button_down       = process_key(&SIX_key      , IO);}
        // else if (this_key == KEYPAD_FIVE      ) {FIVE_key.button_down      = process_key(&FIVE_key     , IO);}
        // else if (this_key == KEYPAD_FOUR      ) {FOUR_key.button_down      = process_key(&FOUR_key     , IO);}
        // else if (this_key == KEYPAD_THREE     ) {THREE_key.button_down     = process_key(&THREE_key    , IO);}
        // else if (this_key == KEYPAD_TWO       ) {TWO_key.button_down       = process_key(&TWO_key      , IO);}
        // else if (this_key == KEYPAD_ONE       ) {ONE_key.button_down       = process_key(&ONE_key      , IO);}
        // else if (this_key == KEYPAD_AMPS      ) {AMPS_key.button_down      = process_key(&AMPS_key     , IO);}
        // else if (this_key == KEYPAD_DECIMAL   ) {DECIMAL_key.button_down   = process_key(&DECIMAL_key  , IO);}
        // else if (this_key == KEYPAD_ZERO      ) {ZERO_key.button_down      = process_key(&ZERO_key     , IO);}
        // else if (this_key == KEYPAD_uA        ) {uA_key.button_down        = process_key(&uA_key       , IO);}
        if (this_key == KEYPAD_uA        ) {uA_key.button_down        = process_key(&uA_key       , IO);}
        // else if (this_key == KEYPAD_BACKSPACE ) {BACKSPACE_key.button_down = process_key(&BACKSPACE_key, IO);}
        
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
        if (uA_key.button_down       ) fprintf(IO.USB_stream, "%s", "uA"); else fprintf(IO.USB_stream, "%s", "  ");
        // if (BACKSPACE_key.button_down) fprintf(IO.USB_stream, "%s", "X"); else fprintf(IO.USB_stream, "%s", " ");
        // fprintf(IO.USB_stream, ":%d \r", (this_key >> 4));
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
        if      (BACKSPACE_key.button_down  )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_BACKSPACE  ; keypad_timeout = 0;}
        else if (NINE_key.button_down       )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_NINE       ; keypad_timeout = 0;}
        else if (EIGHT_key.button_down      )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_EIGHT      ; keypad_timeout = 0;}
        else if (SEVEN_key.button_down      )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_SEVEN      ; keypad_timeout = 0;}
        else if (mA_key.button_down         )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_mA         ; keypad_timeout = 0;}
        else if (HI_LO_key.button_down      )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_HI_LO      ; keypad_timeout = 0;}
        else if (SIX_key.button_down        )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_SIX        ; keypad_timeout = 0;}
        else if (FIVE_key.button_down       )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_FIVE       ; keypad_timeout = 0;}
        else if (FOUR_key.button_down       )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_FOUR       ; keypad_timeout = 0;}
        else if (THREE_key.button_down      )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_THREE      ; keypad_timeout = 0;}
        else if (TWO_key.button_down        )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_TWO        ; keypad_timeout = 0;}
        else if (ONE_key.button_down        )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_ONE        ; keypad_timeout = 0;}
        else if (AMPS_key.button_down       )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_AMPS       ; keypad_timeout = 0;}
        else if (DECIMAL_key.button_down    )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_DECIMAL    ; keypad_timeout = 0;}
        else if (ZERO_key.button_down       )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_ZERO       ; keypad_timeout = 0;}
        else if (uA_key.button_down         )   {key_encountered = true; key_acknowledged = false; hot_key = KEYPAD_uA         ; keypad_timeout = 0;}
    }
    else if (debounce_time != 0xFFFF)
        debounce_time++;

    if (
         (debounce_time > DEBOUNCE_TIME)&&
        !(BACKSPACE_key.button_down     ||
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
    if (keypad_timeout != 0xFFFF) keypad_timeout++;
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
        if (hot_key == KEYPAD_HI_LO)
            toggle_range(IO);
        else
        {
            if (!get_entry_mode_screen_active())
                setup_entry_mode_screen(IO);
            switch(hot_key)
            {   
                case KEYPAD_BACKSPACE: if (index >  0)              {LCD_Backspace();                      index--;                    }  break;
                case KEYPAD_ZERO     : if (index < MAX_ENTRY_CHARS) {fprintf(IO.LCD_stream, "%s", "0");    input_string[index++] = '0';}  break;
                case KEYPAD_ONE      : if (index < MAX_ENTRY_CHARS) {fprintf(IO.LCD_stream, "%s", "1");    input_string[index++] = '1';}  break;
                case KEYPAD_TWO      : if (index < MAX_ENTRY_CHARS) {fprintf(IO.LCD_stream, "%s", "2");    input_string[index++] = '2';}  break;
                case KEYPAD_THREE    : if (index < MAX_ENTRY_CHARS) {fprintf(IO.LCD_stream, "%s", "3");    input_string[index++] = '3';}  break;
                case KEYPAD_FOUR     : if (index < MAX_ENTRY_CHARS) {fprintf(IO.LCD_stream, "%s", "4");    input_string[index++] = '4';}  break;
                case KEYPAD_FIVE     : if (index < MAX_ENTRY_CHARS) {fprintf(IO.LCD_stream, "%s", "5");    input_string[index++] = '5';}  break;
                case KEYPAD_SIX      : if (index < MAX_ENTRY_CHARS) {fprintf(IO.LCD_stream, "%s", "6");    input_string[index++] = '6';}  break;
                case KEYPAD_SEVEN    : if (index < MAX_ENTRY_CHARS) {fprintf(IO.LCD_stream, "%s", "7");    input_string[index++] = '7';}  break;
                case KEYPAD_EIGHT    : if (index < MAX_ENTRY_CHARS) {fprintf(IO.LCD_stream, "%s", "8");    input_string[index++] = '8';}  break;
                case KEYPAD_NINE     : if (index < MAX_ENTRY_CHARS) {fprintf(IO.LCD_stream, "%s", "9");    input_string[index++] = '9';}  break;
                case KEYPAD_DECIMAL  : if (index < MAX_ENTRY_CHARS) {fprintf(IO.LCD_stream, "%s", ".");    input_string[index++] = '.';}  break;
                case KEYPAD_uA       : if (index < MAX_ENTRY_CHARS) {fprintf(IO.LCD_stream, "%s", "uA");   input_string[index++] = 'U';}  break;
                case KEYPAD_mA       : if (index < MAX_ENTRY_CHARS) {fprintf(IO.LCD_stream, "%s", "mA");   input_string[index++] = 'M';}  break;
                case KEYPAD_AMPS     : if (index < MAX_ENTRY_CHARS) {fprintf(IO.LCD_stream, "%s", "A");    input_string[index++] = 'A';}  break;
            }
            clear_keypad_timer();
            input_string[index] = NUL;
            if ((hot_key == KEYPAD_AMPS) || (hot_key == KEYPAD_mA) || (hot_key == KEYPAD_uA))
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