#include "scpi.h"

char bad_command[MAX_TOKEN_LEN + 1];
uint16_t error_number = 0;
const char *error_messages[ERROR_QUEUE_LEN];

int main(void)
{
    #define MAX_NODES 50
    IO_pointers_t IO = 0; //  TODO deleteme
    SCPI_Node_t* scpi_nodes[MAX_NODES];
    setup_scpi_commands(scpi_nodes);

    printf("Sucessfully completed process_scpi_input_new...\r\n");
    while(true)
    {
        process_scpi_input(scpi_nodes, IO);
    }
}
/****************************************************************************
*  Build input string from terminal then run SCPI command.                  *
*****************************************************************************/
void process_scpi_input(SCPI_Node_t **scpi_nodes, IO_pointers_t IO)
{
    uint16_t ReceivedByte;
    char usb_data;                                                                  // Temporary USB data storage
    static uint16_t string_index = 0;                                               // Running string index
    static char input_string[MAX_IN_STR_LEN+1] = "";                                // Running incoming string
    
    ReceivedByte = CDC_Device_ReceiveByte();
    while (ReceivedByte >= 0)                                                       // Returns -1 if empty, else 0-255
    {
        usb_data = toupper((char)ReceivedByte);                                               // Get all chars and convert to upper case.
        if ((usb_data == ';') || (usb_data == '\r') || (usb_data == '\n'))          // Check for one of three SCPI terminations
        {
            // printf("Found Terminator\r\n");
            if (string_index > 0)
                scpi_process_cmd_P(input_string, scpi_nodes, IO);                   // Process
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
                    printf("ERROR command too long\r\n");
            }
        }
        ReceivedByte = CDC_Device_ReceiveByte();
    }
}
/****************************************************************************
*  Parse and process input string from com port then run SCPI command.      *
*****************************************************************************/
void scpi_process_cmd_P( char *input_string, SCPI_Node_t **scpi_nodes, IO_pointers_t IO)
{
    char *token;
    char *argument;
    char *long_name;
    char *short_name;   
    SCPI_Node_t *current_command = NULL;                                        // Pointer to commands in command array. Initialize to the top (no command).
    SCPI_Node_t *next_command;                                                  // Pointer to commands in command array used for tokens.
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
        for (uint16_t command_index=0; scpi_nodes[command_index] != NULL; command_index++)  // Walk through known commands looking for a match.
        {
            next_command = scpi_nodes[command_index];                                 // Get a pointer to next_command.
            if (next_command->parent == current_command)                        // Only proceed if the next_command's parent is the current_command.
            {
                // long_name = PGM_P_to_string(scpi_nodes[command_index]->scpi_node);           // Get the long_name
                long_name = scpi_nodes[command_index]->scpi_node;           // Get the long_name
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
            if (!scpi_find_implied(&current_command, token, scpi_nodes))    // If an implied command wasn't found, there was a real error. This function updates current_command.
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
    if (valid_command)                                                          // Not needed. If it got this far, it's runnable.
        current_command->function(argument, IO);                                // If it's valid or it didn't bail at scpi_find_implied(), it should be a runnable head.
}
/****************************************************************************
*  Parser Find Implied                                                      *
*****************************************************************************/
bool scpi_find_implied(SCPI_Node_t **current_command, char *token, SCPI_Node_t **scpi_nodes)
{
    const SCPI_Node_t *current_ptr;
    const SCPI_Node_t *parent_ptr;
    char *long_name;
    char *short_name;

    for (uint16_t command_index=0; scpi_nodes[command_index] != NULL; command_index++)  // Walk through known commands looking for a match.
    {
        current_ptr = scpi_nodes[command_index];                              // Get pointer to found command
        long_name = current_ptr->scpi_node;
        // long_name = PGM_P_to_string(current_ptr->scpi_node);
        short_name = scpi_get_short_name(long_name);
        if (!strcmp(long_name, token) || !strcmp(short_name, token))    // If the command matches long name or short name
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
*  Setup Awesomfaucet Specific SCPI commands and functions                  *
*****************************************************************************/
void setup_scpi_commands(SCPI_Node_t **scpi_nodes)
{
    OPEN_SCPI_REGISTRY();

 // REGISTER_SCPI(.node,           .value,                             .parent,    .function,                              .implied);
 // --------------------------------------------------------------------------------------------------------------------------------
    REGISTER_SCPI(OPC,              "*OPC?",                            NULL,       &st_OPC_q,                              false);
    REGISTER_SCPI(IDN,              "*IDN?",                            NULL,       &scpi_IDN_q,                            false);
    // REGISTER_SCPI(CLRI2C,           "CLRI2C",                           NULL,       &clr_i2c,                               false);
    // REGISTER_SCPI(DEBUG,            "DEBUG?",                           NULL,       &debug,                                 false);

    REGISTER_SCPI(SYSTEM,           "SYSTem",                           NULL,       NULL,                                   true);
    REGISTER_SCPI(RST,                  "RST",                          &SYSTEM,    NULL,                                   false);
    // REGISTER_SCPI(BTLOADER,                 "BTLOader",                 &RST,       &sys_rst_btloader,                      false);
    REGISTER_SCPI(ERROR,                "ERRor?",                       &SYSTEM,    &sys_error_q,                           false);
    // REGISTER_SCPI(VERSION,              "VERSion?",                     &SYSTEM,    &scpi_get_version_q,                    false);
    // REGISTER_SCPI(RECORD,               "RECORD",                       &SYSTEM,    &scpi_record,                           false);

    // REGISTER_SCPI(GET,              "GET",                              NULL,       NULL,                                   false);
    // REGISTER_SCPI(RANGEq,               "RANGE?",                       &GET,       &scpi_get_range_q,                      false);
    // REGISTER_SCPI(IIR_ALPHAq,           "IIR_ALPHA?",                   &GET,       &scpi_get_IIR_alpha_q,                  false);
    // REGISTER_SCPI(IIR_BETAq,            "IIR_BETA?",                    &GET,       &scpi_get_IIR_beta_q,                   false);
    // REGISTER_SCPI(IIR_GAINq,            "IIR_GAIN?",                    &GET,       &scpi_get_IIR_gain_q,                   false);
    // REGISTER_SCPI(LASER_POWERq,         "LASERPOWER?",                  &GET,       &scpi_get_laserpower_q,                 false);
    // REGISTER_SCPI(DET_THRESHOLDq,       "DETECTION_THRESHOLD_MM?",      &GET,       &scpi_get_detection_threshold_mm_q,     false);
    // REGISTER_SCPI(MAX_DIS_LEAKAGEq,     "MAX_DISTANCE_LEAKAGE?",        &GET,       &scpi_get_max_distance_leakage_q,       false);
    // REGISTER_SCPI(WATER_DEB_TIMEOUTq,   "WATER_DEBOUNCE_TIMEOUT?",      &GET,       &scpi_get_water_debounce_timeout_q,     false);
    // REGISTER_SCPI(BLACKBOXq,            "BLACKBOX?",                    &GET,       &scpi_get_blackbox_q,                   false);
    // REGISTER_SCPI(MX_DIST_MM_RST_RATEq, "MAX_DISTANCE_MM_RESET_RATE?",  &GET,       &scpi_get_max_distance_mm_reset_rate_q, false);
    // REGISTER_SCPI(RANGESq,              "RANGE_AND_MAXRANGE?",          &GET,       &scpi_get_range_and_maxrange_q,         false);
    // REGISTER_SCPI(WATER_STATEq,         "WATER_STATE?",                 &GET,       &scpi_water_state_q,                    false);

    // REGISTER_SCPI(SET,              "SET",                              NULL,       NULL,                                   false);
    // REGISTER_SCPI(IIR_ALPHA,            "IIR_ALPHA",                    &SET,       &scpi_set_IIR_alpha,                    false);
    // REGISTER_SCPI(IIR_BETA,             "IIR_BETA",                     &SET,       &scpi_set_IIR_beta,                     false);
    // REGISTER_SCPI(LASER_POWER,          "LASERPOWER",                   &SET,       &scpi_set_laserpower,                   false);
    // REGISTER_SCPI(DET_THRESHOLD,        "DETECTION_THRESHOLD_MM",       &SET,       &scpi_set_detection_threshold_mm,       false);
    // REGISTER_SCPI(MAX_DIS_LEAKAGE,      "MAX_DISTANCE_LEAKAGE",         &SET,       &scpi_set_max_distance_leakage,         false);
    // REGISTER_SCPI(WATER_DEB_TIMEOUT,    "WATER_DEBOUNCE_TIMEOUT",       &SET,       &scpi_set_water_debounce_timeout,       false);
    // REGISTER_SCPI(MX_DIST_MM_RST_RATE,  "MAX_DISTANCE_MM_RESET_RATE",   &SET,       &scpi_set_max_distance_mm_reset_rate,   false);
    // REGISTER_SCPI(WATER_v,              "WATER",                        &SET,       NULL,                                   false);
    // REGISTER_SCPI(ON,                       "ON",                       &WATER_v,   &scpi_water_on,                         false);
    // REGISTER_SCPI(OFF,                      "OFF",                      &WATER_v,   &scpi_water_off,                        false);
    // REGISTER_SCPI(AUTO,                     "AUTO",                     &WATER_v,   &scpi_water_auto,                       false);

    CLOSE_SCPI_REGISTRY();
}

void st_OPC_q ( char *arg, int IO )
{
    printf("Running st_OPC_q()\n\r");
}
void scpi_IDN_q ( char *arg, int IO )
{
    printf("Running scpi_IDN_q()\n\r");
}

int CDC_Device_ReceiveByte()
{
    return getche();
}

void scpi_add_error_P(const char *error_message, IO_pointers_t IO)
{
    if (error_number < ERROR_QUEUE_LEN)
        error_messages[++error_number] = error_message;
}

void scpi_add_error_arg_too_long(int IO)
{
    printf("ERROR argument too long\r\n");
}
/****************************************************************************
*  :SYSTem:ERRor?                                                           *
*****************************************************************************/
void sys_error_q(char *arg, IO_pointers_t IO)
{
    if (error_number == 0)
        printf("+0,\"No error\"\r\n");
    else
    {
        scpi_prStr_P(error_messages[error_number], IO);
        if (bad_command[0] != NUL)
        {
            printf("%s\r\n", bad_command);
            bad_command[0] = NUL;
        }
        error_number--;
    }
}


/****************************************************************************
*  Prints a program memory string one char at a time to the terminal        *
*****************************************************************************/
void scpi_prStr_P(const char *arg, IO_pointers_t IO)
{
    printf(arg);
}


// char *PGM_P_to_string(char *arg)
// {
    // static char name[MAX_TOKEN_LEN+1] = "";
    // uint16_t i = 0;

    // name[i] = arg[i];
    // while (name[i] != NUL)
    // {
        // i++;
        // name[i] = arg[i];
    // }
    // i++;
    // name[i] = NUL;
    // return name;
// }