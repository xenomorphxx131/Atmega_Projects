#include "Loadbox_scpi.h"

// EEPROM Hardware Revision
uint8_t		EEMEM HARDWARE_REV[CAL_STRING_SIZE];

// EEPROM Calibration Date
uint8_t		EEMEM CAL_DATE[CAL_STRING_SIZE];

// EEPROM Calibration Data
uint16_t	EEMEM UPPER_RANGE_DAC_CAL_VALUE;
uint16_t	EEMEM LOWER_RANGE_DAC_CAL_VALUE;
uint8_t		EEMEM LOWER_RANGE_CAL_STRING[CAL_STRING_SIZE];
uint8_t		EEMEM UPPER_RANGE_CAL_STRING[CAL_STRING_SIZE];
uint16_t	EEMEM CALIBRATION_LO_CHECK_SUM;
uint16_t	EEMEM CALIBRATION_HI_CHECK_SUM;

char const error_message_lb_1[]	PROGMEM = "+10,\"ERR Date argument too long - max 15\"";
char const error_message_lb_2[]	PROGMEM = "+10,\"ERR Cal argument too long - max 15\"";

settings_t		settings =		{.range = LOW,
								 .dropout = YES,
								 .dac_value = 0,
								 .upper_range_dac_cal_value = 0,
								 .lower_range_dac_cal_value = 0,
								 .touchpad_locked = NO,
								 .screen_up_to_date = NO,
								 .checksum_lo = 0,
								 .checksum_hi = 0,
								 .upper_range_cal_string = "",
								 .lower_range_cal_string = "",
								 .cal_date_string = ""};
							 
double			current = 0;
bool            fan_off_override = 0;
							 
temperatures_t	temperatures =	{.ntc_temp = 21,
								 .die_temp = 21};

/**************************************************************************
*  Setup Loadbox Specific SCPI commands and functions                     *
***************************************************************************/
int Setup_ScpiCommandsArray_Loadbox_P( scpi_commands_P_t command_array_P[COMMAND_ARRAY_SIZE] )
{
	int i = 0;
	i = SetupScpiCommandsArray_P( command_array_P, i );
	
		// -1 [[:]SYSTem:]:LOCK
		command_array_P[i].name      = PSTR("LOCK");
		command_array_P[i].implied    = NO;
		command_array_P[i].parent     = &command_array_P[2]; // ***** Careful here, this crosses back to "scpi.c"
		command_array_P[i++].function = &scpi_lock_touchpad;

			// 0 [[:]SYSTem:]:LOCK:RELease
			command_array_P[i].name      = PSTR("RELease");
			command_array_P[i].implied    = NO;
			command_array_P[i].parent     = &command_array_P[i-1];
			command_array_P[i++].function = &scpi_unlock_touchpad;

	// 1 [[:]SOURce:]
 	command_array_P[i].name      = PSTR("SOURce");
	command_array_P[i].implied    = YES;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &scpi_empty_func;

 		// 2 [[:]SOURce:]CURRent <value>
		command_array_P[i].name      = PSTR("CURRent");
		command_array_P[i].implied    = NO;
		command_array_P[i].parent     = &command_array_P[i-1];
		command_array_P[i++].function = &scpi_set_crnt_auto;

			// 3 [[:]SOURce:]CURRent:]RANGe
 			command_array_P[i].name      = PSTR("RANGe");
			command_array_P[i].implied    = NO;
			command_array_P[i].parent     = &command_array_P[i-1];
			command_array_P[i++].function = &scpi_empty_func;
			
			// 4 [[:]SOURce:]CURRent:]RANGe
 			command_array_P[i].name      = PSTR("RANGe?");
			command_array_P[i].implied    = NO;
			command_array_P[i].parent     = &command_array_P[i-2];
			command_array_P[i++].function = &scpi_get_range_q;
	
				// 5 [[:]SOURce:]CURRent:]RANGe:]LOw <value>
				command_array_P[i].name      = PSTR("LOw");
				command_array_P[i].implied    = NO;
				command_array_P[i].parent     = &command_array_P[i-2];
				command_array_P[i++].function = &scpi_set_crnt_low;
				
				// 6 [[:]SOURce:]CURRent:]RANGe:]HIgh <value>
				command_array_P[i].name      = PSTR("HIgh");
				command_array_P[i].implied    = NO;
				command_array_P[i].parent     = &command_array_P[i-3];
				command_array_P[i++].function = &scpi_set_crnt_high;
				
		// 7 [[:]SOURce:]CURRent?
		command_array_P[i].name      = PSTR("CURRent?");
		command_array_P[i].implied    = NO;
		command_array_P[i].parent     = &command_array_P[i-6];
		command_array_P[i++].function = &scpi_current_q;
	
	// 8 [:]CALibrate
	command_array_P[i].name      = PSTR("CALibrate");
	command_array_P[i].implied    = NO;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &scpi_empty_func;
	
		// 9 [:]CALibrate:OUTPut
		command_array_P[i].name      = PSTR("OUTPut");
		command_array_P[i].implied    = NO;
		command_array_P[i].parent     = &command_array_P[i-1];
		command_array_P[i++].function = &scpi_empty_func;	
	
			// 10 [:]CALibrate:OUTPut:CURRent
			command_array_P[i].name      = PSTR("CURRent");
			command_array_P[i].implied    = NO;
			command_array_P[i].parent     = &command_array_P[i-1];
			command_array_P[i++].function = &scpi_empty_func;	
	
				// 11 [:]CALibrate:OUTPut:CURRent:VALue
				command_array_P[i].name      = PSTR("VALue");
				command_array_P[i].implied    = NO;
				command_array_P[i].parent     = &command_array_P[i-1];
				command_array_P[i++].function = &scpi_empty_func;
				
					// 12 [:]CALibrate:OUTPut:CURRent:VALue:RANGe <value>
					command_array_P[i].name      = PSTR("RANGe");
					command_array_P[i].implied    = NO;
					command_array_P[i].parent     = &command_array_P[i-1];
					command_array_P[i++].function = &scpi_empty_func;
				
						// 13 [:]CALibrate:OUTPut:CURRent:VALue:RANGe:HIgh <value>
						command_array_P[i].name      = PSTR("HIgh");
						command_array_P[i].implied    = NO;
						command_array_P[i].parent     = &command_array_P[i-1];
						command_array_P[i++].function = &scpi_store_cal_val_hi_range;
				
						// 14 [:]CALibrate:OUTPut:CURRent:VALUe:RANGe:HIgh?
						command_array_P[i].name      = PSTR("HIgh?");
						command_array_P[i].implied    = NO;
						command_array_P[i].parent     = &command_array_P[i-2];
						command_array_P[i++].function = &scpi_read_cal_val_hi_range_q;
		
						// 15 [:]CALibrate:OUTPut:CURRent:VALUe:RANGe:LOw <value>
						command_array_P[i].name      = PSTR("LOw");
						command_array_P[i].implied    = NO;
						command_array_P[i].parent     = &command_array_P[i-3];
						command_array_P[i++].function = &scpi_store_cal_val_lo_range;
				
						// 16 [:]CALibrate:OUTPut:CURRent:VALUe:RANGe:LOw?
						command_array_P[i].name      = PSTR("LOw?");
						command_array_P[i].implied    = NO;
						command_array_P[i].parent     = &command_array_P[i-4];
						command_array_P[i++].function = &scpi_read_cal_val_lo_range_q;

		// 17 [:]CALibrate:DAC
		command_array_P[i].name      = PSTR("DAC");
		command_array_P[i].implied    = NO;
		command_array_P[i].parent     = &command_array_P[i-7];
		command_array_P[i++].function = &scpi_empty_func;
				
			// 18 [:]CALibration:DAC:RANGe <value>
			command_array_P[i].name      = PSTR("RANGe");
			command_array_P[i].implied    = NO;
			command_array_P[i].parent     = &command_array_P[i-1];
			command_array_P[i++].function = &scpi_empty_func;
				
				// 19 [:]CALibration:DAC:RANGe:HIgh <value>
				command_array_P[i].name      = PSTR("HIgh");
				command_array_P[i].implied    = NO;
				command_array_P[i].parent     = &command_array_P[i-1];
				command_array_P[i++].function = &scpi_store_cal_dac_hi_range;
				
				// 20 [:]CALibration:DAC:RANGE:HIgh?
				command_array_P[i].name      = PSTR("HIgh?");
				command_array_P[i].implied    = NO;
				command_array_P[i].parent     = &command_array_P[i-2];
				command_array_P[i++].function = &scpi_read_cal_dac_hi_range_q;
		
				// 21 [:]CALibration:DAC:RANGe:LOw <value>
				command_array_P[i].name      = PSTR("LOw");
				command_array_P[i].implied    = NO;
				command_array_P[i].parent     = &command_array_P[i-3];
				command_array_P[i++].function = &scpi_store_cal_dac_lo_range;
				
				// 22 [:]CALibration:DAC:RANGe:LOw?
				command_array_P[i].name      = PSTR("LOw?");
				command_array_P[i].implied    = NO;
				command_array_P[i].parent     = &command_array_P[i-4];
				command_array_P[i++].function = &scpi_read_cal_dac_lo_range_q;
  
		// 23 [:]CALibrate:DATE <value>
		command_array_P[i].name      = PSTR("DATE");
		command_array_P[i].implied    = NO;
		command_array_P[i].parent     = &command_array_P[i-15];
		command_array_P[i++].function = &scpi_store_cal_date;
		
		// 24 [:]CALibrate:DATE?
		command_array_P[i].name      = PSTR("DATE?");
		command_array_P[i].implied    = NO;
		command_array_P[i].parent     = &command_array_P[i-16];
		command_array_P[i++].function = &scpi_read_cal_date_q;
		
		// 25 [:]CALibrate:CHECKsum
		command_array_P[i].name      = PSTR("CHECKsum");
		command_array_P[i].implied    = NO;
		command_array_P[i].parent     = &command_array_P[i-17];
		command_array_P[i++].function = &scpi_empty_func;
		
			// 26 [:]CALibrate:CHECKsum:HIgh?
			command_array_P[i].name      = PSTR("HIgh?");
			command_array_P[i].implied    = NO;
			command_array_P[i].parent     = &command_array_P[i-1];
			command_array_P[i++].function = &scpi_read_checksum_high_q;
		
			// 27 [:]CALibrate:CHECKsum:LOw?
			command_array_P[i].name      = PSTR("LOw?");
			command_array_P[i].implied    = NO;
			command_array_P[i].parent     = &command_array_P[i-2];
			command_array_P[i++].function = &scpi_read_checksum_low_q;
		
		// 28 [:]CALibrate:BOArdrev <value>
		command_array_P[i].name      = PSTR("BOArdrev");
		command_array_P[i].implied    = NO;
		command_array_P[i].parent     = &command_array_P[i-20];
		command_array_P[i++].function = &scpi_store_board_rev;

	// 29 [:]*IDN?
	command_array_P[i].name      = PSTR("*IDN?");
	command_array_P[i].implied    = NO;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &st_IDN_q;
	
	// 30 [:]TEMP
	command_array_P[i].name      = PSTR("TEMP");
	command_array_P[i].implied    = NO;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &scpi_empty_func;
	
		// 31 [:]TEMP:HEATsink?
		command_array_P[i].name      = PSTR("HEATsink?");
		command_array_P[i].implied    = NO;
		command_array_P[i].parent     = &command_array_P[i-1];
		command_array_P[i++].function = &scpi_get_heatsink_temp_q;
		
		// 32 [:]TEMP:BOARD?
		command_array_P[i].name      = PSTR("BOARD?");
		command_array_P[i].implied    = NO;
		command_array_P[i].parent     = &command_array_P[i-2];
		command_array_P[i++].function = &scpi_get_board_temp_q;
	
	// 33 [:]DROPout?
	command_array_P[i].name      = PSTR("DROPout?");
	command_array_P[i].implied    = NO;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &scpi_get_dropout_state_q;
    
	// 34 [:]FAN
	command_array_P[i].name      = PSTR("FAN");
	command_array_P[i].implied    = NO;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &scpi_empty_func;
    
        // 35 [:]ON
        command_array_P[i].name      = PSTR("ON");
        command_array_P[i].implied    = NO;
        command_array_P[i].parent     = &command_array_P[i-1];
        command_array_P[i++].function = &scpi_enable_fan;
        
        // 36 [:]OFF
        command_array_P[i].name      = PSTR("OFF");
        command_array_P[i].implied    = NO;
        command_array_P[i].parent     = &command_array_P[i-2];
        command_array_P[i++].function = &scpi_disable_fan;
        
	// 37 [:]FAN?
	command_array_P[i].name      = PSTR("FAN?");
	command_array_P[i].implied    = NO;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &scpi_get_fan_state;
	
	return i; // This is incremented so it matches "COMMAND_ARRAY_SIZE"
		
  // TODO need to add TRIG:RAMP
  // TODO Need to add *TRG
  // TODO Need to add RAMP <value> <time>
  // TODO Need to add MENU?
}
/**************************************************************************
*  Save Calibration date to EEPROM                                        *
***************************************************************************/
void scpi_store_cal_date( char *arg, IO_pointers_t IO )
{
	if (strlen(arg) <= MAX_ARG_LEN)
		{
		eeprom_busy_wait();
		eeprom_write_block(arg, &CAL_DATE, CAL_STRING_SIZE);
		}
	else
		scpi_add_error_P(error_message_lb_1, IO);
}
/**************************************************************************
*  Save Board Revision data to EEPROM                                     *
***************************************************************************/
void scpi_store_board_rev( char *arg, IO_pointers_t IO )
{
	if (strlen(arg) <= MAX_ARG_LEN)
		{
		eeprom_busy_wait();
		eeprom_write_block(arg, &HARDWARE_REV, CAL_STRING_SIZE);
		}
	else
		scpi_add_error_P(error_message_lb_2, IO);
}
/**************************************************************************
*  Retrieve calibration data from EEPROM                                  *
***************************************************************************/
void scpi_read_cal_data(IO_pointers_t IO)
{
		eeprom_busy_wait();
		eeprom_read_block((void*)&settings.cal_date_string, (const void *)&CAL_DATE, CAL_STRING_SIZE);
		eeprom_busy_wait();
		eeprom_read_block(&settings.upper_range_cal_string, &UPPER_RANGE_CAL_STRING, CAL_STRING_SIZE);
		eeprom_busy_wait();
		settings.upper_range_dac_cal_value = eeprom_read_word (&UPPER_RANGE_DAC_CAL_VALUE);
		eeprom_busy_wait();
		eeprom_read_block(&settings.lower_range_cal_string, &LOWER_RANGE_CAL_STRING, CAL_STRING_SIZE);
		eeprom_busy_wait();
		settings.lower_range_dac_cal_value = eeprom_read_word (&LOWER_RANGE_DAC_CAL_VALUE);
		eeprom_busy_wait();
		settings.checksum_lo = eeprom_read_word (&CALIBRATION_LO_CHECK_SUM);
		eeprom_busy_wait();
		settings.checksum_hi = eeprom_read_word (&CALIBRATION_HI_CHECK_SUM);
}
/**************************************************************************
*  Store low range DAC value and calibration value to EEPROM              *
***************************************************************************/
void scpi_store_cal_val_lo_range( char *arg, IO_pointers_t IO )
{
	uint16_t checksum;
	if (strlen(arg) <= MAX_ARG_LEN)
	{
		remove_ws(arg);
		eeprom_busy_wait();
		eeprom_write_block(arg, &LOWER_RANGE_CAL_STRING, CAL_STRING_SIZE);
		eeprom_busy_wait();
		eeprom_write_word (&LOWER_RANGE_DAC_CAL_VALUE, settings.dac_value);
		checksum = get_checksum(settings.cal_date_string, arg, settings.dac_value);
		eeprom_write_word (&CALIBRATION_LO_CHECK_SUM, checksum);
	}
	else
		scpi_add_error_P(error_message_lb_2, IO);
	settings.touchpad_locked = NO;
	settings.screen_up_to_date = NO;
}
/**************************************************************************
*  Store high range DAC value and calibration value to EEPROM             *
***************************************************************************/
void scpi_store_cal_val_hi_range( char *arg, IO_pointers_t IO )
{	
	uint16_t checksum;
	if (strlen(arg) <= MAX_ARG_LEN)
	{
		remove_ws(arg);
		eeprom_busy_wait();
		eeprom_write_block(arg, &UPPER_RANGE_CAL_STRING, CAL_STRING_SIZE);
		eeprom_busy_wait();
		eeprom_write_word (&UPPER_RANGE_DAC_CAL_VALUE, settings.dac_value);
		checksum = get_checksum(settings.cal_date_string, arg, settings.dac_value);
		eeprom_write_word (&CALIBRATION_HI_CHECK_SUM, checksum);
	}
	else
		scpi_add_error_P(error_message_lb_2, IO);
	settings.touchpad_locked = NO;
	settings.screen_up_to_date = NO;
}
/**************************************************************************
*  Retrieve Cal Date value from EEPROM                                    *
***************************************************************************/
void scpi_read_cal_date_q( char *arg, IO_pointers_t IO )
{	
	scpi_read_cal_data(IO);
	fprintf(IO.USB_stream, "%s\n", settings.cal_date_string);
}
/**************************************************************************
*  Retrieve high range calibration value from EEPROM                      *
***************************************************************************/
void scpi_read_cal_val_hi_range_q( char *arg, IO_pointers_t IO )
{	
	scpi_read_cal_data(IO);
	fprintf(IO.USB_stream, "%s\n", settings.upper_range_cal_string);
}
/**************************************************************************
*  Retrieve low range calibration value from EEPROM                       *
***************************************************************************/
void scpi_read_cal_val_lo_range_q( char *arg, IO_pointers_t IO )
{	
	scpi_read_cal_data(IO);
	fprintf(IO.USB_stream, "%s\n", settings.lower_range_cal_string);
}
/**************************************************************************
*  Retrieve high range calibration dac setting from EEPROM                *
***************************************************************************/
void scpi_read_cal_dac_hi_range_q( char *arg, IO_pointers_t IO )
{	
	scpi_read_cal_data(IO);
	fprintf(IO.USB_stream, "%X\n", settings.upper_range_dac_cal_value);
}
/**************************************************************************
*  Retrieve low range calibration dac setting from EEPROM                 *
***************************************************************************/
void scpi_read_cal_dac_lo_range_q( char *arg, IO_pointers_t IO )
{	
	scpi_read_cal_data(IO);
	fprintf(IO.USB_stream, "%X\n", settings.lower_range_dac_cal_value);
}
/**************************************************************************
*  Retrieve high range calibration checksum from EEPROM                   *
***************************************************************************/
void scpi_read_checksum_high_q( char *arg, IO_pointers_t IO )
{
	scpi_read_cal_data(IO);
	if (settings.checksum_hi == get_checksum(	settings.cal_date_string,
												settings.upper_range_cal_string,
												settings.upper_range_dac_cal_value))
		fprintf(IO.USB_stream, "1\n");
	else
		fprintf(IO.USB_stream, "0\n");						
}
/**************************************************************************
*  Retrieve low range calibration checksum from EEPROM                    *
***************************************************************************/
void scpi_read_checksum_low_q( char *arg, IO_pointers_t IO )
{
	scpi_read_cal_data(IO);
	if (settings.checksum_lo == get_checksum(	settings.cal_date_string,
												settings.lower_range_cal_string,
												settings.lower_range_dac_cal_value))
		fprintf(IO.USB_stream, "1\n");
	else
		fprintf(IO.USB_stream, "0\n");
}
/**************************************************************************
* CALibrate:DAC:RANGe:HIgh <value> Direct access to the dac in low range  *
***************************************************************************/
void scpi_store_cal_dac_hi_range( char *arg, IO_pointers_t IO )
{
	settings.touchpad_locked = YES;
	scpi_set_range(HIGH, IO);
	settings.dac_value = atof(arg);
	scpi_set_dac(settings.dac_value, IO.I2C_port1);
}
/**************************************************************************
* CALibrate:DAC:RANGe:LOw <value> Direct access to the dac in low range   *
***************************************************************************/
void scpi_store_cal_dac_lo_range( char *arg, IO_pointers_t IO )
{
	settings.touchpad_locked = YES;
	scpi_set_range(LOW, IO);
	settings.dac_value = atof(arg);
	scpi_set_dac(settings.dac_value, IO.I2C_port1);
}
/**************************************************************************
*  Set the Current in Auto Range mode                                     *
***************************************************************************/
void scpi_set_crnt_auto( char *arg, IO_pointers_t IO )
{
	double requested_value;
	double parsed_cal_value_lo;
	double parsed_cal_value_hi;
	double stepsize_lo;
	double stepsize_hi;

	scpi_read_cal_data(IO);
	requested_value = scpi_parse_current_arg(arg);
	parsed_cal_value_lo = scpi_parse_current_arg(settings.lower_range_cal_string);
	parsed_cal_value_hi = scpi_parse_current_arg(settings.upper_range_cal_string);
	stepsize_lo = parsed_cal_value_lo / settings.lower_range_dac_cal_value;
	stepsize_hi = parsed_cal_value_hi / settings.upper_range_dac_cal_value;
	
	if (settings.range == LOW && (requested_value + stepsize_lo / 2) * settings.lower_range_dac_cal_value  / parsed_cal_value_lo > 0xFFFF)
		scpi_set_range(HIGH, IO);				// Only switch ranges up if necessary otherwise keep old range
	
//	if (settings.range == HIGH && (requested_value * settings.upper_range_dac_cal_value) / parsed_cal_value_hi < 1)
	if (settings.range == HIGH && (requested_value * settings.upper_range_dac_cal_value) / parsed_cal_value_hi < 65635 / parsed_cal_value_hi * parsed_cal_value_lo)
		scpi_set_range(LOW, IO);				// Only switch ranges down if necessary otherwise keep old range

	if (settings.range == LOW)
	{	if ((requested_value + stepsize_lo / 2) / parsed_cal_value_lo * settings.lower_range_dac_cal_value <= 0xFFFF)
			settings.dac_value = (requested_value + stepsize_lo / 2) / parsed_cal_value_lo * settings.lower_range_dac_cal_value;
		else
			settings.dac_value = 0xFFFF;
	}
	else
	{	if ((requested_value + stepsize_hi / 2) / parsed_cal_value_hi * settings.upper_range_dac_cal_value <= 0xFFFF)
			settings.dac_value = (requested_value + stepsize_hi / 2) / parsed_cal_value_hi * settings.upper_range_dac_cal_value;
		else
			settings.dac_value = 0xFFFF;
	}
	
	scpi_set_dac(settings.dac_value, IO.I2C_port1);
	// re-normalize for LCD display with dac resolution and clipped value
	if (settings.range == LOW)
		requested_value = settings.dac_value * parsed_cal_value_lo / settings.lower_range_dac_cal_value;
	else
		requested_value = settings.dac_value * parsed_cal_value_hi / settings.upper_range_dac_cal_value;

	draw_status_screen(requested_value, temperatures.ntc_temp, settings.touchpad_locked, settings.range, IO);
}
/**************************************************************************
*  Set the Current Low Range	                                          *
***************************************************************************/
void scpi_set_crnt_low( char *arg, IO_pointers_t IO )
{
	double requested_value;
	double parsed_cal_value;
	
	scpi_read_cal_data(IO);
	scpi_set_range( LOW, IO );
	requested_value = scpi_parse_current_arg(arg);
	parsed_cal_value = scpi_parse_current_arg(settings.lower_range_cal_string);
	if ((requested_value + STEPSIZE_LO / 2) / parsed_cal_value * settings.lower_range_dac_cal_value <= 0xFFFF)
		settings.dac_value = (requested_value + STEPSIZE_LO / 2) / parsed_cal_value * settings.lower_range_dac_cal_value;
	else
		settings.dac_value = 0xFFFF;
		
	scpi_set_dac(settings.dac_value, IO.I2C_port1);
	// re-normalize for LCD display with dac resolution and clipped value
	requested_value = settings.dac_value * parsed_cal_value / settings.lower_range_dac_cal_value;
	draw_status_screen(requested_value, temperatures.ntc_temp, settings.touchpad_locked, settings.range, IO);
}
/**************************************************************************
*  Set the Current High	Range                                             *
***************************************************************************/
void scpi_set_crnt_high( char *arg, IO_pointers_t IO )
{
	double requested_value;
	double parsed_cal_value;

	scpi_read_cal_data(IO);
	scpi_set_range( HIGH, IO );

	requested_value = scpi_parse_current_arg(arg);
	parsed_cal_value = scpi_parse_current_arg(settings.upper_range_cal_string);
	if ((requested_value + STEPSIZE_HI / 2 ) / parsed_cal_value * settings.upper_range_dac_cal_value <= 0xFFFF)
		settings.dac_value = (requested_value + STEPSIZE_HI / 2 ) / parsed_cal_value * settings.upper_range_dac_cal_value;
	else
		settings.dac_value = 0xFFFF;
		
	scpi_set_dac(settings.dac_value, IO.I2C_port1);
	// re-normalize for LCD display with dac resolution and clipped value
	requested_value = settings.dac_value * parsed_cal_value / settings.upper_range_dac_cal_value;
	// update_LCD_current( requested_value, IO );
    draw_status_screen(requested_value, temperatures.ntc_temp, settings.touchpad_locked, settings.range, IO);
}
/**************************************************************************
*  Toggle the range from the keypad                                       *
***************************************************************************/
void toggle_range(IO_pointers_t IO)
{
    settings.range = settings.range == HIGH ? LOW : HIGH;
    scpi_set_range( settings.range, IO);
    update_screen(IO);
}
/**************************************************************************
*  Set the Current Range to Low or High	                                  *
***************************************************************************/
void scpi_set_range( uint8_t range, IO_pointers_t IO )
{
	if  (range == HIGH)
	{	settings.range = HIGH;
		scpi_set_dac(0, IO.I2C_port1);	// Set the dac to zero before switching
		I2P5A_PORT |=  I2P5A;			// To prevent load dump
	}
	else
	{	settings.range = LOW;
		scpi_set_dac(0, IO.I2C_port1);	// Set the dac to zero before switching
		I2P5A_PORT &= ~I2P5A;			// To prevent load dump
	}
}
/**************************************************************************
*  Set the Current Range to Low or High	                                  *
***************************************************************************/
void scpi_current_q( char *arg, IO_pointers_t IO )
{
	if (settings.range == LOW)
		fprintf(IO.USB_stream, "%6.4e\n", get_current_value(IO));
	else
		fprintf(IO.USB_stream, "%6.5e\n", get_current_value(IO));
}
/**************************************************************************
*  Set the Current Range to Low or High	                                  *
***************************************************************************/
double get_current_value( IO_pointers_t IO )
{
	double parsed_cal_value_lo;
	double parsed_cal_value_hi;
	scpi_read_cal_data(IO);
	parsed_cal_value_lo = scpi_parse_current_arg(settings.lower_range_cal_string);
	parsed_cal_value_hi = scpi_parse_current_arg(settings.upper_range_cal_string);
	
	if (settings.range == LOW)
		return settings.dac_value * parsed_cal_value_lo / settings.lower_range_dac_cal_value;
	else
		return settings.dac_value * parsed_cal_value_hi / settings.upper_range_dac_cal_value;
}
/****************************************************************************
*    Output the D/A Value to the D/A                                        *
*****************************************************************************/
void scpi_set_dac(uint16_t dac_setting, i2c_port_t *i2c_port)
{
	DACPORT &= ~LDAC;									// Load line low to receive data
    SPDR = (uint8_t)(dac_setting >> 8);
    while (!SPI_READY)
        ;                                               // Wait for transaction to complete
    SPDR = (uint8_t)(dac_setting);
    while (!SPI_READY)
        ;                                               // Wait for transaction to complete
	DACPORT |= LDAC;									// Load high update the D/A data
	
	if (dac_setting == 0)
	{
		ZERO_CURRENT_PORT   |= ZERO_CURRENT;				// Set the Zero Current port pin
		LOW_CUR_ASSIST_PORT	|= LOW_CUR_ASSIST;			// Set the Low Current assist pin
	}
	else if (settings.dac_value <= LOW_CURR_ASSIST_DACVAL)
	{
		ZERO_CURRENT_PORT   &= ~ZERO_CURRENT;				// Clear the Zero Current port pin
		LOW_CUR_ASSIST_PORT	|= LOW_CUR_ASSIST;			// Set the Low Current Assist port pin
	}
	else
	{
		ZERO_CURRENT_PORT   &= ~ZERO_CURRENT;				// Clear the Zero Current port pin
		LOW_CUR_ASSIST_PORT	&= ~LOW_CUR_ASSIST;			// Clear the Low Current Assist port pin
	}
}
/****************************************************************************
*    Set the current from the key pad                                       *
*****************************************************************************/
void set_current_from_keypad( char* value, IO_pointers_t IO )
{
	if (!settings.touchpad_locked)
		scpi_set_crnt_auto(value, IO);
}
/****************************************************************************
*   Retrieve the D/A Value                                                  *
*****************************************************************************/
// uint16_t set_knob_from_dac()
// {
	// return settings.dac_value;
// }
/****************************************************************************
*   Retrieve the loadbox range                                              *
*****************************************************************************/
uint8_t get_range()
{
	return settings.range;
}
/**************************************************************************
*  *IDN? function                                                         *
***************************************************************************/
void st_IDN_q( char *arg, IO_pointers_t IO )
{
	char hardware_rev[CAL_STRING_SIZE] = "";
	eeprom_read_block(&hardware_rev, &HARDWARE_REV, CAL_STRING_SIZE);
	scpi_prStr_P(PSTR(COMPANY_NAME), IO.USB_stream);
	scpi_prStr_P(PSTR(PROJECT_NAME), IO.USB_stream);
	fprintf(IO.USB_stream, "%s", hardware_rev);
	scpi_prStr_P(PSTR(", "), IO.USB_stream);
	scpi_prStr_P(PSTR(FIRMWARE_VERSION), IO.USB_stream);
	scpi_prStr_P(PSTR("\n"), IO.USB_stream);
}
/**************************************************************************
*  my_remove_ws                                                           *
***************************************************************************/
void remove_ws( char *arg )
{
	int	ri, wi = 0;												//read index, write index
	for ( ri = 0 ; ri <= strlen(arg) ; ri++ )					// Walk string looking for whitespaces, <= preserves NUL char
		if ( !isspace(arg[ri]) ) arg[wi++] = arg[ri];			// Strip off leading and any additional whitespaces
}
/**************************************************************************
*  Parse argument from user                                               *
***************************************************************************/
double scpi_parse_current_arg( char *arg )
{
	char		units = NUL;									// Keeps track of the units if any
	uint16_t	strg_len = 0;									// Defined to ensure strlen only gets called once
	double		value;											// 1 sign 8 exponents and 23 fractions
	
	remove_ws (arg);											// Strip off leading and any additional white spaces
	strg_len = strlen(arg);										// Get initial string length

	if (arg[strg_len-1] == 'A')									// Check for an 'A'. Look at -1 because string starts at index 0.
	{	arg[strg_len-1] = NUL;									// Replace the 'A' with NUL character to end the string
		if (arg[strg_len-2] == 'N'		||						// Check for Nano
			arg[strg_len-2] == 'U'		||						// Check for micro
			arg[strg_len-2] == 'M')								// Check for Milli
		{	units = arg[strg_len-2];							// If a unit was found get the units.
			arg[strg_len-2] = NUL;								// Replace trailing [N|U|M] with NUL character to end the string
		}
	}
	else if (arg[strg_len-1] == 'N'		||						// Check for Nano
			 arg[strg_len-1] == 'U'		||						// Check for micro
			 arg[strg_len-1] == 'M')							// Check for Milli
		{	 units = arg[strg_len-1];							// If a unit was found get the units.
			 arg[strg_len-1] = NUL;								// Replace trailing [N|U|M] with NUL character to end the string
		}

	value = atof(arg);											// Send the remaining string to float
	if (units == 'N') value = value /1e9;						// If units were previously found to be nano then divide by 1e9
	if (units == 'U') value = value /1e6;						// If units were previously found to be micro then divide by 1e6
	if (units == 'M') value = value /1e3;						// If units were previously found to be milli then divide by 1e3
	// TODO Need to parse “AMPS” according to scpi
	return value;
}
/**************************************************************************
*  *TRG - Sets value previous programmed                                  *
***************************************************************************/
void trig( char *arg, IO_pointers_t IO )
{

} // END scpi_set_crnt_low_trig()
/**************************************************************************
*  Save Calibration data to EEPROM or Read Calibration data from EEPROM   *
***************************************************************************/
void scpi_set_crnt_low_trig( char *arg, IO_pointers_t IO )
{

} // END scpi_set_crnt_low_trig()
/**************************************************************************
*  Save Calibration data to EEPROM or Read Calibration data from EEPROM   *
***************************************************************************/
void scpi_set_crnt_high_trig( char *arg, IO_pointers_t IO )
{

} // END scpi_set_crnt_high_trig()
/**************************************************************************
*  Save Calibration data to EEPROM or Read Calibration data from EEPROM   *
***************************************************************************/
void scpi_set_crnt_trig( char *arg, IO_pointers_t IO )
{

}
/**************************************************************************
*  Get the thermistor A/D reading and die temp                            *
***************************************************************************/
void update_LCD_temperature()
{
	double ntc_old;
	ntc_old = temperatures.ntc_temp;
	get_temperatures();
	temperatures.ntc_temp = 0.95 * ntc_old + 0.05 * temperatures.ntc_temp;
    if (temperatures.ntc_temp > FAN_ON_TEMP)
        FAN_PORT |= FAN;
    else if ((temperatures.ntc_temp < FAN_OFF_TEMP) && !fan_off_override)
        FAN_PORT &= ~FAN;
}
/**************************************************************************
*  Report the Heat Sink A/D Reading                                       *
***************************************************************************/
void scpi_get_heatsink_temp_q( char *arg, IO_pointers_t IO )
{
	get_temperatures();
	fprintf(IO.USB_stream, "%7.3f\n", temperatures.ntc_temp);
}
/**************************************************************************
*  Report the PC Board Temperature A/D reading                            *
***************************************************************************/
void scpi_get_board_temp_q( char *arg, IO_pointers_t IO )
{
	get_temperatures();
	fprintf(IO.USB_stream, "%7.3f\n", temperatures.die_temp);
}
/****************************************************************************
*  Get data together and update screen                                      *
*****************************************************************************/
void update_screen(IO_pointers_t IO)
{
    update_LCD_temperature();
    draw_status_screen(get_current_value(IO), temperatures.ntc_temp, settings.touchpad_locked, settings.range, IO);
}
/****************************************************************************
*  Get the thermistor A/D reading and die temp                              *
*****************************************************************************/
void get_temperatures(void)
{
	double die_temp_new;
	
	ADMUX		 = 	AVCC_REF		|				// Set to supply as reference for thermistor.
					(1 << ADLAR)	|				// Set ADC reading to left justified.
					NTC_ADC_INPUTA;					// Set the MUX[4:0] to point to the thermistor single ended (still needs MUX[5] on ADCSRB).
	ADCSRB		 =	NTC_ADC_INPUTB;					// makes up the rest of the MUX[5:0] value.
	for (int i = 1; i <= 2; i++)					// Take two reading to ensure reference is settled
	{
		ADCSRA		|= (1 << ADSC);					// Start Conversion
		while (ADCSRA & (1 << ADSC));				// Conversion done when ADSC goes low again. Loop until conversion complete.
	}
	temperatures.ntc_temp
		= Convert_NTC_To_DEGC(ADCL | (ADCH << 8));	// Ensure low byte first to guarantee data coherence (see manual).

	ADMUX		 = 	INTERNAL_REF	|				// Set to external internal ref for die temp.
					(0 << ADLAR)	|				// Leave ADC reading right justified.
					DIETEMP_ADC_INPUTA;				// Set the MUX[4:0] to point to the on board temp sensor single ended (still needs MUX[5] on ADCSRB).
	ADCSRB		 =	DIETEMP_ADC_INPUTB;				// makes up the rest of the MUX[5:0] value.
	for (int i = 1; i <= 2; i++)					// Take two reading to ensure reference is settled
	{
		ADCSRA 		|= (1 << ADSC);					// Start Conversion
		while (ADCSRA & (1 << ADSC));				// Conversion done when ADSC goes low again. Loop until conversion complete.
	}

	die_temp_new
	= Convert_Dietemp_To_DEGC(ADCL | (ADCH << 8));	// Ensure low byte first to guarantee data coherence (see manual).
	temperatures.die_temp
	= temperatures.die_temp * 0/8
	+ die_temp_new * 8/8;							// 7/8 old + 1/8 new IIR filter
}
/**************************************************************************
*       Convert Die Temperature ADC Reading to °C                         *
***************************************************************************/
double Convert_Dietemp_To_DEGC( uint16_t DIE_ADC_Val )
{
	return (DIE_ADC_Val - DIE_TEMP_ADC_OFS) * DIE_TEMP_ADC_SPAN;
}
/**************************************************************************
*  Get State of the Dropout Detector                                      *
***************************************************************************/
void scpi_get_dropout_state_q( char *arg, IO_pointers_t IO )
{
settings.dropout = DROPOUT_DTECTR_PIN & DROPOUT_DTECTR;
if (settings.dropout)
	scpi_prStr_P(PSTR("1\n"), IO.USB_stream);
else
	scpi_prStr_P(PSTR("0\n"), IO.USB_stream);
}
/**************************************************************************
*  Get State of the Loadbox Range                                         *
***************************************************************************/
void scpi_get_range_q( char *arg, IO_pointers_t IO )
{
if (settings.range)
	scpi_prStr_P(PSTR("1\n"), IO.USB_stream);
else
	scpi_prStr_P(PSTR("0\n"), IO.USB_stream);
}
/**************************************************************************
*  Lock out the Touch Pad                                                 *
***************************************************************************/
void scpi_lock_touchpad( char *arg, IO_pointers_t IO )
{
	settings.touchpad_locked = YES;
    draw_status_screen(get_current_value(IO), temperatures.ntc_temp, settings.touchpad_locked, settings.range, IO);
}
/**************************************************************************
*  Unlock the Touch Pad                                                   *
***************************************************************************/
void scpi_unlock_touchpad( char *arg, IO_pointers_t IO )
{
	settings.touchpad_locked = NO;
    draw_status_screen(get_current_value(IO), temperatures.ntc_temp, settings.touchpad_locked, settings.range, IO);
}
/**************************************************************************
*  Enable the Fan                                                         *
***************************************************************************/
void scpi_enable_fan( char *arg, IO_pointers_t IO )
{
    fan_off_override = 1;
    FAN_PORT |= FAN;
}
/**************************************************************************
*  Disable the Fan                                                        *
***************************************************************************/
void scpi_disable_fan( char *arg, IO_pointers_t IO )
{
    FAN_PORT &= ~FAN;
    fan_off_override = 0;  // Added 4/9/19 by FL. Re-enable automatic fan shutoff.
}
/**************************************************************************
*  Get the Fan Status                                                     *
***************************************************************************/
void scpi_get_fan_state( char *arg, IO_pointers_t IO )
{
    if (FAN_PIN & FAN)
        scpi_prStr_P(PSTR("1\n"), IO.USB_stream);
    else
        scpi_prStr_P(PSTR("0\n"), IO.USB_stream);
}
/**************************************************************************
*  Compute a Checksum                                                     *
***************************************************************************/
uint16_t get_checksum( char * cal_date_string, char * val_cal_string, uint16_t dac_cal_value )
{
	uint16_t index = 0;
	uint16_t checksum = 0;
	
	while (cal_date_string[index] != NUL)
		checksum += (uint8_t)cal_date_string[index++];

	index = 0;
	while (val_cal_string[index] != NUL)
		checksum += (uint8_t)val_cal_string[index++];
		
	checksum += dac_cal_value;

	return checksum;
}















