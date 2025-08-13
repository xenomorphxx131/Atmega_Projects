/****************************************************************************
 *                                                                          *
 *  SCPI Parser                                                             *
 *                                                                          *
 ****************************************************************************/

#include "scpi_parser.h"

char bad_command[MAX_TOKEN_LEN + 1] = "";
PGM_P error_messages[ERROR_QUEUE_LEN + 1];
int error_number = 0;
char const MNEMONIC_TOO_LONG[]      PROGMEM = "-112,Program mnemonic too long";
char const ARG_TOO_LONG[]           PROGMEM = "-112,Argument too long";
char const BAD_PATH_OR_HEADER[]     PROGMEM = "-113,Bad path or header: ";
char const TOO_MANY_PARAMETERS[]    PROGMEM = "-108,Too many parameters: ";
char const TOO_FEW_PARAMETERS[]     PROGMEM = "-108,Too few parameters";
/****************************************************************************
*  Build input string from terminal then run SCPI command.                  *
*****************************************************************************/
void process_scpi_input(scpi_commands_P_t cmd_array_P[], IO_pointers_t IO )
{
    int16_t ReceivedByte;
    uint8_t usb_data;                                                               // Temporary USB data storage
    static int  string_index = 0;                                                   // Running string index
    static char input_string[MAX_IN_STR_LEN+1] = "";                                // Running incoming string

    ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
    while (ReceivedByte >= 0)                                                       // Returns -1 if empty, else 0-255
    {
        usb_data = toupper((char)ReceivedByte);                                     // Get all chars and convert to upper case.
        if ((usb_data == ';') || (usb_data == '\r') || (usb_data == '\n'))          // Check for one of three SCPI terminations
        {
            if (string_index > 0)
                scpi_process_cmd_P(input_string, cmd_array_P, IO);                  // Process
            input_string[0] = NUL;                                                  // Empty input_string by placing the termination character in the first position
            string_index = 0;                                                       // Reset the string_index
        }
        else
        {
            if (( usb_data == '\b' || usb_data == DEL_KEY ) && string_index > 0)    // If backspace or DELETE key (ASC127) and the length is non-zero
                input_string[--string_index] = NUL;                                       // Decrement string_index, Move termination character back the first position
            else                                                                    // If not backspace and...
            {
                if (string_index < MAX_IN_STR_LEN - 1)                              // as long as the length is still OK...
                {
                    input_string[string_index] = usb_data;                          // pile on whatever is being sent to current input_string and increment string_index
                    input_string[++string_index] = NUL;                             // and terminate the string with the NUL character
                }
                else
                    // scpi_prStr_P(PSTR("ERROR command too long\r\n"), IO.USB_stream);
                    scpi_prStr_P(PSTR("ERROR command too long\r\n"), IO);
            }
        }
        ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
    }
}
/****************************************************************************
*  Parse and process input string from com port then run SCPI command.      *
*****************************************************************************/
void scpi_process_cmd_P( char* input_string, scpi_commands_P_t cmd_array_P[], IO_pointers_t IO )
{
    char *token;
    char *argument = "";
    char *long_name;
    char *short_name;
    int cmd_i = 0;                                                              // Command index for walking dictionary
    scpi_commands_P_t *current_command = NULL;                                  // Pointer to commands in command array. Initialize to the top (no command).
    scpi_commands_P_t *next_command;                                            // Pointer to commands in command array used for tokens.
    bool valid_command = false;                                                 // Valid command variable
    /****************************************************************************
     * Get the argument if there is one                                         *
     ****************************************************************************/
    argument = strpbrk(input_string, " ");                                      // Check for a space indicating an argument
    if (argument != NULL)                                                       // Return value will be NULL if no spaces
    {
        *argument = NUL;                                                        // Moves termination of the original string to the where the space used to be
        argument++;                                                             // Move argument pointer one past the space
        if (strlen(argument) > MAX_ARG_LEN)                                     // Check for argument length error
        {                                                                       // If too long...
            scpi_add_error_arg_too_long(IO);                                    // document the error and...
            return;                                                             // bail from the whole SCPI input
        }                                                                       // TODO: Allow for strings delimited by ""
        if (strpbrk(argument, " ") != NULL)                                     // If any more spaces...
        {                                                                       // Declare the error,
            scpi_add_error_P(TOO_MANY_PARAMETERS, IO);                          // and...
            strncpy(bad_command, argument, MAX_TOKEN_LEN);                      // display the malfored argument.
            return;                                                             // Bail from the whole SCPI input
        }
    }
    /****************************************************************************
     * Parse the command tokens                                                 *
     ****************************************************************************/
    token = strtok(input_string, ":");                                          // Scan string looking for ":" separators
    while (token != NULL)                                                       // until we run out of tokens
    {                                                                           // Search for new tokens
        valid_command = false;
        /************************************************************************
         * Check the command length                                             *
         ************************************************************************/
        if (strlen(token) > MAX_TOKEN_LEN)
        {
            scpi_add_error_P(MNEMONIC_TOO_LONG, IO);
            return;
        }
        /************************************************************************
         * Walk the command list in pairs keeping a continuous link to the past *
         ************************************************************************/
        for(cmd_i = 0; cmd_i < COMMAND_ARRAY_SIZE; cmd_i++)                     // Walk through known commands looking for a match.
        {
            next_command = &cmd_array_P[cmd_i];                                 // Get a pointer to next_command.
            if (next_command->parent == current_command)                        // Only proceed if the next_command's parent is the current_command.
            {
                long_name = PGM_P_to_string(cmd_array_P[cmd_i].name);           // Get the long_name
                short_name = scpi_get_short_name(long_name);                    // and the short_name
                if (!strcmp(long_name, token) || !strcmp(short_name, token))    // If the command matches either long_name or short_name...
                {                                                               //
                    current_command = next_command;                             // Make the current_command the next_command for the next iteration.
                    valid_command = true;                                       // Mark it as valid up until now.
                    break;                                                      // Stop walking the command list once a valid command is found.
                }
            }
        }
        if (!valid_command)                                                     // If no match check for implied match.
        {
            if (!scpi_find_implied(&current_command, token, cmd_array_P))       // If an implied command wasn't found, there was a real error. This function updates current_command.
            {
                scpi_add_error_P(BAD_PATH_OR_HEADER, IO);                       // Indicate bad token
                strncpy(bad_command, token, MAX_TOKEN_LEN);                     // Record the bad token string for debug output
                return;                                                         // Bail out
            }
            else
                valid_command = true;                                           // Not really needed, will run the function if it didn't bail during implied check.
        }
        token = strtok(NULL, ":");                                              // Otherwise get the next ":" separated token
    }                                                                           // End of new token search
    // if (valid_command)                                                       // Not needed. If it got this far, it's runnable.
        current_command->function(argument, IO);                                // If it's valid or it didn't bail at scpi_find_implied(), it should be a runnable head.
}
/****************************************************************************
*  Parser Find Implied                                                      *
*****************************************************************************/
bool scpi_find_implied(scpi_commands_P_t **current_command, char *token, scpi_commands_P_t cmd_array_P[])
{
    scpi_commands_P_t *current_ptr;
    scpi_commands_P_t *parent_ptr;
    char *long_name;
    char *short_name;

    for(int cmd_i = 0; cmd_i < COMMAND_ARRAY_SIZE; cmd_i++)             // Walk through known commands looking for a match
    {
        current_ptr = &cmd_array_P[cmd_i];                              // Get pointer to found command
        long_name = PGM_P_to_string(current_ptr->name);
        short_name = scpi_get_short_name(long_name);
        if (!strcmp(long_name, token) || !strcmp(short_name, token))    // If the command matches long name or short name
        {
            while(true)
            {
                parent_ptr = current_ptr->parent;
                if (parent_ptr == *current_command)
                {
                    *current_command = &cmd_array_P[cmd_i];
                    return true;
                }
                else if (parent_ptr == NULL)
                    break;
                else if(parent_ptr->implied == true)
                    current_ptr = parent_ptr;
                else
                    break;
            }
        }
    }
    return false;
}
/****************************************************************************
*  Parser Get Short Name                                                    *
*****************************************************************************/
char *scpi_get_short_name(char *long_name )
{
    uint16_t long_index=0, short_index = 0;
    static char short_name[MAX_TOKEN_LEN+1] = "";

    short_name[0] = NUL;
    while (long_name[long_index] != NUL)                                // Walk the name string
    {   
        if (long_name[long_index] == toupper(long_name[long_index]))    // Check if the character is uppercase
        {   
            short_name[short_index] = long_name[long_index];            // build up the short_name
            short_index++;   
        }   
        else
            long_name[long_index] = toupper(long_name[long_index]); 
        long_index++;
    }
    short_name[short_index] = NUL;                                      // Terminate the short_name string
    return short_name;
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
*  Adds arg too long error to queue                                         *
*****************************************************************************/
void scpi_add_error_arg_too_long(IO_pointers_t IO)
{
    scpi_add_error_P(ARG_TOO_LONG, IO);
}
/****************************************************************************
*  Adds too few parameters error to queue                                   *
*****************************************************************************/
void scpi_add_error_too_few_parameters(IO_pointers_t IO)
{
    scpi_add_error_P(TOO_FEW_PARAMETERS, IO);
}
/****************************************************************************
*  Prints a program memory string one char at a time to the terminal        *
*****************************************************************************/
void scpi_prStr_P( PGM_P arg, IO_pointers_t IO )
{
    uint16_t string_index=0;
    char the_char = pgm_read_byte(arg);

    while (the_char != NUL)
    {
        fputc(the_char, IO.USB_stream);
        the_char = pgm_read_byte(&arg[++string_index]);
    }
}
/****************************************************************************
*  Prints a program memory string one char at a time to the terminal        *
*****************************************************************************/
char *PGM_P_to_string( PGM_P arg )
{
    static char name[MAX_TOKEN_LEN+1] = "";
    uint16_t i=0;

    name[i] = pgm_read_byte(arg);
    while (name[i] != NUL)
    {
        i++;
        name[i] = pgm_read_byte(&arg[i]);
    }
    i++;
    name[i] = NUL;
    return name;
}
/****************************************************************************
*   NULL Function called when command is not recognized or not compplete.   *
*****************************************************************************/
void scpi_null_func( char *arg, IO_pointers_t IO ) {}
/****************************************************************************
*  my_remove_ws                                                             *
*****************************************************************************/
// void remove_ws( char *arg )
// {
    // uint16_t read_index, write_index = 0;                                       //read index, write index

    // for ( read_index=0; read_index <= strlen(arg); read_index++ )               // Walk string looking for whitespaces, <= preserves NUL char
        // if ( !isspace(arg[read_index]) ) arg[write_index++] = arg[read_index];  // Strip off leading and any additional whitespaces
// }