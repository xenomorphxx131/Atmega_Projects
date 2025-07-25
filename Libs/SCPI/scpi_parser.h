/****************************************************************************
 *                                                                          *
 *  SCPI Parser                                                             *
 *                                                                          *
 ****************************************************************************/

#ifndef SCPI_PARSER_H_
#define SCPI_PARSER_H_

    #include <avr/pgmspace.h>           // Should need this for PGM_P but doesn't. Coming in from somewhere else.
    #include <string.h>                 // strlen
    #include <ctype.h>                  // isspace
    #include <i2c_twi.h>                // i2c_twi_port_t
    #include "Awesomefaucet_info.h"     // Command array size.
    #include <process_USB.h>
/****************************************************************************
*  Define various SCPI parameters                                           *
*****************************************************************************/
	#define MAX_IN_STR_LEN 260
	#define MAX_TOKEN_LEN 80
	#define ERROR_QUEUE_LEN 2
	#define ERROR_MESSAGE_LEN 24
	#define MAX_TOKEN_COUNT 16
	#define MAX_ARG_LEN 16
	#define DEL_KEY 127
	#define NUL '\0'
/****************************************************************************
*  Create a data structure to contain the various IO pointers               *
*  that need global scope                                                   *
*****************************************************************************/
typedef struct {
	FILE*		    USB_stream;                     // Stream to the USB port
	FILE*		    LCD_stream;                     // Stream to the LCD Screen
	i2c_twi_port_t*	I2C_port;                       // Pointer to the I²C Port
	} IO_pointers_t;

typedef struct scpi_node_P {
	PGM_P		name;							    // Node Name
	uint8_t		implied;						    // Implied Command TODO storing this as a PGM_P probably takes more ram than a char or uint8_t
	struct		scpi_node_P *parent;			    // Pointer to Parent Node
	void		(*function)(char*, IO_pointers_t);  // Command Function
	} scpi_commands_P_t;
/****************************************************************************
* SCPI Core Functions                                                       *
*****************************************************************************/
	void process_scpi_input( char * str_in, int *str_len, scpi_commands_P_t cmd_array_P[], IO_pointers_t IO );
    void scpi_process_cmd_P ( char * input_string, scpi_commands_P_t cmd_array_P[], IO_pointers_t IO );
    bool scpi_find_implied(scpi_commands_P_t **current_state, char *token, scpi_commands_P_t cmd_array_P[]);
    void scpi_get_short_name(char *lname, char *sname);
	void scpi_add_error_P(PGM_P error_message, IO_pointers_t IO);
	void scpi_prStr_P (  PGM_P arg, FILE *fstream );
	void PGM_P_to_string ( PGM_P arg, char * name, FILE *fstream);
	void scpi_null_func ( char *arg, IO_pointers_t IO);
	void remove_ws ( char *arg );
/****************************************************************************
*  Compulsory SCPI commands                                                 *
*  see: http://www.ivifoundation.org/docs/scpi-99.pdf                       *
*****************************************************************************/
 #endif