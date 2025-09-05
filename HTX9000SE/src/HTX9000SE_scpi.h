/**************************************************************************
*                                                                         *
* SCPI Commands                                                           *
*                                                                         *
***************************************************************************/

#ifndef HTX9000SE_SCPI_H
#define HTX9000SE_SCPI_H

#include <stdlib.h> // Uses atof()
#include <scpi_parser.h>
#include <thermistor.h>
#include <bootloader.h>
#include "HTX9000SE_info.h"
#include "HTX9000SE_board.h"
#include "HTX9000SE_lcd_manager.h"
/**************************************************************************
* Define calibration string size                                          *
***************************************************************************/
#define CAL_STRING_SIZE     16
/**************************************************************************
* Define Step sizes load box setting                                      *
***************************************************************************/
#define STEPSIZE_HI         38.15e-6
#define STEPSIZE_LO         381.5e-9
/**************************************************************************
* Define Low Current Assist DAC Setting                                   *
***************************************************************************/
#define LOW_CURR_ASSIST_DACVAL 20
/**************************************************************************
* Define Step sizes load box setting                                      *
***************************************************************************/
#define DIE_TEMP_ADC_OFS    205
#define DIE_TEMP_ADC_SPAN   1
#define LOW_RANGE           0
#define HIGH_RANGE          1
#define DEGREE_BUBBLE       176
/**************************************************************************
* Fan Values                                                              *
***************************************************************************/
#define FAN_ON_TEMP         55
#define FAN_OFF_TEMP        45    
/**************************************************************************
* SPI check for done                                                      *
***************************************************************************/
#define SPI_READY           (SPSR & (1 << SPIF))
/**************************************************************************
* Create a data structure to contain the load box state                   *
***************************************************************************/
typedef struct
    {
        uint8_t     range;                                      // Keep track of the load box range
        uint8_t     dropout;                                    // State of the dropout detector pin
        uint16_t    dac_value;                                  // Current dac value
        uint16_t    upper_range_dac_cal_value;                  // Upper range DAC value during Cal
        uint16_t    lower_range_dac_cal_value;                  // Lower range DAC value during Cal
        uint16_t    checksum_lo;                                // Lower range calibration checksum
        uint16_t    checksum_hi;                                // Upper range calibration checksum
        uint8_t     touchpad_locked;                            // Touchpad lockout mechanism
        uint8_t     screen_up_to_date;                          // Used to elminate excessive overwriting
        char        upper_range_cal_string[CAL_STRING_SIZE];    // Upper range Cal string
        char        lower_range_cal_string[CAL_STRING_SIZE];    // Lower range Cal string
        char        cal_date_string[CAL_STRING_SIZE];           // Calibration Date
    }   settings_t;

typedef struct
    {
        double die_temp;
        double ntc_temp;
    }   temperatures_t;

void        setup_scpi_commands(SCPI_Node_t **command_array);
void        scpi_store_cal_date(char *arg, IO_pointers_t IO);
void        scpi_store_cal_val_hi_range(char *arg, IO_pointers_t IO);
void        scpi_store_cal_val_lo_range(char *arg, IO_pointers_t IO);
void        scpi_store_cal_dac_hi_range(char *arg, IO_pointers_t IO);
void        scpi_store_cal_dac_lo_range(char *arg, IO_pointers_t IO);
void        scpi_read_cal_date_q(char *arg, IO_pointers_t IO);
void        scpi_read_cal_val_hi_range_q(char *arg, IO_pointers_t IO);
void        scpi_read_cal_val_lo_range_q(char *arg, IO_pointers_t IO);
void        scpi_read_cal_dac_hi_range_q(char *arg, IO_pointers_t IO);
void        scpi_read_cal_dac_lo_range_q(char *arg, IO_pointers_t IO);
void        scpi_read_checksum_high_q(char *arg, IO_pointers_t IO);
void        scpi_read_checksum_low_q (char *arg, IO_pointers_t IO);
void        scpi_store_board_rev(char *arg, IO_pointers_t IO);
void        scpi_store_crnt_trig(char *arg, IO_pointers_t IO);
void        scpi_store_crnt_low_trig(char *arg, IO_pointers_t IO);
void        scpi_store_crnt_high_trig(char *arg, IO_pointers_t IO);
double      scpi_parse_current_arg(char *arg);
void        scpi_set_crnt_auto(char *arg, IO_pointers_t IO);
void        scpi_set_crnt_low(char *arg, IO_pointers_t IO);
void        scpi_set_crnt_high(char *arg, IO_pointers_t IO);
void        toggle_range(IO_pointers_t IO);
void        scpi_set_range(uint8_t range, IO_pointers_t IO);
void        scpi_get_current_q(char *arg, IO_pointers_t IO);
void        scpi_set_dac(uint16_t dac_setting);
void        set_current_from_keypad(char* value, IO_pointers_t IO);
double      get_current_value(IO_pointers_t IO);
uint8_t     get_range(void);
void        st_IDN_q(char *arg, IO_pointers_t IO);
void        trig (char *arg, IO_pointers_t IO);
void        update_LCD_current (double current, IO_pointers_t IO);
void        update_LCD_statics(IO_pointers_t IO);
void        update_LCD_temperature(void);
void        scpi_get_heatsink_temp_q(char *arg, IO_pointers_t IO);
void        scpi_get_board_temp_q(char *arg, IO_pointers_t IO);
void        get_temperatures(void);
void        scpi_get_dropout_state_q(char *arg, IO_pointers_t IO);
void        scpi_lock_touchpad(char *arg, IO_pointers_t IO);
void        scpi_unlock_touchpad(char *arg, IO_pointers_t IO);
void        scpi_get_range_q(char *arg, IO_pointers_t IO); 
void        scpi_enable_fan(char *arg, IO_pointers_t IO);
void        scpi_disable_fan(char *arg, IO_pointers_t IO);
void        scpi_get_fan_state(char *arg, IO_pointers_t IO);
double      Convert_Dietemp_To_DEGC(uint16_t NTC_ADC_Val);
uint16_t    get_checksum(char *cal_date_string, char *val_cal_string, uint16_t dac_cal_value);
void        update_screen(IO_pointers_t IO);
void        st_OPC_q(char *arg, IO_pointers_t IO);
void        sys_rst_btloader(char *arg, IO_pointers_t IO);
void        scpi_get_version_q(char*, IO_pointers_t IO);

 #endif