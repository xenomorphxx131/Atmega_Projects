#include "scpi.h"

uint16_t error_count = 0;
SCPI_Error_t error_log[ERROR_QUEUE_LENGTH];

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
            printf("\n"); // Windows only
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
        if (strlen(space+1) > MAX_ARG_LEN)
        {
            scpi_add_error_P(ARG_TOO_LONG, space+1, MAX_TOKEN_LEN);             // and...
            return;
        }
        strncpy(argument, space+1, MAX_ARG_LEN);                                // Copy up to max arg length
        if (strpbrk(argument, " ") != NULL)                                     // If any more spaces...
        {                                                                       // Declare the error,
            scpi_add_error_P(TOO_MANY_PARAMETERS, argument, MAX_ARG_LEN);       // and...
            return;                                                             // Bail from the whole SCPI input
        }
    }
    /****************************************************************************
     * Parse the command tokens                                                 *
     ****************************************************************************/
    separator = strtok(input_string, ":");                                          // Scan string looking for ":" separators
    while (separator != NULL)                                                       // until we run out of tokens
    {                                                                           // Search for new tokens
        valid_command = false;
        strncpy(token, separator, MAX_TOKEN_LEN);
        /************************************************************************
         * Walk the command list in pairs keeping a continuous link to the past *
         ************************************************************************/
        command_index = 0;
        while(scpi_nodes[command_index] != NULL)
        {
            next_command = scpi_nodes[command_index];                                 // Get a pointer to next_command.
            if (next_command->parent == current_command)                        // Only proceed if the next_command's parent is the current_command.
            {
                strncpy(long_name, scpi_nodes[command_index]->scpi_node, MAX_TOKEN_LEN);
                strncpy(short_name, scpi_get_short_name(long_name), MAX_TOKEN_LEN);
                if (!strcmp(long_name, token) || !strcmp(short_name, token))    // If the command matches either long_name or short_name...
                {                                                               //
                    current_command = next_command;                             // Make the current_command the next_command for the next iteration.
                    valid_command = true;                                       // Mark it as valid up until now.
                    break;                                                      // Stop walking the command list once a valid command is found.
                }
            }
            command_index++;
        }
        if (!valid_command)                                                     // If no match check for implied match.
        {
            if (!scpi_find_implied(&current_command, token, scpi_nodes))        // If an implied command wasn't found, there was a real error. This function updates current_command.
            {
                scpi_add_error_P(BAD_PATH_OR_HEADER, token, MAX_TOKEN_LEN);                       // Indicate bad token
                return;                                                         // Bail out
            }
            else
                valid_command = true;                                           // Not really needed, will run the function if it didn't bail during implied check.
        }
        separator = strtok(NULL, ":");                                              // Otherwise get the next ":" separated token
        if (separator != NULL)
            strncpy(token, separator, MAX_TOKEN_LEN);
    }                                                                           // End of new token search
    if (valid_command && current_command->function != NULL)
        current_command->function(argument, IO);                                // If it's valid or it didn't bail at scpi_find_implied(), it should be a runnable head.
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

    for (uint16_t command_index=0; scpi_nodes[command_index] != NULL; command_index++)  // Walk through known commands looking for a match.
    {
        current_ptr = scpi_nodes[command_index];                              // Get pointer to found command
        strncpy(long_name, current_ptr->scpi_node, MAX_TOKEN_LEN);
        strncpy(short_name, scpi_get_short_name(long_name), MAX_TOKEN_LEN);
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
    static char short_name[MAX_TOKEN_LEN+1] = "";                       // Static so it can be passed back out

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

void scpi_add_error_P(const char *error_message, char *bad_command, uint16_t max_length)
{
    if (error_count < ERROR_QUEUE_LENGTH)
    {
        error_count++;
        strncpy(error_log[error_count-1].entered_value, bad_command, max_length);
        error_log[error_count-1].error_message = error_message;
    }
}
/****************************************************************************
*  :SYSTem:ERRor?                                                           *
*****************************************************************************/
void sys_error_q(char *arg, IO_pointers_t IO)
{
    if (error_count == 0)
        printf(NO_ERROR);
    else
    {
        scpi_prStr_P(error_log[error_count-1].error_message, IO);
        printf("%s\r\n", error_log[error_count-1].entered_value);
        error_count--;
    }
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
    REGISTER_SCPI(BTLOADER,                 "BTLOader",                 &RST,       &sys_rst_btloader,                      false);
    REGISTER_SCPI(ERROR,                "ERRor?",                       &SYSTEM,    &sys_error_q,                           false);
    // REGISTER_SCPI(VERSION,              "VERSion?",                     &SYSTEM,    &scpi_get_version_q,                    false);
    // REGISTER_SCPI(RECORD,               "RECORD",                       &SYSTEM,    &scpi_record,                           false);

    REGISTER_SCPI(GET,              "GET",                              NULL,       NULL,                                   false);
    REGISTER_SCPI(RANGEq,               "RANGE?",                       &GET,       &scpi_get_range_q,                      false);
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

void st_OPC_q(char *arg, int IO)
{
    printf("Running st_OPC_q()\n\r");
}
void scpi_IDN_q(char *arg, int IO)
{
    printf("Running scpi_IDN_q()\n\r");
}

void scpi_get_range_q(char *arg, int IO)
{
    printf("Running scpi_get_range_q()\n\r");
}

void sys_rst_btloader(char *arg, int IO)
{
    printf("Running sys_rst_btloader()\n\r");
}

int CDC_Device_ReceiveByte()
{
    return getche();
}
/****************************************************************************
*  Prints a program memory string one char at a time to the terminal        *
*****************************************************************************/
void scpi_prStr_P(const char *arg, IO_pointers_t IO)
{
    printf(arg);
}

    // int command_index = 0;
    // while(scpi_nodes[command_index] != NULL)
    // {
        // printf("address: 0x%x: %s\r\n", scpi_nodes[command_index], scpi_nodes[command_index]->scpi_node);
        // command_index++;
    // }
    // printf("Adding error..\r\n");
    // printf("Error Number: %u, Value: \"%s\", Message: %s \r\n\r\n", error_count, error_log[error_count-1].entered_value, error_log[error_count-1].error_message);

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
        /************************************************************************
         * Check the command length                                             *
         ************************************************************************/
        // if (strlen(token) > MAX_TOKEN_LEN)                                          Can no longer be too long, strncpy will truncate
        // {                                                                           Can no longer be too long, strncpy will truncate
            // scpi_add_error_P(MNEMONIC_TOO_LONG, "", 0);                             Can no longer be too long, strncpy will truncate
            // return;                                                                 Can no longer be too long, strncpy will truncate
        // }                                                                           Can no longer be too long, strncpy will truncate
        
        
        
            // printf("Checking Address: 0x%x, Command: %s\r\n", scpi_nodes[command_index], scpi_nodes[command_index]->scpi_node);
            // printf("Line 126, hit enter to continue: \r\n");
            // getchar();
        // printf("Token: %s\r\n", token);
        // printf("Valid Command: %s\r\n", valid_command ? "True" : "False");


    
        // printf(" Line 163, hit enter to continue: \r\n");
        // getchar();
    // printf(" Line 163, hit enter to continue: \r\n");
    // getchar();
    
    