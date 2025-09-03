#ifndef SCPI_H_
#define SCPI_H_

	#include <string.h>
	#include <stdio.h>
	#include <ctype.h>

	#include <avr/io.h>
	#include <avr/wdt.h>
	#include <avr/power.h>
	#include <avr/interrupt.h>
	#include <avr/eeprom.h>
	#include <avr/pgmspace.h>

	#include <LUFA/Version.h>
	#include <LUFA/Drivers/USB/USB.h>
	#include <process_USB.h>

	#include <i2c_twi.h>
	#include <scpi_config.h>
	#include <bootloader.h>

	#define NUL					'\0'
	#define DEL_KEY				127
	#define	NO					0
	#define YES					1
	#define FALSE				0
	#define TRUE				1

/**************************************************************************
*  Create a data structure to contain the various IO pointers             *
*  that need global scope                                                 *
***************************************************************************/
typedef struct {
	FILE*		    USB_stream;							    // Stream to the USB port
	FILE*		    LCD_stream;							    // Stream to the LCD Screen
	i2c_port_t*	    I2C_port1;							    // Pointer to the first I²C Port
	} IO_pointers_t;

typedef struct scpi_node_P {
	PGM_P		name;										// Node Name
	uint8_t		implied;									// Implied Command TODO storing this as a PGM_P probably takes more ram than a char or uint8_t
	struct		scpi_node_P *parent;						// Pointer to Parent Node
	void		(*function)(char*, IO_pointers_t);			// Command Function
	} scpi_commands_P_t;
	
	int	 SetupScpiCommandsArray_P (scpi_commands_P_t cmd_array_P[COMMAND_ARRAY_SIZE], int i);
	void scpi_process_cmd_P ( char * input_string, scpi_commands_P_t cmd_array_P[COMMAND_ARRAY_SIZE], IO_pointers_t IO );
	void scpi_process_input( char * str_in, int *str_len, scpi_commands_P_t cmd_array_P[COMMAND_ARRAY_SIZE], IO_pointers_t IO );
	void sys_rst_btloader ( char *arg, IO_pointers_t IO );
	void sys_error_q ( char *arg, IO_pointers_t IO );
	void scpi_empty_func ( char *arg, IO_pointers_t IO );
	void scpi_null_func ( char *arg, IO_pointers_t IO );
	void scpi_add_error(PGM_P error_txt, IO_pointers_t IO);
	void scpi_add_error_P(PGM_P error_message, IO_pointers_t IO);
	void scpi_ver(char*, IO_pointers_t IO);
	void scpi_prStr_P (  PGM_P arg, FILE *fstream );
	void PGM_P_to_string ( PGM_P arg, char * name , FILE *fstream);

/**************************************************************************
*  Compulsory SCPI commands                                               *
*  see: http://www.ivifoundation.org/docs/scpi-99.pdf                     *
***************************************************************************/
	void st_CLS		( char *arg, IO_pointers_t IO );
	void st_ESE		( char *arg, IO_pointers_t IO );
	void st_ESE_q	( char *arg, IO_pointers_t IO );
	void st_ESR_q	( char *arg, IO_pointers_t IO );
//	void st_IDN_q	( char *arg, IO_pointers_t IO ); // See product specific SCPI file <project_name>_scpi.h
	void st_OPC		( char *arg, IO_pointers_t IO );
	void st_OPC_q	( char *arg, IO_pointers_t IO );
	void st_RST		( char *arg, IO_pointers_t IO );
	void st_SRE		( char *arg, IO_pointers_t IO );
	void st_SRE_q	( char *arg, IO_pointers_t IO );
	void st_STB		( char *arg, IO_pointers_t IO );
	void st_TST		( char *arg, IO_pointers_t IO ); // See product specific SCPI file <project_name>_scpi.h
	void st_WAI		( char *arg, IO_pointers_t IO );

#endif // SCPI_H_