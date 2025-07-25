/****************************************************************************
 *                                                                          *
 *  Awesomefaucet SCPI                                                      *
 *                                                                          *
 ****************************************************************************/
#ifndef AWESOMEFAUCET_SCPI_H_
#define AWESOMEFAUCET_SCPI_H_

    #include <stdlib.h>                 // strtod, strtol etc.
    #include <process_USB.h>
	#include <scpi_parser.h>
    #include <bootloader.h>             // 'Jump_To_Bootloader
    #include "Awesomefaucet_info.h"     // Project namne, version, array size, etc.
    #include "Awesomefaucet_board.h"
    #include "Awesomefaucet_laser.h"
    #include "Awesomefaucet_water.h"
    #include "Awesomefaucet_sensor.h"
    #include "Awesomefaucet_timers.h"
/****************************************************************************
* SCPI Commands                                                             *
*****************************************************************************/
	uint8_t	 Setup_ScpiCommandsArray_P ( scpi_commands_P_t command_array_P[] );
	void st_OPC_q	( char *arg, IO_pointers_t IO );
	void sys_rst_btloader ( char *arg, IO_pointers_t IO );
	void sys_error_q ( char *arg, IO_pointers_t IO );
	void scpi_get_version_q(char*, IO_pointers_t IO);
	void scpi_IDN_q( char *arg, IO_pointers_t IO );
	void st_CLS ( char *arg, IO_pointers_t IO );
	void st_RST ( char *arg, IO_pointers_t IO );
	void st_TST ( char *arg, IO_pointers_t IO );
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
    void scpi_get_blackbox_q( char *arg, IO_pointers_t IO );
    void scpi_record( char *arg, IO_pointers_t IO );
    void scpi_get_max_distance_mm_reset_rate_q( char *arg, IO_pointers_t IO );
    void scpi_set_max_distance_mm_reset_rate( char *arg, IO_pointers_t IO );
    void retrieve_max_distance_mm_reset_rate(void);
    void scpi_get_range_and_maxrange_q(char *arg, IO_pointers_t IO);
/****************************************************************************
*  Compulsory SCPI commands                                                 *
*  see: http://www.ivifoundation.org/docs/scpi-99.pdf                       *
*****************************************************************************/
 #endif