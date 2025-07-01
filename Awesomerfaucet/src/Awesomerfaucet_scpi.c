/****************************************************************************
 *                                                                          *
 *  Awesomerfaucet SCPI                                                     *
 *                                                                          *
 ****************************************************************************/

#include "Awesomerfaucet_scpi.h"

/**************************************************************************
*  Error Message Buffers have scope within this file                      *
***************************************************************************/
char bad_command[MAX_TOKEN_LEN + 1] = "";
PGM_P error_messages[ERROR_QUEUE_LEN + 1];
int error_number = 0;
char const error_mnemonic_too_long[]	PROGMEM = "-112,Program mnemonic too long";
char const error_arg_too_long[]			PROGMEM = "-112,Argument too long";
char const error_bad_path_or_header[] 	PROGMEM = "-113,Bad path or header: ";
uint8_t EEMEM LASER_POWER[MAX_LASER_CHARS];
uint8_t EEMEM IIR_VALUE[MAX_IIR_CHARS];
extern uint8_t iir_value;
bool water_auto = true;

extern long long max_reading;
extern long long IIR_range_reading;

/**************************************************************************
*  Build input string from terminal then run SCPI command.                *
***************************************************************************/
void process_scpi_input( char * str_in, int *str_len, scpi_commands_P_t cmd_array_P[], IO_pointers_t IO )
{
	int16_t		ReceivedByte;
	uint8_t		usb_data;										// Temporary USB data storage
	
	ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
	while (ReceivedByte >= 0)
	{	usb_data = toupper((char)ReceivedByte);					// Get all chars and convert to upper case.
		if ((usb_data == ';')			||						// Check for ';' or
			(usb_data == '\r')			||						// 'Enter' key or
			(usb_data == '\n'))									// Other 'Enter' key
		{	if (*str_len > 0)									// If SCPI command string of string isn't empty
				scpi_process_cmd_P(str_in, cmd_array_P, IO);	// Process
			str_in[0] = NUL;									// Empty str_in.
			*str_len = 0;										// Clear str_len counter
		}
		else
		{if (usb_data == '\b' || usb_data == DEL_KEY)			// If backspace or DELETE key (ASC127) and 
		{	if (*str_len > 0)									// it's not already 0
				str_in[--(*str_len)] = NUL;						// Decrement str_len, remove last char from str_in
		}
		else													// If not backspace and
		{	if (*str_len < MAX_IN_STR_LEN - 1)					// as long as the length is ok
			{	str_in[*str_len] = usb_data;					// pile on whatever is being sent to current str_in and increment str_len
				str_in[++(*str_len)] = NUL;						// and terminate the string with the NUL character
			}
			else
				scpi_prStr_P(PSTR("ERROR command too long\r\n"),IO.USB_stream);
		}
		}
	ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
	}
}
/**************************************************************************
*  Parse and process input string from terminal then run SCPI command.    *
***************************************************************************/
void scpi_process_cmd_P( char * input_string, scpi_commands_P_t cmd_array_P[], IO_pointers_t IO )
{
	char * token;
	char * argument = "";
	static char lname[MAX_TOKEN_LEN+1] = "";
	static char sname[MAX_TOKEN_LEN+1] = "";
	int cmd_i = 0;														// Command index for walking dictionary
	scpi_commands_P_t* current_state = NULL;							// Pointer to commands in command array. Initialize to the top (no command).
	scpi_commands_P_t* current_ptr;										// Pointer to commands in command array used for tokens.
	uint8_t valid_command = false;										// Valid command variable
	token = strtok (input_string,":");									// Scan string looking for : separators
	while (token != NUL)
	{//--------------------------------------------------------------->	// Search for new tokens
 		argument = strpbrk(token, " ");									// Check for space indicating an argument
 		valid_command = false;
		if (argument[0] == ' ' ) // or other chars
		{	argument[0] = NUL;											// Terminates last token
			argument++;													// Move argument pointer past space
		}
		if (strlen(token) > MAX_TOKEN_LEN)								// Check for token length error
		{	scpi_add_error_P(error_mnemonic_too_long, IO);				// Document the error
			break;														// Bail from the token search loop
		}
		if (strlen(argument) > MAX_ARG_LEN)								// Check for argument length error
		{	scpi_add_error_P(error_arg_too_long, IO);					// Document the error
			break;														// Bail from the token search loop
		}
		
		for(cmd_i = 0; cmd_i < COMMAND_ARRAY_SIZE; cmd_i++)				// Walk through known commands looking for a match
		{
			current_ptr = &cmd_array_P[cmd_i];							// Get pointer to found command
			if (current_ptr->parent == current_state) {					// only process children of the current state
				PGM_P_to_string(cmd_array_P[cmd_i].name, lname, IO.USB_stream);
				scpi_get_short_name(lname,sname);
				if ((strcmp(lname, token) == 0) || (strcmp(sname, token) == 0))		// If the command matches
				{
					current_state = current_ptr;						// Update current_state to new found token
					valid_command = true;
					break;
				}
			}
		}
		if (!valid_command) {
			// if a command wasn't found, determine if an implied command is
			// this function updates current_state
			valid_command = scpi_find_implied(&current_state, token, cmd_array_P);
		}

		if (!valid_command) {
			// if no match or implied match was not? found there was a real error
			scpi_add_error_P(error_bad_path_or_header, IO);				// Indicate bad token
			strncpy(bad_command, token, MAX_TOKEN_LEN);					// Send up to MAX_TOKEN_LEN of bad token
			return;
		}
		token = strtok (NULL, ":");										// Otherwise get the next : separated token
	}//--------------------------------------------------------------->	// End of new token search
	if (valid_command) {
		current_state->function(argument, IO);							// Run the handler function of the tail
	}
}
/**************************************************************************
*  Parser Find Implied                                                    *
***************************************************************************/
bool scpi_find_implied(scpi_commands_P_t **current_state, char *token, scpi_commands_P_t cmd_array_P[]) {
	scpi_commands_P_t* current_ptr;
	scpi_commands_P_t* parent_ptr;
	static char lname[MAX_TOKEN_LEN+1] = "";
	static char sname[MAX_TOKEN_LEN+1] = "";

	for(int cmd_i = 0; cmd_i < COMMAND_ARRAY_SIZE; cmd_i++)	 {				// Walk through known commands looking for a match
		current_ptr = &cmd_array_P[cmd_i];									// Get pointer to found command
		PGM_P_to_string(current_ptr->name, lname, NULL);
		scpi_get_short_name(lname, sname);
		if ((strcmp(lname, token) == 0) || (strcmp(sname, token) == 0))	{	// If the command matches
			do {
				parent_ptr = current_ptr->parent;
				if (parent_ptr == *current_state) {
					*current_state = &cmd_array_P[cmd_i];
					return true;
				} else if (parent_ptr == NULL) {
					break;
				} else if(parent_ptr->implied == true) {
					current_ptr = parent_ptr;
				} else {
					break;
				}
			} while(true);
		}
	}
	return false;
}
/**************************************************************************
*  Parser Get Short Name                                                  *
***************************************************************************/
void scpi_get_short_name(char *lname, char *sname) {
 	sname[0] = NUL;
	int lindex = 0;
	int sindex = 0;
	while (lname[lindex] != NUL)					    // Walk the name string
	{   
		if (lname[lindex] == toupper(lname[lindex]))    // Check if the character is uppercase
		{   
			sname[sindex] = lname[lindex];			    // build up the sname
			sindex++;   
		}   
		else    
			lname[lindex] = toupper(lname[lindex]); 
		lindex++;   
	}   
	sname[sindex] = NUL;							    // Terminate the sname string
}
/**************************************************************************
*  Adds error to error queue                                              *
***************************************************************************/
void scpi_add_error_P(PGM_P error_message, IO_pointers_t IO)
{
	if (error_number < ERROR_QUEUE_LEN)
		error_messages[++error_number] = error_message;
}
/**************************************************************************
*  Prints a program memory string one char at a time to the terminal      *
***************************************************************************/
void scpi_prStr_P( PGM_P arg, FILE *fstream )
{
	uint16_t string_index = 0;
	char the_char =  pgm_read_byte(arg);
	while (the_char != NUL)
	{	
		fputc(the_char,fstream);
		the_char = pgm_read_byte(&arg[++string_index]);
	}
}
/**************************************************************************
*  Prints a program memory string one char at a time to the terminal      *
***************************************************************************/
void PGM_P_to_string( PGM_P arg, char * name, FILE *fstream )
{
 	uint16_t i = 0;
	name[i] =  pgm_read_byte(arg);
	while (name[i] != NUL)
	{
		i++;
		name[i] = pgm_read_byte(&arg[i]);
	}
	i++;
	name[i] = NUL;
}
/**************************************************************************
*  Setup Awesomfaucet Specific SCPI commands and functions                *
***************************************************************************/
int Setup_ScpiCommandsArray_P( scpi_commands_P_t command_array_P[] )
{
	int i = 0;
	// 1 Top NULL node
	command_array_P[i].name      = PSTR("NULL");
	command_array_P[i].implied    = true;
	command_array_P[i].parent     = NULL;
	command_array_P[i++].function   = &scpi_null_func;

	command_array_P[i].name      = PSTR("*OPC?");
	command_array_P[i].implied    = false;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &st_OPC_q;
/**************************************************************************
*  Non-Compulsory SCPI commands                                           *
***************************************************************************/
	// 3 [[:]SYSTem]
	command_array_P[i].name      = PSTR("SYSTem");
	command_array_P[i].implied    = true;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &scpi_empty_func; // ****** Careful here, Loadbox_scpi.c refers to this directly as "&command_array_P[2]"

	// [[:]SYSTem:]PRESet

		// 4 [[:]SYSTem:]:RST
		command_array_P[i].name      = PSTR("RST");
		command_array_P[i].implied    = false;
		command_array_P[i].parent     = &command_array_P[i-1];
		command_array_P[i++].function = &scpi_empty_func;

			// 5 [[:]SYSTem:]:RST:BTLOader
			command_array_P[i].name      = PSTR("BTLOader");
			command_array_P[i].implied    = false;
			command_array_P[i].parent     = &command_array_P[i-1];
			command_array_P[i++].function = &sys_rst_btloader;

		// 6 [[:]SYSTem:]ERRor?
		command_array_P[i].name      = PSTR("ERRor?");
		command_array_P[i].implied    = false;
		command_array_P[i].parent     = &command_array_P[i-3];
		command_array_P[i++].function = &sys_error_q;
	// [[:]SYSTem:]ERRor
	// [[:]SYSTem:]ERRor:NEXT?
	// 7 [[:]SYSTem:][:]VERSion?
		command_array_P[i].name      = PSTR("VERSion?");
		command_array_P[i].implied    = false;
		command_array_P[i].parent     = &command_array_P[i-4]; // 
		command_array_P[i++].function = &scpi_get_version_q;
	// [[:]STATus]
	// [[:]STATus:]OPERation
	// [[:]STATus:]OPERation:CONDition?
	// [[:]STATus:]OPERation:ENABle
	// [[:]STATus:]OPERation:ENABle?
	// [[:]STATus:]QUEStionable
	// [[:]STATus:]QUEStionable:CONDition?
	// [[:]STATus:]QUEStionable:ENABle
	// [[:]STATus:]QUEStionable:ENABle?
	// [:]PRESet
    
	// 8 [:]*IDN?
	command_array_P[i].name       = PSTR("*IDN?");
	command_array_P[i].implied    = false;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &scpi_IDN_q;
    
	command_array_P[i].name       = PSTR("DEBUG?");
	command_array_P[i].implied    = false;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &debug;
    
	command_array_P[i].name       = PSTR("GET");
	command_array_P[i].implied    = false;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &scpi_null_func;
		
        command_array_P[i].name       = PSTR("IIR?");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-1];
        command_array_P[i++].function = &scpi_get_IIR_value;
		
        command_array_P[i].name       = PSTR("LASERPOWER?");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-2];
        command_array_P[i++].function = &scpi_get_laserpower_q;

	command_array_P[i].name       = PSTR("CLRI2C");
	command_array_P[i].implied    = false;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &clr_i2c;
        
	command_array_P[i].name       = PSTR("WATER");
	command_array_P[i].implied    = false;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &scpi_null_func;
    
        command_array_P[i].name       = PSTR("ON");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-1];
        command_array_P[i++].function = &scpi_water_on;
        
        command_array_P[i].name       = PSTR("OFF");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-2];
        command_array_P[i++].function = &scpi_water_off;
		
        command_array_P[i].name       = PSTR("AUTO");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-3];
        command_array_P[i++].function = &scpi_water_auto;

	command_array_P[i].name       = PSTR("STORE");
	command_array_P[i].implied    = false;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &scpi_null_func;
		
		command_array_P[i].name       = PSTR("IIR");
		command_array_P[i].implied    = false;
		command_array_P[i].parent     = &command_array_P[i-1];
		command_array_P[i++].function = &scpi_set_IIR_value;
		
		command_array_P[i].name       = PSTR("LASERPOWER");
		command_array_P[i].implied    = false;
		command_array_P[i].parent     = &command_array_P[i-2];
		command_array_P[i++].function = &scpi_set_laserpower;

	return i; // This is incremented so it matches "COMMAND_ARRAY_SIZE"
}
/**************************************************************************
*   NULL Function called when command is not recognized.                  *
***************************************************************************/
void scpi_null_func( char *arg, IO_pointers_t IO )
{
	// scpi_prStr_P(PSTR("Null func runs\r\n"), IO.USB_stream);
}
/**************************************************************************
*  *OPC (Operation Complete Query) function                               *
***************************************************************************/
void st_OPC_q ( char *arg, IO_pointers_t IO )
{
	scpi_prStr_P(PSTR("1\r\n"), IO.USB_stream);
}
/**************************************************************************
*  Empty/placeholder function                                             *
***************************************************************************/
void scpi_empty_func( char *arg, IO_pointers_t IO ) {}
/**************************************************************************
*  Write to the bootloader start address                                  *
***************************************************************************/
void sys_rst_btloader( char *arg, IO_pointers_t IO )
{
	Jump_To_Bootloader();
}
/**************************************************************************
*  :SYSTem:ERRor?                                                         *
***************************************************************************/
void sys_error_q( char *arg, IO_pointers_t IO )
{
	if (error_number == 0)
		scpi_prStr_P(PSTR("+0,\"No error\"\r\n"), IO.USB_stream);
	else
	{
		scpi_prStr_P(error_messages[error_number], IO.USB_stream);
		if (bad_command[0] != NUL)
		{
			fprintf(IO.USB_stream, "%s\r\n", bad_command);
			bad_command[0] = NUL;
		}	
		error_number--;
	}
}
/**************************************************************************
*  VERSION? function                                                      *
***************************************************************************/
void scpi_get_version_q( char *arg, IO_pointers_t IO )
{
	scpi_prStr_P(PSTR(FIRMWARE_VERSION),IO.USB_stream);
	scpi_prStr_P(PSTR("\r\n"), IO.USB_stream);
}
/**************************************************************************
*  *IDN? function                                                         *
***************************************************************************/
void scpi_IDN_q( char *arg, IO_pointers_t IO )
{
	scpi_prStr_P(PSTR(COMPANY_NAME), IO.USB_stream);
    scpi_prStr_P(PSTR(" | "), IO.USB_stream);
	scpi_prStr_P(PSTR(PROJECT_NAME), IO.USB_stream);
    scpi_prStr_P(PSTR(" | "), IO.USB_stream);
	scpi_prStr_P(PSTR("Firmware Revision: "), IO.USB_stream);
    scpi_prStr_P(PSTR(FIRMWARE_VERSION), IO.USB_stream);
    scpi_prStr_P(PSTR("\r\n"), IO.USB_stream);
}
/**************************************************************************
*  *CLS (Clear Status) function                                           *
***************************************************************************/
void st_CLS( char *arg, IO_pointers_t IO ) {}
/**************************************************************************
*      On *RST call rt_open with "(@ALL)" to open all relays              *
***************************************************************************/
void st_RST( char *arg, IO_pointers_t IO ) {}
/**************************************************************************
*   *TST                                                                  *
***************************************************************************/
void st_TST( char *arg, IO_pointers_t IO ) {}
/**************************************************************************
*  *WAI (Wait To Complete) function                                       *
***************************************************************************/
void st_WAI( char *arg, IO_pointers_t IO ) {}
/**************************************************************************
*  Debug function                                                         *
***************************************************************************/
void debug(char *arg, IO_pointers_t IO)
{
    // scpi_prStr_P(PSTR("Entering Function!\r\n"), IO.USB_stream);
    // {
        // fprintf(IO.USB_stream, "VAL: %d\r\n", I2C16_Read_Byte ( VL6180X_ADDR7b, 0 ));
        // fprintf(IO.USB_stream, "TWCR Byte: 0x%x\r\n", TWCR);
    // }
    // scpi_prStr_P(PSTR("Exiting Function!\r\n"), IO.USB_stream);
    // fprintf(IO.USB_stream, "TCCR0B: %d\r\n", TCCR0B);
    // fprintf(IO.USB_stream, "CLKPR: %d\r\n", CLKPR);
    // fprintf(IO.USB_stream, "Measurement ready: %d\r\n", range__measurement_ready());
    // scpi_prStr_P(PSTR("Starting measurement\r\n"), IO.USB_stream);
    // start_range_measurement();
    // fprintf(IO.USB_stream, "Measurement ready now: %d\r\n", range__measurement_ready());
    // fprintf(IO.USB_stream, "Ready bit: %d\r\n", range__range_device_ready());
    // scpi_prStr_P(PSTR("Waiting 50ms\r\n"), IO.USB_stream);
    // Delay_MS(50);
    // fprintf(IO.USB_stream, "Measurement ready: %d\r\n", range__measurement_ready());
    // fprintf(IO.USB_stream, "Ready bit: %d\r\n", range__range_device_ready());
    // fprintf(IO.USB_stream, "Reading: %d\r\n", get_range());

    // fprintf(IO.USB_stream, "Max Reading: %d\r\n", (int)(max_reading/8192));
    // fprintf(IO.USB_stream, "IIR Reading: %d\r\n", (int)(IIR_range_reading/8192));
}
/**************************************************************************
*  SCPI Get Range Reading                                                 *
***************************************************************************/
// void scpi_get_range_q(char *arg, IO_pointers_t IO)
// {
    // if (range_measurement_ready())  // There's a reading pending
    // {;}                             // For now do nothing
    // else if (range_sensor_busy())   // There's none pending and it's busy
    // {
        // do{;}
        // while(range_sensor_busy()); // just wait it out
    // }
    // else                            // There's none pending and it's not busy
    // {   start_range_measurement();  // Kick off a new reading and
        // do{;}                       // just
        // while(range_sensor_busy()); // wait it out
    // }
                                    // Always
	// fprintf(IO.USB_stream, "%d\r\n", get_range());
    // start_range_measurement();
	
	// if (range_measurement_ready())
		// fprintf(IO.USB_stream, "%d\r\n", get_range());
	// else
	// {
		// while(range_sensor_busy()) {}
		// fprintf(IO.USB_stream, "%d\r\n", get_range());
	// }
	
	// fprintf(IO.USB_stream, "%d\r\n", get_range());
// }
/**************************************************************************
*  Clear Port                                                             *
***************************************************************************/
void clr_i2c (char *arg, IO_pointers_t IO)
{
    reset_i2c();
}
/**************************************************************************
*  SCPI Water Auto                                                        *
***************************************************************************/
void scpi_water_auto (char *arg, IO_pointers_t IO)
{
    water_auto = true;
	water_on(false);
}
/**************************************************************************
*  SCPI Water On                                                          *
***************************************************************************/
void scpi_water_on (char *arg, IO_pointers_t IO)
{
    water_auto = false;
	water_on(true);
}
/**************************************************************************
*  SCPI Water Off                                                         *
***************************************************************************/
void scpi_water_off (char *arg, IO_pointers_t IO)
{
    water_auto = false;
	water_on(false);
}
/**************************************************************************
*  my_remove_ws                                                           *
***************************************************************************/
void remove_ws( char *arg )
{
	int	ri, wi = 0;                                     //read index, write index
	for ( ri = 0 ; ri <= strlen(arg) ; ri++ )           // Walk string looking for whitespaces, <= preserves NUL char
		if ( !isspace(arg[ri]) ) arg[wi++] = arg[ri];   // Strip off leading and any additional whitespaces
}
/**************************************************************************
*  Store Laser Power to EEPROM                                            *
***************************************************************************/
void scpi_set_laserpower( char *arg, IO_pointers_t IO )
{
	if (strlen(arg) <= MAX_ARG_LEN) remove_ws(arg);
    if (strlen(arg) <= MAX_LASER_CHARS)
	{
		eeprom_busy_wait();
		eeprom_write_block(arg, &LASER_POWER, MAX_LASER_CHARS);
		set_laserpower(retrieve_laserpower_setting());
	}
	else
		scpi_add_error_P(error_arg_too_long, IO);
}
/**************************************************************************
*  Update Laser Power from EEPROM                                         *
***************************************************************************/
uint8_t retrieve_laserpower_setting()
{
	char laserpower_string[MAX_LASER_CHARS];
    eeprom_busy_wait();
    eeprom_read_block((void*)&laserpower_string, (const void *)&LASER_POWER, MAX_LASER_CHARS);
	return (uint8_t)atoi(laserpower_string);
}
/**************************************************************************
*  SCPI Get Laser Power Setting                                           *
***************************************************************************/
void scpi_get_laserpower_q( char *arg, IO_pointers_t IO )
{
	fprintf(IO.USB_stream, "%d\r\n", get_laserpower());
}
/**************************************************************************
*  Store IIR Factor to EEPROM                                             *
***************************************************************************/
void scpi_set_IIR_value( char *arg, IO_pointers_t IO )
{
	if (strlen(arg) <= MAX_ARG_LEN) remove_ws(arg);
    if (strlen(arg) <= MAX_IIR_CHARS && strlen(arg) >= 1)
	{
		eeprom_busy_wait();
		eeprom_write_block(arg, &IIR_VALUE, MAX_IIR_CHARS);
		iir_value = retrieve_IIR_value();
	}
	else
		scpi_add_error_P(error_arg_too_long, IO);
}
/**************************************************************************
*  SCPI Retreive IIR Factor from EEPROM                                   *
***************************************************************************/
void scpi_get_IIR_value( char *arg, IO_pointers_t IO )
{
	fprintf(IO.USB_stream, "%d\r\n", retrieve_IIR_value());
}
/**************************************************************************
*  Update IIR Factor from EEPROM                                          *
***************************************************************************/
uint8_t retrieve_IIR_value()
{
	char iir_string[MAX_IIR_CHARS];
    eeprom_busy_wait();
    eeprom_read_block((void*)&iir_string, (const void *)&IIR_VALUE, MAX_IIR_CHARS);
    return (uint8_t)atoi(iir_string);
}