/****************************************************************************
 *                                                                          *
 *  Awesomefaucet SCPI                                                      *
 *                                                                          *
 ****************************************************************************/

#ifndef AWESOMEFAUCET_SCPI_H_
#define AWESOMEFAUCET_SCPI_H_

    #include <stdlib.h>					// atoi used in 'scpi_laser_power'
    #include <process_USB.h>
    #include <bootloader.h>				// 'Jump_To_Bootloader'
    #include "scpi_config.h"
    #include "Awesomefaucet_board.h"
    #include "Awesomefaucet_sensor.h"
/**************************************************************************
*  Create a data structure to contain the various IO pointers             *
*  that need global scope                                                 *
***************************************************************************/
typedef struct {
	FILE*		    USB_stream;                             // Stream to the USB port
	FILE*		    LCD_stream;                             // Stream to the LCD Screen
	i2c_twi_port_t*	I2C_port;                               // Pointer to the I²C Port
	} IO_pointers_t;

typedef struct scpi_node_P {
	PGM_P		name;										// Node Name
	uint8_t		implied;									// Implied Command TODO storing this as a PGM_P probably takes more ram than a char or uint8_t
	struct		scpi_node_P *parent;						// Pointer to Parent Node
	void		(*function)(char*, IO_pointers_t);			// Command Function
	} scpi_commands_P_t;
	
/**************************************************************************
* Create a data structure to contain the load box state                   *
***************************************************************************/
	void process_scpi_input( char * str_in, int *str_len, scpi_commands_P_t cmd_array_P[], IO_pointers_t IO );
    void scpi_process_cmd_P ( char * input_string, scpi_commands_P_t cmd_array_P[], IO_pointers_t IO );
    bool scpi_find_implied(scpi_commands_P_t **current_state, char *token, scpi_commands_P_t cmd_array_P[]);
    void scpi_get_short_name(char *lname, char *sname);
	void scpi_add_error_P(PGM_P error_message, IO_pointers_t IO);
	void scpi_prStr_P (  PGM_P arg, FILE *fstream );
	void PGM_P_to_string ( PGM_P arg, char * name , FILE *fstream);
	int	 Setup_ScpiCommandsArray_P ( scpi_commands_P_t command_array_P[] );
	void scpi_null_func ( char *arg, IO_pointers_t IO );
	void st_OPC_q	( char *arg, IO_pointers_t IO );
	void scpi_empty_func ( char *arg, IO_pointers_t IO );
	void sys_rst_btloader ( char *arg, IO_pointers_t IO );
	void sys_error_q ( char *arg, IO_pointers_t IO );
	void scpi_get_version_q(char*, IO_pointers_t IO);
	void scpi_IDN_q( char *arg, IO_pointers_t IO );
	void st_CLS ( char *arg, IO_pointers_t IO );
	void st_RST ( char *arg, IO_pointers_t IO );
	void st_TST ( char *arg, IO_pointers_t IO ); // See product specific SCPI file <project_name>_scpi.h
	void st_WAI ( char *arg, IO_pointers_t IO );
    void debug (char *arg, IO_pointers_t IO);
    void scpi_get_range_q (char *arg, IO_pointers_t IO);
    void scpi_get_als_q(char *arg, IO_pointers_t IO);
	void scpi_get_laserpower_q( char *arg, IO_pointers_t IO );
    void scpi_laser_power(char *arg, IO_pointers_t IO);
    void scpi_laser_auto (char *arg, IO_pointers_t IO);
	void scpi_water_auto (char *arg, IO_pointers_t IO);
    void scpi_water_on (char *arg, IO_pointers_t IO);
    void scpi_water_off (char *arg, IO_pointers_t IO);
    void scpi_store_floordarkness( char *arg, IO_pointers_t IO );
    void scpi_get_floordarkness_q( char *arg, IO_pointers_t IO );
	void scpi_set_IIR_value( char *arg, IO_pointers_t IO );
	void scpi_get_IIR_value( char *arg, IO_pointers_t IO );
    void clr_i2c (char *arg, IO_pointers_t IO);
    void water_on(bool on);
    uint8_t get_darkness_setting(void);
	void update_darkness_setting(void);
	void update_IIR_value(void);
	void remove_ws ( char *arg );
/**************************************************************************
*  Compulsory SCPI commands                                               *
*  see: http://www.ivifoundation.org/docs/scpi-99.pdf                     *
***************************************************************************/
	// void st_ESE		( char *arg, IO_pointers_t IO );
	// void st_ESE_q	( char *arg, IO_pointers_t IO );
	// void st_ESR_q	( char *arg, IO_pointers_t IO );
//	void st_IDN_q	( char *arg, IO_pointers_t IO ); // See product specific SCPI file <project_name>_scpi.h
	// void st_SRE		( char *arg, IO_pointers_t IO );
	// void st_SRE_q	( char *arg, IO_pointers_t IO );
	// void st_STB		( char *arg, IO_pointers_t IO );
 #endif

 
 
 
 
 