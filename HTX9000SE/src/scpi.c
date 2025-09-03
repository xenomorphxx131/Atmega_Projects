#include "scpi.h"

/**************************************************************************
*  Error Message Buffers have scope within this file                      *
***************************************************************************/
int error_number = 0;
char bad_command[MAX_TOKEN_LEN + 1] = "";
PGM_P error_messages[ERROR_QUEUE_LEN + 1];
char const error_message1[]	PROGMEM = "-112,Program mnemonic too long";
char const error_message2[]	PROGMEM = "-112,Argument too long";
char const error_message3[] PROGMEM = "-113,Bad path or header: ";


int SetupScpiCommandsArray_P ( scpi_commands_P_t command_array_P[COMMAND_ARRAY_SIZE], int i )
{
	// 1 Top NULL node
	command_array_P[i].name      = PSTR("NULL");
	command_array_P[i].implied    = YES;
	command_array_P[i].parent     = NULL;
	command_array_P[i++].function   = &scpi_null_func;
	// 2 [:]*CLS
	// 3 [:]*ESE
	// 4 [:]*ESE?
	// 5 [:]*ESR?
	// 6 [:]*OPC
	// 2 [:]*OPC?
	command_array_P[i].name      = PSTR("*OPC?");
	command_array_P[i].implied    = NO;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &st_OPC_q;
	// 8 [:]*RST
	// 9 [:]*SRE
	// 10 [:]*SRE?
	// 11 [:]*STB?
	// 12 [:]*TST?
	// 13 [:]*WAI

/**************************************************************************
*  Non-Compulsory SCPI commands                                           *
***************************************************************************/

	// 3 [[:]SYSTem]
	command_array_P[i].name      = PSTR("SYSTem");
	command_array_P[i].implied    = YES;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &scpi_empty_func; // ****** Careful here, Loadbox_scpi.c refers to this directly as "&command_array_P[2]"

	// 15[[:]SYSTem:]PRESet

		// 4 [[:]SYSTem:]:RST
		command_array_P[i].name      = PSTR("RST");
		command_array_P[i].implied    = NO;
		command_array_P[i].parent     = &command_array_P[i-1];
		command_array_P[i++].function = &scpi_empty_func;

			// 5 [[:]SYSTem:]:RST:BTLOader
			command_array_P[i].name      = PSTR("BTLOader");
			command_array_P[i].implied    = NO;
			command_array_P[i].parent     = &command_array_P[i-1];
			command_array_P[i++].function = &sys_rst_btloader;

		// 6 [[:]SYSTem:]ERRor?
		command_array_P[i].name      = PSTR("ERRor?");
		command_array_P[i].implied    = NO;
		command_array_P[i].parent     = &command_array_P[i-3];
		command_array_P[i++].function = &sys_error_q;
	// 19 [[:]SYSTem:]ERRor
	// 20 [[:]SYSTem:]ERRor:NEXT?
	// 7 [[:]SYSTem:][:]VERSion?
		command_array_P[i].name      = PSTR("VERSion?");
		command_array_P[i].implied    = NO;
		command_array_P[i].parent     = &command_array_P[i-4]; // 
		command_array_P[i++].function = &scpi_ver;
	// 22 [[:]STATus]
	// 23 [[:]STATus:]OPERation
	// 24 [[:]STATus:]OPERation:CONDition?
	// 25 [[:]STATus:]OPERation:ENABle
	// 26 [[:]STATus:]OPERation:ENABle?
	// 27 [[:]STATus:]QUEStionable
	// 28 [[:]STATus:]QUEStionable:CONDition?
	// 29 [[:]STATus:]QUEStionable:ENABle
	// 30 [[:]STATus:]QUEStionable:ENABle?
	// 31 [:]PRESet
   
  return i; // Passes the number of entries in this section to the product specific section.
}
/**************************************************************************
*  Build input string from terminal then run SCPI command.                *
***************************************************************************/
void scpi_process_input( char * str_in, int *str_len, scpi_commands_P_t cmd_array_P[COMMAND_ARRAY_SIZE], IO_pointers_t IO )
{
	int16_t		ReceivedByte;
	uint8_t		usb_data;										// Temporary SUB data storage
	
	ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
//	if (!(ReceivedByte < 0))
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
				scpi_prStr_P(PSTR("ERROR command too long\n"),IO.USB_stream);
		}
		}
	ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
	}
}
/**************************************************************************
*  Parser Get Short Name                                                  *
***************************************************************************/
void scpi_get_short_name(char *lname, char *sname) {
 	sname[0] = NUL;
	int lindex = 0;
	int sindex = 0;
	while (lname[lindex] != NUL)								// Walk the name string
	{
		if (lname[lindex] == toupper(lname[lindex]))			// Check if the character is uppercase
		{
			sname[sindex] = lname[lindex];						// build up the sname
			sindex++;
		}
		else
			lname[lindex] = toupper(lname[lindex]);
		lindex++;
	}
	sname[sindex] = NUL;										// Terminate the sname string
}
/**************************************************************************
*  Parser Find Implied                                                    *
***************************************************************************/
bool scpi_find_implied(scpi_commands_P_t **current_state, char *token, scpi_commands_P_t cmd_array_P[COMMAND_ARRAY_SIZE]) {
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
				} else if(parent_ptr->implied == YES) {
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
*  Parse and process input string from terminal then run SCPI command.    *
***************************************************************************/
void scpi_process_cmd_P ( char * input_string, scpi_commands_P_t cmd_array_P[COMMAND_ARRAY_SIZE], IO_pointers_t IO )
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
 		valid_command = NO;
		if (argument[0] == ' ' ) // or other chars
		{	argument[0] = NUL;											// Terminates last token
			argument++;													// Move argument pointer past space
		}
		if (strlen(token) > MAX_TOKEN_LEN)								// Check for token length error
		{	scpi_add_error_P(error_message1, IO);						// Document the error
			break;														// Bail from the token search loop
		}
		if (strlen(argument) > MAX_ARG_LEN)								// Check for argument length error
		{	scpi_add_error_P(error_message2, IO);						// Document the error
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
			// if no match or implied match was found there was an error
			scpi_add_error_P(error_message3, IO);						// Indicate bad token
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
*  Write to the bootloader start address                                  *
***************************************************************************/
void sys_rst_btloader ( char *arg, IO_pointers_t IO )
{
	scpi_prStr_P(PSTR("Bootloader Running..."), IO.USB_stream);
	process_USB();
	Delay_MS(500);
	Jump_To_Bootloader();
}
/**************************************************************************
*  :SYSTem:ERRor?                                                         *
***************************************************************************/
void sys_error_q ( char *arg, IO_pointers_t IO )
{
	if (error_number == 0)
		scpi_prStr_P(PSTR("+0,\"No error\"\n"), IO.USB_stream);
	else
	{
		scpi_prStr_P(error_messages[error_number], IO.USB_stream);
		if (bad_command[0] != NUL)
		{
			fprintf(IO.USB_stream, "%s\n", bad_command);
			bad_command[0] = NUL;
		}	
		error_number--;
	}
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
*  Empty/placeholder function                                             *
***************************************************************************/
void scpi_empty_func ( char *arg, IO_pointers_t IO )
{} // END scpi_empty_func()
/**************************************************************************
*   NULL Function called when command is not recognized.                  *
***************************************************************************/
void scpi_null_func ( char *arg, IO_pointers_t IO )
{
	// scpi_prStr_P(PSTR("Null func runs\n"), IO.USB_stream);
}
/**************************************************************************
*  *CLS (Clear Status) function                                           *
***************************************************************************/
void st_CLS ( char *arg, IO_pointers_t IO )
{}// END st_CLS()
/**************************************************************************
*  *OPC (Operation Complete) function                                     *
***************************************************************************/
void st_OPC ( char *arg, IO_pointers_t IO )
{} // END st_OPC()
/**************************************************************************
*  *OPC (Operation Complete Query) function                               *
***************************************************************************/
void st_OPC_q ( char *arg, IO_pointers_t IO )
{
	fprintf(IO.USB_stream, "1\n");
}
/**************************************************************************
*      On *RST call rt_open with "(@ALL)" to open all relays              *
***************************************************************************/
void st_RST ( char *arg, IO_pointers_t IO )
{} // END st_RST()
/**************************************************************************
*   *TST                                                                  *
***************************************************************************/
void st_TST ( char *arg, IO_pointers_t IO )
{} // END st_TST()
/**************************************************************************
*  *WAI (Wait To Complete) function                                       *
***************************************************************************/
void st_WAI ( char *arg, IO_pointers_t IO )
{} // END st_wai()
/**************************************************************************
*  VERSION? function                                                      *
***************************************************************************/
void scpi_ver ( char *arg, IO_pointers_t IO )
{
	scpi_prStr_P(PSTR(FIRMWARE_VERSION),IO.USB_stream);
	scpi_prStr_P(PSTR("\n"), IO.USB_stream);
} // END scpi_ver()
/**************************************************************************
*  Prints a program memory string one char at a time to the terminal      *
***************************************************************************/
void scpi_prStr_P ( PGM_P arg, FILE *fstream )
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
void PGM_P_to_string ( PGM_P arg, char * name, FILE *fstream )
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
*  Parse and process input string from terminal then run SCPI command.    *
***************************************************************************/
void scpi_process_cmd_P_OLD ( char * input_string, scpi_commands_P_t cmd_array_P[COMMAND_ARRAY_SIZE], IO_pointers_t IO )
{
	char * token;
	char * argument = "";
	char lname[MAX_TOKEN_LEN] = "";
	char sname[MAX_TOKEN_LEN] = "";
	int cmd_i = 0;														// Command index for walking dictionary
	int lindex = 0;														// Temporary variables used to parse
	int sindex = 0;														// name into long and short
	scpi_commands_P_t* current_state = &cmd_array_P[0];					// Pointer to commands in command array. Initialize to the top (no command).
	scpi_commands_P_t* current_ptr;										// Pointer to commands in command array used for tokens.
	scpi_commands_P_t* parent_ptr;										// Pointer to commands in command array used for parents.
	uint8_t valid_command_path = NO;									// Valid path variable
	uint8_t valid_command = NO;											// Valid command variable
	int has_implied_parent = NO;										// Implied parent variable

	token = strtok (input_string,":");									// Scan string looking for : separators
	while (token != NUL)
	{//--------------------------------------------------------------->	// Search for new tokens
 		argument = strpbrk(token, " ");									// Check for space indicating an argument
		if (argument[0] == ' ' ) // or other chars
		{	argument[0] = NUL;											// Terminates last token
			argument++;													// Move argument pointer past space
		}
		if (strlen(token) > MAX_TOKEN_LEN)								// Check for token length error
		{	scpi_add_error_P(error_message1, IO);						// Document the error
			break;														// Bail from the token search loop
		}
		if (strlen(argument) > MAX_ARG_LEN)								// Check for argument length error
		{	scpi_add_error_P(error_message2, IO);						// Document the error
			break;														// Bail from the token search loop
		}
		valid_command_path = NO;										// Initialize valid path to NO
		valid_command = NO;												// Initialize valid token to NO
		
		for(cmd_i = 0; cmd_i < COMMAND_ARRAY_SIZE; cmd_i++)				// Walk through known commands looking for a match
		{// ---------------------------------------------------------->	// Beginning of the PRIMARY array scan
			current_ptr = &cmd_array_P[cmd_i];							// Get pointer to found command		
			parent_ptr = current_ptr -> parent;							// Get pointer of parent of current_ptr
			has_implied_parent =
				//pgm_read_byte(parent_ptr->implied) ==  'Y';				// Figure out if the parent is implied
				parent_ptr->implied ==  YES;							// Figure out if the parent is implied

			PGM_P_to_string(cmd_array_P[cmd_i].name, lname, IO.USB_stream);
			sname[0] = NUL;
			lindex = 0;
			sindex = 0;
			while (lname[lindex] != NUL)								// Walk the name string
			{
				if (lname[lindex] == toupper(lname[lindex]))			// Check if the character is uppercase
				{
					sname[sindex] = lname[lindex];						// build up the sname
					sindex++;
				}
				else
					lname[lindex] = toupper(lname[lindex]);
				lindex++;
			}
			sname[sindex] = NUL;										// Terminate the sname string

			valid_command =												// The current command is valid if
				strcmp(lname, token) == 0	||							// It matches long name.
				strcmp(sname, token) == 0;								// OR if it matches short name.
			valid_command_path =										// The path is valid if
				cmd_array_P[cmd_i].parent == current_state;				// its parent if the current state
			if (valid_command && valid_command_path)					// If the command and its path are good
			{	current_state = &cmd_array_P[cmd_i];					// Update current_state to new found token
				break;													// Bail from the array scan loop
			}
		}// ---------------------------------------------------------->	// End of the PRIMARY array search
		
		if (!(valid_command_path && valid_command))						// Try again looking for implied
			for(cmd_i = 0; cmd_i < COMMAND_ARRAY_SIZE; cmd_i++)			// Walk through known commands looking for a match
			{// ------------------------------------------------------>	// Beginning of the SECONDARY array scan
				current_ptr = &cmd_array_P[cmd_i];						// Get pointer to found command		
				parent_ptr = current_ptr -> parent;						// Get pointer of parent of current_ptr
				has_implied_parent =
					//pgm_read_byte(parent_ptr->implied) ==  'Y';		// Figure out if the parent is implied
					parent_ptr->implied ==  YES;						// Figure out if the parent is implied

				PGM_P_to_string(cmd_array_P[cmd_i].name, lname, IO.USB_stream);
				sname[0] = NUL;
				lindex = 0;
				sindex = 0;
				while (lname[lindex] != NUL)							// Walk the name string
				{
					if (lname[lindex] == toupper(lname[lindex]))		// Check if the character is uppercase
					{
						sname[sindex] = lname[lindex];					// build up the sname
						sindex++;
					}
					else
						lname[lindex] = toupper(lname[lindex]);
					lindex++;
				}
				sname[sindex] = NUL;									// Terminate the sname string
						
				valid_command =											// The current command is valid if
					strcmp(lname, token) == 0	||						// It matches long name.
					strcmp(sname, token) == 0;							// OR if it matches short name.
				valid_command_path =									// The path is valid if
					cmd_array_P[cmd_i].parent == current_state ||		// its parent is correct
					has_implied_parent;									// OR of the parent is implied - this breaks reuse of commands
				if (valid_command && valid_command_path)				// If the command and its path are good
				{	current_state = &cmd_array_P[cmd_i];				// Update current_state to new found token
					break;												// Bail from the array scan loop
				}
			}// ------------------------------------------------------>	// End of the SECONDARY array search

		if (!(valid_command_path && valid_command))						// If both aren't correct here,
			break;														// Then there's been an error - bail.
		token = strtok (NULL, ":");										// Otherwise get the next : separated token
	}//--------------------------------------------------------------->	// End of new token search
	if (valid_command && valid_command_path)
			current_state->function(argument, IO);						// Run the handler function of the tail
	else if (!valid_command)											// If here must have been invalid
	{	scpi_add_error_P(error_message3, IO);							// Indicate bad token
		strncpy(bad_command, token, MAX_TOKEN_LEN);						// Send up to MAX_TOKEN_LEN of bad token
	}
}


