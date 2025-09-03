#include "USB_Load_Box.h"

/**************************************************************************
*  Create up a TWI I2C port with hyper mega global scope                  *
***************************************************************************/
i2c_port_t  i2cport_1;
uint16_t    Soft_LCD_NTC_Timer = 0;
bool        time_to_update_LCD_NTC  = NO;
bool        update_timers = NO;
long        LCD_temperature_timer = 0;
uint16_t    keypad_timer = 0;
/**************************************************************************
*                            Main                                         *
***************************************************************************/
int main(void)
{
/**************************************************************************
*  Create data structures                                                 *
***************************************************************************/
    static FILE         USB_stream;                        // Create the USB Stream data
    static FILE         LCD_stream;                        // Create the LCD Stream data
    IO_pointers_t       IO;                                // Create the passable IO pointer
    char                str_in[MAX_IN_STR_LEN+1] = "";    // Incoming string
    int                 str_len = 0;                    // Length of incoming string
    scpi_commands_P_t   commands_P[COMMAND_ARRAY_SIZE];    // Create the SCPI command array
    int                 command_count;                    // Used to check that commands were counted properly
    CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USB_stream);
    fdev_setup_stream(&LCD_stream, LCD_Put_Char, NULL, _FDEV_SETUP_WRITE);
//    static FILE LCD_stream = FDEV_SETUP_STREAM(LCD_Put_Char, NULL, _FDEV_SETUP_WRITE);
//    to use this format, comment out the line "static FILE LCD_stream";
/**************************************************************************
*  Assign values to data structures                                       *
***************************************************************************/
    IO.USB_stream = &USB_stream;                                    // Assign the USB stream field
    IO.LCD_stream = &LCD_stream;                                    // Assign the LCD stream field
    IO.I2C_port1 = &i2cport_1;                                      // Assign the I2C stream field
    i2cport_1.sdaport = &SDAPORT;                                   // Assign the SDAPORT field
    i2cport_1.sclport = &SCLPORT;                                   // Assign the SCLPORT field
    i2cport_1.sdaddr = &SDADDR;                                     // Assign the SDADDR field
    i2cport_1.sclddr = &SCLDDR;                                     // Assign the SCLDDR field
    i2cport_1.sdapin = &SDAPIN;                                     // Assign the SDAPIN field
    i2cport_1.sclpin = &SCLPIN;                                     // Assign the SCLPIN field
    i2cport_1.sda = SDA;                                            // Assign the SDA field
    i2cport_1.scl = SCL;                                            // Assign the SCL field
    i2cport_1.br = 25;                                              // Assign the bit rate field - SCL frequency = (CPU Clock frequency)/(16 + 2*BR * 4^PS)
    i2cport_1.ps = 0;                                               // Note: TWBR should be 10 or higher if the TWI operates in Master mode. If TWBR is lower
                                                                    // than 10, the Master may produce an incorrect output on SDA and SCL for the reminder
                                                                    // of the byte. The problem occurs when operating the TWI in Master mode, sending
                                                                    // Start + SLA + R/W to a Slave (a Slave does not need to be connected to the bus for
                                                                    // the condition to happen).
    SetupHardware();                                                // Initialize ports, pins and timers
    _delay_ms(512);                                                 // Help USB get recognized on reset
    USB_Init();                                                     // LUFA stuff
    CDC_init();                                                     // LUFA stuff
    sei();                                                          // Enable interrupts
    i2cTwiInit(IO.I2C_port1);                                       // Initialize I2C TWI Port
    LCD_Init();                                                     // Initialize the LCD display. Requires an already setup I2C port.
    command_count = Setup_ScpiCommandsArray_Loadbox_P (commands_P); // Build the command array (mostly pointers to PROGMEM)
    // This is an error message if the command count isn't right
    if (command_count != COMMAND_ARRAY_SIZE)
    {
        LCD_Clear();
        LCD_Home();
        scpi_prStr_P(PSTR("Array Size "), IO.LCD_stream);
        fprintf(IO.LCD_stream, "%d", command_count);
    }
    scpi_set_dac(0, IO.I2C_port1);  // Added 4/9/2019 by FL: if load box is delivering current when micro is reset,
                                    // then when firmware boots up, the load box is still delivering current (DAC not reset)
                                    // even though the display reads zero current, which is misleading.
    setup_icons();                                                  // Send the H, L and lock icons to the screen
    Run_Intro_Screen(IO);                                           // Splash Screen on LCD
    // fprintf(IO.USB_stream, "%s\r\n", "987mR654321A.0uB");           //Use in conjunction with the key monitor in touchpad.c
    for (EVER)
    {
        process_USB();
        scpi_process_input(str_in, &str_len, commands_P, IO);
        process_keypad(IO);                                         // IO here only needed for debug.
        acquire_command(IO);
        process_soft_timers(IO);
    }
} // END main()
/****************************************************************************
*  LCD_Put_Char is a wrapper to bundle the I2C port argument                *
*  so that a single argument function can be passed to the stream handler   *
*****************************************************************************/
int LCD_Put_Char (char the_char, FILE *stream)
{
        LCD_Display_Char(the_char);
        return 0;
}
/****************************************************************************
 *            SetupHardware                                                    *
 *        Configures the board hardware and chip peripherals                    *
 ***************************************************************************/
 void SetupHardware(void)
{
    CLKPR =_BV(CLKPCE);                             // Enable CLK prescaler change (CLKPCE)
    CLKPR = 0;                                      // Change CLK prescaler to divide by 1 (CLKPS[3..0])
    MCUSR &= ~_BV(WDRF);                            // Clear watchdog bit
    MCUCR |= _BV(JTD);                              // Used port F for the touch pad so need to disable JTAG.
    MCUCR |= _BV(JTD);                              // Manual Pg 323. "The application software must write this bit to the desired value twice within four cycles to change its value."
    wdt_disable();                                  // Disable watchdog if enabled by bootloader/fuses
    TCCR0A                = 0;                      // 8 bit Timer 0 Setup Initialization
    TCCR0B                = _BV(CS01);              // 8MHz, 125ns * 8 prescale = 1us tick time * 256 = 256us overflow. This is used to run the soft timers.
                                                    // This is the minimum USB process time to avoid buffer problems. See Table 15-9 in the ATMEGA32u4 datasheet.
    TCCR1A                = 0;                      // 16-bit Timer 1 Setup Initialization. This timer is used for touch pad timing and is not available otherwise.
    TCCR1B                = 0;                      // Timer 1 disabled - Why?
    PCMSK0                = 0;                      // No pin change interrupts on this bank
    TIMSK0               |= _BV(TOIE0);             // Detect Timer 0 Overflow Changes as Interrupt
    DACDDR               |= LDAC | MOSI | SCLK;     // Set the LDAC port line direction to output
    DACPORT              |= LDAC;                   // Set the LDAC line high - its resting state
    SPCR                  = (0 << SPIE)    |
                            (1 << SPE )    |
                            (0 << DORD)    |
                            (1 << MSTR)    |
                            (0 << CPOL)    |
                            (0 << CPHA)    |
                            (0 << SPR1)    |
                            (0 << SPR0);            // 0x50;// SPE and MSTR set, all other SPCR bits low.
    LCD_LED_PORT         &= ~LCD_LED;               // Set the LCD LED to off by setting the port pin low
    LCD_LED_DDR          |= LCD_LED;                // Set the LCD LED port pin direction to OUTPUT
    I2P5A_PORT           &= ~I2P5A;                 // Set the Range Transistor port pin to low
    I2P5A_DDR            |= I2P5A;                  // Set the Range Transistor port pin direction to OUTPUT
    LCD_RST_DDR          |= LCD_RST;                // Set the LCD RESET port pin direction to OUTPUT
    LCD_RST_PORT         &= ~LCD_RST;               // LCD display is active reset low. Ensure it's low first.
    LCD_RST_PORT         |= LCD_RST;                // Now high.
    LOW_CUR_ASSIST_DDR   |= LOW_CUR_ASSIST;         // Set the Low Current Assist port pin direction to OUTPUT
    LOW_CUR_ASSIST_PORT  &= ~LOW_CUR_ASSIST;        // Set the Low Current Assist port pin to low    
    ZERO_CURRENT_DDR     &= ~ZERO_CURRENT;          // Set the Force Zero Current port pin direction to INPUT
    ZERO_CURRENT_PORT    &= ~ZERO_CURRENT;          // Set the Force Zero Current port pin to low for hiZ, Set it to 1 for weak pull up.   
    DROPOUT_DTECTR_DDR   &= ~DROPOUT_DTECTR_PIN;    // Set dropout detector pin direction to IN and
    DROPOUT_DTECTR_PORT  |= DROPOUT_DTECTR_PIN;     // enable pullup resistor. The HTX9000SE's dropout signal is from an open drain.
// *********** KEYPAD Pins **************************************************************************************************
    KEYPAD_ADDR_DDR      |=(KEYPAD_ADDR0 |
                            KEYPAD_ADDR1 |
                            KEYPAD_ADDR2 |
                            KEYPAD_ADDR3);          // Set keypad scanner selector mux port pins direction to OUTPUT.
// **************************************************************************************************************************
    FAN_DDR              |= FAN;                    // Set the Fan port direction to OUTPUT
    NTC_ADC_DDR          &= ~NTC_ADC;               // Set the pin to input mode so the thermistor isn't shorted out.
    NTC_ADC_PORT         &= ~NTC_ADC;               // May be redundant, done by the uController already?
    ADC_DIDR             |= ADC_DIDR_BIT;           // Disable the digital input bit on the thermocouple input to the ADC to kill the floating gate.
    ADMUX                 = AVCC_REF       |        // Set to external reference for thermistor.
                        _BV(ADLAR)         |        // Set ADC readings to be left justified.
                         NTC_ADC_INPUTA;            // Set the MUX[4:0] to point to the thermistor single ended (still needs MUX[5] on ADCSRB).
    ADCSRA                = (1 << ADEN ) |          // Enable the A/D
                            (1 << ADPS2)    |       // ADPS2 - ADPS1 are the
                            (0 << ADPS1)    |       // ADC Clock Divider
                            (0 << ADPS0);           // Setting just ADPS2 high gives /16: 62.5kHz, 2us/bit, ~27uS conversion
    ADCSRB                = NTC_ADC_INPUTB;         // makes up the rest of the MUX[5:0] value.
} // END SetupHardware()
/****************************************************************************
*    Interrupt Service Routine for Timer 0 256us-OVF tdiv = 1us/tick        * Somehow about 1ms/cycle???
*****************************************************************************/
ISR(TIMER0_OVF_vect)
{update_timers = YES;}
/****************************************************************************
*                                                                           *
*****************************************************************************/
void clear_keypad_timer()
{keypad_timer = 0;}
/****************************************************************************
*                                                                           *
*****************************************************************************/
long get_keypad_timer()
{return keypad_timer;}
/****************************************************************************
*                                                                           *
*****************************************************************************/
void process_soft_timers(IO_pointers_t IO)
{    
    if (update_timers)
    {
        keypad_timer++;
        LCD_temperature_timer++;
        update_timers = NO;
    }

    if (LCD_temperature_timer > INT_CYCLES_PER_LCD_NTC_UPDATE)
    {  
        if (!get_entry_mode_screen_active())
            update_screen(IO);
        LCD_temperature_timer = 0;
    }
}

































