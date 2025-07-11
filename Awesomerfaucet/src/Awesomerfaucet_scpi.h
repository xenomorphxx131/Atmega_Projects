/****************************************************************************
 *                                                                          *
 *  Awesomefaucet SCPI                                                      *
 *                                                                          *
 ****************************************************************************/

#ifndef AWESOMEFAUCET_SCPI_H_
#define AWESOMEFAUCET_SCPI_H_

    #include <stdlib.h>						// atoi, strtof etc.
    #include <process_USB.h>
    #include <bootloader.h>					// 'Jump_To_Bootloader'
	#include <avr/interrupt.h>          	// Needed for cli() and sei()
    #include "Awesomerfaucet_scpiconfig.h"
    #include "Awesomerfaucet_sensor.h"
    #include "Awesomerfaucet_board.h"
    #include "Awesomerfaucet_laser.h"
    #include "Awesomerfaucet_water.h"
    #include "Awesomerfaucet_timers.h"
    #include "VL53L4CD_api.h"               // To get direct access to sensor data types and methods
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
	void scpi_get_laserpower_q( char *arg, IO_pointers_t IO );
	void scpi_water_auto (char *arg, IO_pointers_t IO);
    void scpi_water_on (char *arg, IO_pointers_t IO);
    void scpi_water_off (char *arg, IO_pointers_t IO);
    void scpi_water_state_q (char *arg, IO_pointers_t IO);
    void scpi_set_laserpower( char *arg, IO_pointers_t IO );
	void scpi_set_IIR_alpha( char *arg, IO_pointers_t IO );
	void scpi_get_IIR_alpha_q( char *arg, IO_pointers_t IO );
	void scpi_set_IIR_beta( char *arg, IO_pointers_t IO );
	void scpi_get_IIR_beta_q( char *arg, IO_pointers_t IO );
    void scpi_get_IIR_gain_q( char *arg, IO_pointers_t IO );
    void clr_i2c (char *arg, IO_pointers_t IO);
	void retrieve_laserpower_setting(void);
	void retrieve_IIR_alpha(void);
	void retrieve_IIR_beta(void);
    void compute_iir_gain(void);
    void scpi_set_detection_threshold_mm( char *arg, IO_pointers_t IO );
    void scpi_get_detection_threshold_mm_q( char *arg, IO_pointers_t IO );
    void retrieve_detection_threshold_mm(void);
    void scpi_set_max_distance_leakage( char *arg, IO_pointers_t IO );
    void retrieve_max_distance_leakage(void);
    void scpi_get_max_distance_leakage_q( char *arg, IO_pointers_t IO );
    
    
    void scpi_set_water_debounce_timeout( char *arg, IO_pointers_t IO );
    void retrieve_water_debounce_timeout(void);
    void scpi_get_water_debounce_timeout_q( char *arg, IO_pointers_t IO );
    
    
    
	void remove_ws ( char *arg );
/**************************************************************************
*  Compulsory SCPI commands                                               *
*  see: http://www.ivifoundation.org/docs/scpi-99.pdf                     *
***************************************************************************/
 #endif