/****************************************************************************
 *                                                                          *
 *  Awesomefaucet                                                           *
 *                                                                          *
 ****************************************************************************/
#include "Awesomerfaucet.h"

/**************************************************************************
*  Create the timer update control global                                 *
***************************************************************************/
uint16_t time = 0;
/**************************************************************************
*                            Main                                         *
***************************************************************************/
int main(void)
{
    /**************************************************************************
    *  Create data structures                                                 *
    ***************************************************************************/
    static FILE         USB_stream;                         // Create the USB Stream data
    i2c_twi_port_t      i2cport;
    IO_pointers_t       IO;                                 // Create the passable IO pointer
    char                str_in[MAX_IN_STR_LEN+1] = "";      // Incoming string
    int                 str_len = 0;                        // Length of incoming string
    scpi_commands_P_t   commands_P[COMMAND_ARRAY_SIZE];     // Create the SCPI command array
    CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USB_stream);
    /**************************************************************************
    *  Assign values to data structures and setup hardware                    *
    ***************************************************************************/
    IO.USB_stream = &USB_stream;                    // Assign the USB stream field
    IO.I2C_port = &i2cport;                         // Assign the I2C stream field
    i2cport.sdaport = &SDAPORT;                     // Assign the SDAPORT field
    i2cport.sclport = &SCLPORT;                     // Assign the SCLPORT field
    i2cport.sdaddr = &SDADDR;                       // Assign the SDADDR field
    i2cport.sclddr = &SCLDDR;                       // Assign the SCLDDR field
    i2cport.sdapin = &SDAPIN;                       // Assign the SDAPIN field
    i2cport.sclpin = &SCLPIN;                       // Assign the SCLPIN field
    i2cport.sda = SDA;                              // Assign the SDA field
    i2cport.scl = SCL;                              // Assign the SCL field
    i2cport.twbr = 25;                              // Assign the bit rate field - SCL frequency = (CPU Clock frequency)/(16 + 2*TWBR * 4^TWPS)
    i2cport.twps = 0;                               // Note: TWBR should be 10 or higher if the TWI operates in Master mode. If TWBR is lower
                                                    // than 10, the Master may produce an incorrect output on SDA and SCL for the reminder
                                                    // of the byte. The problem occurs when operating the TWI in Master mode, sending
                                                    // Start + SLA + R/W to a Slave (a Slave does not need to be connected to the bus for
                                                    // the condition to happen).
                                                    // With twbr = 25, twps = 0, SCL should be 16Mhz / (16 + 50*1) = 242Khz or 4.125µs/bit
    CLKPR =_BV(CLKPCE);                             // Enable CLK prescaler change (CLKPCE). The CLKPCE bit is only updated when the other bits in CLKPR are simultaneously written to zero
    CLKPR = 0;                                      // Change CLK prescaler to divide by 1 (CLKPS[3..0])
    MCUSR &= ~_BV(WDRF);                            // Clear watchdog bit
    MCUCR |= _BV(JTD);                              // Disable JTAG.
    MCUCR |= _BV(JTD);                              // Manual Pg 323. "The application software must write this bit to the desired value twice within four cycles to change its value."
                                                    // These lines needed to restart after USB flashing for some reason.
    wdt_disable();                                  // Disable watchdog if enabled by bootloader/fuses
    TCCR0B              = _BV(CS01) | _BV(CS00);    // 16MHz, 62.5ns * 64 prescale = 4µs tick time * 256 = 1.024ms overflow. Timer overflow is used to run the soft timers.
                                                    // This is the minimum USB process time to avoid buffer problems. See Table 15-9 in the ATMEGA32u4 datasheet.
    TIMSK0              |= _BV(TOIE0);              // Detect Timer 0 overflow changes as interrupt
    WATERDDR            |=  WATER;                  // Set the WATER port pin direction to OUTPUT
    WATERPORT           &= ~WATER;                  // Set the WATER to off by setting the port pin low
    LASERDDR            |=  LASER;                  // Set the LASER port pin direction to OUTPUT
    LASERPORT           &= ~LASER;                  // Set the LASER to off by setting the port pin low
    TCCR4B              |= _BV(CS43) |              // Clock / 512 (Not sure what the prescaler is)
                           _BV(CS41);               //
    TCCR4C              |= _BV(COM4D1) |            // Cleared on Compare Match. Set when TCNT4 = 0x000. This should also have the effect of modulating the OC4D pins automatically.
                           _BV(PWM4D);              // Enables the PWM mode of 4D
    _delay_ms(512);                                 // Help USB get recognized on reset
    USB_Init();                                     // LUFA stuff
    CDC_init();                                     // LUFA stuff
    sei();                                          // Enable interrupts
    i2cTwiInit(IO.I2C_port);                        // Initialize I2C TWI Port
    Setup_ScpiCommandsArray_P(commands_P);          // Build the command array (mostly pointers to PROGMEM)
	retrieve_IIR_alpha(IO);                         // Retrieve IIR value from EEPROM
    
    
    // TODO RETREIVE THRESHOLD VALUE (LIKE 2mm etc).
    
    retrieve_laserpower_setting();                  // Retrieve laser power value from EEPROM
	set_laserpower();                               // Set the laser power
	VL53L4CD_SensorInit(0x52);                      // Initialize the sensor
    VL53L4CD_SetRangeTiming(0x52, 50, 0);           // Address, timing budget (ms), inter-measurement (0 means no shutdown, take next reading)
    VL53L4CD_StartRanging(0x52);                    // Kick off the first reading
    /****************************************************************************
     *        Main Loop                                                         *
     ****************************************************************************/
    while(true)
    {
        process_USB();
        process_scpi_input(str_in, &str_len, commands_P, IO);
        process_sensor();
        update_water();
    }
}
/****************************************************************************
*    Interrupt Service Routine for Timer 0 (62.5ns * 64 * 256 = 1.024ms)    *
*****************************************************************************/
ISR(TIMER0_OVF_vect)
{
    cli();
    process_soft_timers();
    sei();
}