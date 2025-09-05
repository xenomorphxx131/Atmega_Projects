/****************************************************************************
 *                                                                          *
 *  SCPI Parser                                                             *
 *                                                                          *
 ****************************************************************************/

#include "scpi_parser.h"

uint16_t error_count = 0;
SCPI_Error_t error_log[ERROR_QUEUE_LENGTH];
char const ARG_TOO_LONG[]        PROGMEM = "-112,Argument too long: ";
char const BAD_PATH_OR_HEADER[]  PROGMEM = "-113,Bad path or header: ";
char const TOO_MANY_PARAMETERS[] PROGMEM = "-108,Too many parameters: ";
char const TOO_FEW_PARAMETERS[]  PROGMEM = "-108,Too few parameters";
/****************************************************************************
*  Build input string from terminal then run SCPI command.                  *
*****************************************************************************/
void process_scpi_input(SCPI_Node_t **scpi_nodes, IO_pointers_t IO)
{
    int16_t ReceivedByte;
    char usb_data;                                                               // Temporary USB data storage
    static uint16_t string_index = 0;                                                    // Running string index
    static char input_string[MAX_IN_STR_LEN+1] = "";                                // Running incoming string

    ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
    while (ReceivedByte >= 0)                                                       // Returns -1 if empty, else the char (byte)
    {
        usb_data = toupper((char)ReceivedByte);                                     // Get all chars and convert to upper case.
        if ((usb_data == ';') || (usb_data == '\r') || (usb_data == '\n'))          // Check for one of three SCPI terminations
        {
            if (string_index > 0)
                scpi_process_cmd_P(input_string, scpi_nodes, IO);               // Process
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
                    scpi_prStr_P(PSTR("ERROR command too long\r\n"), IO.USB_stream);
            }
        }
        ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
    }
}
/****************************************************************************
*  Parse and process input string from com port then run SCPI command.      *
*****************************************************************************/
void scpi_process_cmd_P( char *input_string, SCPI_Node_t **scpi_nodes, IO_pointers_t IO )
{
    char token[MAX_TOKEN_LEN+1];
    char argument[MAX_ARG_LEN+1] = "";
    char long_name[MAX_TOKEN_LEN+1];
    char short_name[MAX_TOKEN_LEN+1];   
    SCPI_Node_t *current_command = NULL;                                        // Pointer to commands in command array. Initialize to the top (no command).
    SCPI_Node_t *next_command;                                                  // Pointer to commands in command array used for tokens.
    bool valid_command = false;                                                 // Valid command variable
    uint16_t command_index;
    char *space;
    char *separator;
    /****************************************************************************
     * Get the argument if there is one                                         *
     ****************************************************************************/
    space = strpbrk(input_string, " ");                                         // Check for a space indicating an argument
    if (space != NULL)                                                          // Return value will be NULL if no spaces
    {
        if (strlen(space + 1) > MAX_ARG_LEN)                                    // Check argument length
        {
            scpi_add_error_P(ARG_TOO_LONG, space+1, MAX_TOKEN_LEN);             // Log error and...
            return;                                                             // bail if too long
        }
        if (strpbrk(argument, " ") != NULL)                                     // If any more spaces...
        {                                                                       // Declare the error,
            scpi_add_error_P(TOO_MANY_PARAMETERS, argument, MAX_ARG_LEN);       // and...
            return;                                                             // bail from the whole SCPI input
        }
        strncpy(argument, space + 1, MAX_ARG_LEN);                              // Copy up to max arg length
        *space = NUL;                                                           // Put the termination character where the space was in the input string.
    }
    /****************************************************************************
     * Parse the command tokens                                                 *
     ****************************************************************************/
    separator = strtok(input_string, ":");                                      // Scan string looking for ":" separators
    while (separator != NULL)                                                   // until we run out of tokens
    {                                                                           // Search for new tokens
        valid_command = false;
        strncpy(token, separator, MAX_TOKEN_LEN);
        /************************************************************************
         * Walk the command list in pairs keeping a continuous link to the past *
         ************************************************************************/
        command_index = 0;
        while(scpi_nodes[command_index] != NULL)                                            // Walk through known commands looking for a match.
        {
            next_command = scpi_nodes[command_index];                                       // Get a pointer to next_command.
            if (next_command->parent == current_command)                                    // Only proceed if the next_command's parent is the current_command.
            {
                strncpy(long_name, scpi_nodes[command_index]->scpi_node, MAX_TOKEN_LEN);
                strncpy(short_name, scpi_get_short_name(long_name), MAX_TOKEN_LEN);
                if (!strcmp(long_name, token) || !strcmp(short_name, token))                // If the command matches either long_name or short_name...
                {                                                                           //
                    current_command = next_command;                                         // Make the current_command the next_command for the next iteration.
                    valid_command = true;                                                   // Mark it as valid up until now.
                    break;                                                                  // Stop walking the command list once a valid command is found.
                }
            }
            command_index++;
        }
        if (!valid_command)                                                     // If no match check for implied match.
        {
            if (!scpi_find_implied(&current_command, token, scpi_nodes))        // If an implied command wasn't found, there was a real error. This function updates current_command.
            {
                scpi_add_error_P(BAD_PATH_OR_HEADER, token, MAX_TOKEN_LEN);     // Indicate bad token
                return;                                                         // Bail out
            }
            else
                valid_command = true;
        }
        separator = strtok(NULL, ":");                                          // Get the next ":" separated token
        if (separator != NULL)
            strncpy(token, separator, MAX_TOKEN_LEN);
    }                                                                           // End of new token search
    if (valid_command && current_command->function != NULL)                     // Check if command is runnable.
        current_command->function(argument, IO);                                // Run it.
}
/****************************************************************************
*  Parser Find Implied                                                      *
*****************************************************************************/
bool scpi_find_implied(SCPI_Node_t **current_command, char *token, SCPI_Node_t **scpi_nodes)
{
    const SCPI_Node_t *current_ptr;
    const SCPI_Node_t *parent_ptr;
    char long_name[MAX_TOKEN_LEN+1];
    char short_name[MAX_TOKEN_LEN+1];
    uint16_t command_index;

    command_index = 0;
    while(scpi_nodes[command_index] != NULL)                                    // Walk through known commands looking for a match.
    {
        current_ptr = scpi_nodes[command_index];                                // Get pointer to found command
        strncpy(long_name, current_ptr->scpi_node, MAX_TOKEN_LEN);
        strncpy(short_name, scpi_get_short_name(long_name), MAX_TOKEN_LEN);
        if (!strcmp(long_name, token) || !strcmp(short_name, token))            // If the command matches long name or short name
        {
            while(true)
            {
                parent_ptr = current_ptr->parent;
                if (parent_ptr == *current_command)
                {
                    *current_command = scpi_nodes[command_index];
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
        command_index++;
    }
    return false;
}
/****************************************************************************
*  Parser Get Short Name                                                    *
*****************************************************************************/
char *scpi_get_short_name(char *long_name )
{
    uint16_t long_index = 0;
    uint16_t short_index = 0;
    static char short_name[MAX_TOKEN_LEN+1];

    short_name[0] = NUL;
    while (long_name[long_index] != NUL)                                // Walk the name string
    {   
        if (long_name[long_index] == toupper(long_name[long_index]))    // Check if the character is uppercase
        {   
            short_name[short_index++] = long_name[long_index];          // build up the short_name
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
void scpi_add_error_P(PGM_P error_message, char *bad_command, uint16_t max_length)
{
    if (error_count < ERROR_QUEUE_LENGTH)
    {
        error_count++;
        strncpy(error_log[error_count-1].entered_value, bad_command, max_length);
        error_log[error_count-1].error_message = error_message;
    }
}
/****************************************************************************
*  Adds too few parameters error to queue                                   *
*****************************************************************************/
void scpi_add_error_too_few_parameters()
{
    scpi_add_error_P(TOO_FEW_PARAMETERS, "", 0);
}
/****************************************************************************
*  Prints a program memory string one char at a time to the terminal        *
*****************************************************************************/
void scpi_prStr_P(PGM_P progmem_string, FILE *fstream)
{
    uint16_t string_index=0;
    char the_char = pgm_read_byte(progmem_string);

    while (the_char != NUL)
    {
        fputc(the_char, fstream);
        the_char = pgm_read_byte(&progmem_string[++string_index]);
    }
}
/****************************************************************************
*  :SYSTem:ERRor?                                                           *
*****************************************************************************/
void sys_error_q(char *arg, IO_pointers_t IO)
{
    if (error_count == 0)
        scpi_prStr_P(PSTR("+0,\"No error\"\r\n"), IO.USB_stream);
    else
    {
        scpi_prStr_P(error_log[error_count-1].error_message, IO.USB_stream);
        fprintf(IO.USB_stream, "%s", error_log[error_count-1].entered_value);
        scpi_prStr_P_cr_nl(IO.USB_stream);
        error_count--;
    }
}
/****************************************************************************
*  SCPI Print New Line And Carriage Return                                  *
*****************************************************************************/
void scpi_prStr_P_cr_nl(FILE *fstream)
{
    scpi_prStr_P(PSTR("\r\n"), fstream);
}