/****************************************************************************
 *                                                                          *
 *  SCPI Parser                                                             *
 *                                                                          *
 ****************************************************************************/

#ifndef SCPI_PARSER_H_
#define SCPI_PARSER_H_

    #include <avr/pgmspace.h>           // Should need this for PGM_P and PROGMEM but doesn't. Coming in from somewhere else...?
    #include <string.h>                 // strlen
    #include <ctype.h>                  // isspace
    #include <i2c_twi.h>                // i2c_twi_port_t
    #include <process_USB.h>
/****************************************************************************
*  Define various SCPI parameters                                           *
*****************************************************************************/
    #define MAX_SCPI_NODES 50
    #define MAX_IN_STR_LEN 80
    #define MAX_TOKEN_LEN 40
    #define ERROR_QUEUE_LENGTH 2
    #define MAX_ARG_LEN 16
    #define DEL_KEY '\x7F'
    #define NUL '\0'
/****************************************************************************
*  Create a data structure to contain the various IO pointers               *
*  that need global scope                                                   *
*****************************************************************************/
typedef struct {
    FILE* USB_stream;           // Stream to the USB port
    FILE* LCD_stream;           // Stream to the LCD Screen
    i2c_twi_port_t* I2C_port;   // Pointer to the I²C Port
    } IO_pointers_t;    
/****************************************************************************
*  Macros to be used when creating the project specific SCPI array          *
*****************************************************************************/
#define SCPI_OPEN_REGISTRY(MACRO_PASSED_ARRAY, miscount_func)                   /* Macro initializer                                                                    */ \
        void (*miscount_function)(void) = miscount_func;                        /* Stash a copy of the miscount function for later                                      */ \
        uint16_t registry_index = 0;                                            /* Create the array indexer                                                             */ \
        SCPI_Node_t **__local_command_array = MACRO_PASSED_ARRAY                /* Rename the command array to a local secret variable passed by the registry initilzer */

#define SCPI(cvar_name, scpi_node, parent, function, implied)                   /* Macro initializer                                                                    */ \
        static SCPI_Node_t cvar_name = {scpi_node, parent, function, implied};  /* Create a local instance of each SCPI node (instance of a SCPI_Node_t struct)         */ \
        __local_command_array[registry_index++] = &cvar_name;                   /* Append the command array with the newly created instance of a SCPI_Node_t            */ \
        if (registry_index >= MAX_SCPI_NODES) miscount_function()               /* Run the error function defined for SCPI array size problems (Jump to Bootloader?)    */

#define SCPI_CLOSE_REGISTRY() __local_command_array[registry_index] = NULL      /* Close out the command array with a loop detectbale terminating instance              */
/****************************************************************************
*  SCPI Node Struct                                                         *
*****************************************************************************/
typedef struct SCPI_Node_t
    {
        PGM_P scpi_node;
        const struct SCPI_Node_t *parent;
        void (*function)(char* arg, IO_pointers_t IO);
        const bool implied;
    } SCPI_Node_t;
/****************************************************************************
*  SCPI Error Struct                                                        *
*****************************************************************************/
typedef struct
    {
        char entered_value[MAX_TOKEN_LEN+1];
        PGM_P error_message;
    } SCPI_Error_t;
/****************************************************************************
* SCPI Core Functions                                                       *
*****************************************************************************/
    void process_scpi_input(SCPI_Node_t **scpi_nodes, IO_pointers_t IO);
    void scpi_process_cmd_P( char* input_string, SCPI_Node_t **scpi_nodes, IO_pointers_t IO);
    bool scpi_find_implied(SCPI_Node_t **current_command, char *token, SCPI_Node_t **scpi_nodes);
    char *scpi_get_short_name(char *long_name);
    void scpi_add_error_P(PGM_P error_message, char *bad_command, uint16_t max_length);
    void scpi_add_error_too_few_parameters(void);
    void scpi_prStr_P(PGM_P progmem_string, FILE *fstream);
    void scpi_prStr_P_cr_nl(FILE *fstream);
    void sys_error_q(char *arg, IO_pointers_t IO);
/****************************************************************************
*  Compulsory SCPI commands                                                 *
*  see: http://www.ivifoundation.org/docs/scpi-99.pdf                       *
*****************************************************************************/
 #endif