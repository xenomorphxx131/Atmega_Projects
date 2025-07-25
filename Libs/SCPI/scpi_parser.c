/****************************************************************************
 *                                                                          *
 *  SCPI Parser                                                             *
 *                                                                          *
 ****************************************************************************/

#include "scpi_parser.h"

char bad_command[MAX_TOKEN_LEN + 1] = "";
PGM_P error_messages[ERROR_QUEUE_LEN + 1];
int error_number = 0;
char const error_mnemonic_too_long[]    PROGMEM = "-112,Program mnemonic too long";
char const error_arg_too_long[]         PROGMEM = "-112,Argument too long";
char const error_bad_path_or_header[]   PROGMEM = "-113,Bad path or header: ";
/****************************************************************************
*  Build input string from terminal then run SCPI command.                  *
*****************************************************************************/
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
/****************************************************************************
*  Parse and process input string from terminal then run SCPI command.      *
*****************************************************************************/
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
				scpi_get_short_name(lname, sname);
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
/****************************************************************************
*  Parser Find Implied                                                      *
*****************************************************************************/
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
/****************************************************************************
*  Parser Get Short Name                                                    *
*****************************************************************************/
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
/****************************************************************************
*  Adds error to error queue                                                *
*****************************************************************************/
void scpi_add_error_P(PGM_P error_message, IO_pointers_t IO)
{
	if (error_number < ERROR_QUEUE_LEN)
		error_messages[++error_number] = error_message;
}
/****************************************************************************
*  Prints a program memory string one char at a time to the terminal        *
*****************************************************************************/
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
/****************************************************************************
*  Prints a program memory string one char at a time to the terminal        *
*****************************************************************************/
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
/****************************************************************************
*   NULL Function called when command is not recognized or not compplete.   *
*****************************************************************************/
void scpi_null_func( char *arg, IO_pointers_t IO ) {}
/****************************************************************************
*  my_remove_ws                                                             *
*****************************************************************************/
void remove_ws( char *arg )
{
	int	ri, wi = 0;                                     //read index, write index
	for ( ri = 0 ; ri <= strlen(arg) ; ri++ )           // Walk string looking for whitespaces, <= preserves NUL char
		if ( !isspace(arg[ri]) ) arg[wi++] = arg[ri];   // Strip off leading and any additional whitespaces
}