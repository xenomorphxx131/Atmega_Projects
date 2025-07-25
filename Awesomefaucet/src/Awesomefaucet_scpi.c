/****************************************************************************
 *                                                                          *
 *  Awesomefaucet SCPI                                                      *
 *                                                                          *
 ****************************************************************************/

#include "Awesomefaucet_scpi.h"

/****************************************************************************
*  Error Message Buffers have scope within this file                        *
*****************************************************************************/
extern unsigned long debug_time_ms;
extern float distance_mm;
extern float max_distance_mm;
extern float threshold_mm;
extern uint8_t laser_power;
extern uint16_t water_debounce_timeout;
extern Blackbox blackbox[BLACKBOX_BUFFER_SIZE];
extern uint8_t blackbox_index;
extern bool record;

extern char bad_command[];
extern PGM_P error_messages[];
extern int error_number;
extern char const error_arg_too_long[];

uint8_t EEMEM LASER_POWER[sizeof(uint8_t)];
float EEMEM IIR_ALPHA[sizeof(float)];
float EEMEM IIR_BETA[sizeof(float)];
float EEMEM THRESHOLD_MM[sizeof(float)];
float EEMEM MAX_DISTANCE_LEAKAGE[sizeof(float)];
float EEMEM MAX_DISTANCE_MM_RESET_RATE[sizeof(float)];
float EEMEM WATER_DEBOUNCE_TIMEOUT[sizeof(uint16_t)];
bool water_auto = true;
float max_distance_leakage;
float max_distance_mm_reset_rate;
float iir_alpha;
float iir_beta;
float iir_gain;
/****************************************************************************
*  Setup Awesomfaucet Specific SCPI commands and functions                  *
*****************************************************************************/
uint8_t Setup_ScpiCommandsArray_P( scpi_commands_P_t command_array_P[] )
{
	uint8_t i = 0;
	command_array_P[i].name         = PSTR("NULL");
	command_array_P[i].implied      = true;
	command_array_P[i].parent       = NULL;
	command_array_P[i++].function   = &scpi_null_func;

	command_array_P[i].name         = PSTR("*OPC?");
	command_array_P[i].implied      = false;
	command_array_P[i].parent       = &command_array_P[0];
	command_array_P[i++].function   = &st_OPC_q;

	command_array_P[i].name         = PSTR("SYSTem");
	command_array_P[i].implied      = true;
	command_array_P[i].parent       = &command_array_P[0];
	command_array_P[i++].function   = &scpi_null_func;

		command_array_P[i].name         = PSTR("RST");
		command_array_P[i].implied      = false;
		command_array_P[i].parent       = &command_array_P[i-1];
		command_array_P[i++].function   = &scpi_null_func;

			command_array_P[i].name         = PSTR("BTLOader");
			command_array_P[i].implied      = false;
			command_array_P[i].parent       = &command_array_P[i-1];
			command_array_P[i++].function   = &sys_rst_btloader;

		command_array_P[i].name         = PSTR("ERRor?");
		command_array_P[i].implied      = false;
		command_array_P[i].parent       = &command_array_P[i-3];
		command_array_P[i++].function   = &sys_error_q;

		command_array_P[i].name         = PSTR("VERSion?");
		command_array_P[i].implied      = false;
		command_array_P[i].parent       = &command_array_P[i-4];
		command_array_P[i++].function   = &scpi_get_version_q;

		command_array_P[i].name         = PSTR("RECORD");
		command_array_P[i].implied      = false;
		command_array_P[i].parent       = &command_array_P[i-5];
		command_array_P[i++].function   = &scpi_record;
    
	command_array_P[i].name       = PSTR("*IDN?");
	command_array_P[i].implied    = false;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &scpi_IDN_q;
    
	command_array_P[i].name       = PSTR("DEBUG?");
	command_array_P[i].implied    = false;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &debug;
    
	command_array_P[i].name       = PSTR("GET");
	command_array_P[i].implied    = false;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &scpi_null_func;
    
        command_array_P[i].name       = PSTR("RANGE?");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-1];
        command_array_P[i++].function = &scpi_get_range_q;
		
        command_array_P[i].name       = PSTR("IIR_ALPHA?");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-2];
        command_array_P[i++].function = &scpi_get_IIR_alpha_q;
        
        command_array_P[i].name       = PSTR("IIR_BETA?");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-3];
        command_array_P[i++].function = &scpi_get_IIR_beta_q;
        
        command_array_P[i].name       = PSTR("IIR_GAIN?");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-4];
        command_array_P[i++].function = &scpi_get_IIR_gain_q;
		
        command_array_P[i].name       = PSTR("LASERPOWER?");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-5];
        command_array_P[i++].function = &scpi_get_laserpower_q;
        
        command_array_P[i].name       = PSTR("DETECTION_THRESHOLD_MM?");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-6];
        command_array_P[i++].function = &scpi_get_detection_threshold_mm_q;
        
        command_array_P[i].name       = PSTR("MAX_DISTANCE_LEAKAGE?");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-7];
        command_array_P[i++].function = &scpi_get_max_distance_leakage_q;
        
        command_array_P[i].name       = PSTR("WATER_DEBOUNCE_TIMEOUT?");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-8];
        command_array_P[i++].function = &scpi_get_water_debounce_timeout_q;

        command_array_P[i].name       = PSTR("BLACKBOX?");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-9];
        command_array_P[i++].function = &scpi_get_blackbox_q;
        
        command_array_P[i].name       = PSTR("MAX_DISTANCE_MM_RESET_RATE?");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-10];
        command_array_P[i++].function = &scpi_get_max_distance_mm_reset_rate_q;
        
        command_array_P[i].name       = PSTR("RANGE_AND_MAXRANGE?");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-11];
        command_array_P[i++].function = &scpi_get_range_and_maxrange_q;
        
        command_array_P[i].name       = PSTR("WATER_STATE?");
        command_array_P[i].implied    = false;
        command_array_P[i].parent     = &command_array_P[i-12];
        command_array_P[i++].function = &scpi_water_state_q;

	command_array_P[i].name       = PSTR("CLRI2C");
	command_array_P[i].implied    = false;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &clr_i2c;

	command_array_P[i].name       = PSTR("SET");
	command_array_P[i].implied    = false;
	command_array_P[i].parent     = &command_array_P[0];
	command_array_P[i++].function = &scpi_null_func;
		
		command_array_P[i].name       = PSTR("IIR_ALPHA");
		command_array_P[i].implied    = false;
		command_array_P[i].parent     = &command_array_P[i-1];
		command_array_P[i++].function = &scpi_set_IIR_alpha;
        
		command_array_P[i].name       = PSTR("IIR_BETA");
		command_array_P[i].implied    = false;
		command_array_P[i].parent     = &command_array_P[i-2];
		command_array_P[i++].function = &scpi_set_IIR_beta;
		
		command_array_P[i].name       = PSTR("LASERPOWER");
		command_array_P[i].implied    = false;
		command_array_P[i].parent     = &command_array_P[i-3];
		command_array_P[i++].function = &scpi_set_laserpower;
        
		command_array_P[i].name       = PSTR("DETECTION_THRESHOLD_MM");
		command_array_P[i].implied    = false;
		command_array_P[i].parent     = &command_array_P[i-4];
		command_array_P[i++].function = &scpi_set_detection_threshold_mm;
        
		command_array_P[i].name       = PSTR("MAX_DISTANCE_LEAKAGE");
		command_array_P[i].implied    = false;
		command_array_P[i].parent     = &command_array_P[i-5];
		command_array_P[i++].function = &scpi_set_max_distance_leakage;
        
		command_array_P[i].name       = PSTR("WATER_DEBOUNCE_TIMEOUT");
		command_array_P[i].implied    = false;
		command_array_P[i].parent     = &command_array_P[i-6];
		command_array_P[i++].function = &scpi_set_water_debounce_timeout;
        
		command_array_P[i].name       = PSTR("MAX_DISTANCE_MM_RESET_RATE");
		command_array_P[i].implied    = false;
		command_array_P[i].parent     = &command_array_P[i-7];
		command_array_P[i++].function = &scpi_set_max_distance_mm_reset_rate;
        
		command_array_P[i].name       = PSTR("WATER");
		command_array_P[i].implied    = false;
		command_array_P[i].parent     = &command_array_P[i-8];
		command_array_P[i++].function = &scpi_null_func;
        
            command_array_P[i].name       = PSTR("ON");
            command_array_P[i].implied    = false;
            command_array_P[i].parent     = &command_array_P[i-1];
            command_array_P[i++].function = &scpi_water_on;

            command_array_P[i].name       = PSTR("OFF");
            command_array_P[i].implied    = false;
            command_array_P[i].parent     = &command_array_P[i-2];
            command_array_P[i++].function = &scpi_water_off;

            command_array_P[i].name       = PSTR("AUTO");
            command_array_P[i].implied    = false;
            command_array_P[i].parent     = &command_array_P[i-3];
            command_array_P[i++].function = &scpi_water_auto;

	return i; // This is incremented so it matches "COMMAND_ARRAY_SIZE"
}
/****************************************************************************
*  *OPC (Operation Complete Query) function                                 *
*****************************************************************************/
void st_OPC_q ( char *arg, IO_pointers_t IO )
{
	scpi_prStr_P(PSTR("1\r\n"), IO.USB_stream);
}
/****************************************************************************
*  Write to the bootloader start address                                    *
*****************************************************************************/
void sys_rst_btloader( char *arg, IO_pointers_t IO )
{
	Jump_To_Bootloader();
}
/****************************************************************************
*  :SYSTem:ERRor?                                                           *
*****************************************************************************/
void sys_error_q( char *arg, IO_pointers_t IO )
{
	if (error_number == 0)
		scpi_prStr_P(PSTR("+0,\"No error\"\r\n"), IO.USB_stream);
	else
	{
		scpi_prStr_P(error_messages[error_number], IO.USB_stream);
		if (bad_command[0] != NUL)
		{
			fprintf(IO.USB_stream, "%s\r\n", bad_command);
			bad_command[0] = NUL;
		}	
		error_number--;
	}
}
/****************************************************************************
*  VERSION? function                                                        *
*****************************************************************************/
void scpi_get_version_q( char *arg, IO_pointers_t IO )
{
	scpi_prStr_P(PSTR(FIRMWARE_VERSION),IO.USB_stream);
	scpi_prStr_P(PSTR("\r\n"), IO.USB_stream);
}
/****************************************************************************
*  *IDN? function                                                           *
*****************************************************************************/
void scpi_IDN_q( char *arg, IO_pointers_t IO )
{
	scpi_prStr_P(PSTR(COMPANY_NAME), IO.USB_stream);
    scpi_prStr_P(PSTR(" | "), IO.USB_stream);
	scpi_prStr_P(PSTR(PROJECT_NAME), IO.USB_stream);
    scpi_prStr_P(PSTR(" | "), IO.USB_stream);
	scpi_prStr_P(PSTR("Firmware Revision: "), IO.USB_stream);
    scpi_prStr_P(PSTR(FIRMWARE_VERSION), IO.USB_stream);
    scpi_prStr_P(PSTR("\r\n"), IO.USB_stream);
}
/****************************************************************************
*  *CLS (Clear Status) function                                             *
*****************************************************************************/
void st_CLS( char *arg, IO_pointers_t IO ) {}
/****************************************************************************
*      On *RST call rt_open with "(@ALL)" to open all relays                *
*****************************************************************************/
void st_RST( char *arg, IO_pointers_t IO ) {}
/****************************************************************************
*   *TST                                                                    *
*****************************************************************************/
void st_TST( char *arg, IO_pointers_t IO ) {}
/****************************************************************************
*  *WAI (Wait To Complete) function                                         *
*****************************************************************************/
void st_WAI( char *arg, IO_pointers_t IO ) {}
/****************************************************************************
*  Debug function                                                           *
*****************************************************************************/
void debug(char *arg, IO_pointers_t IO)
{
    // uint8_t sensor_answer;
    // uint16_t counter = 0;
    // uint8_t reading_mm;

    // for (int i=0; i < 200; i++)
    // {
        // I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_SYSRANGE__START, VL6180X_SYSRANGE_STARTSTOP); //  kick off a reading
        // do {
            // I2C_16BITSUB_Read_Byte(VL6180X_ADDR7, VL6180X_RESULT__INTERRUPT_STATUS_GPIO, &sensor_answer); // wait for reading
            // if (sensor_answer)
            // {
                // fprintf(IO.USB_stream, "time: %lums ", debug_time_ms);
                // fprintf(IO.USB_stream, "RESULT__INTERRUPT_STATUS_GPIO: %x\r\n", sensor_answer);
            // }
        // }
        // while (! (sensor_answer & VL6180X_NEW_SAMPLE_READY_THRESHOLD_EVENT) && counter < 1000);
    
        // I2C_16BITSUB_Read_Byte(VL6180X_ADDR7, VL6180X_RESULT__RANGE_VAL, &reading_mm);
        // fprintf(IO.USB_stream, "Reading: %umm\r\n", reading_mm);
        // I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_SYSTEM__INTERRUPT_CLEAR, VL6180X_CLEAR_ALL_INTS );
    // }
    // fprintf(IO.USB_stream, "END OF RECORD\r\n\n");
}
/****************************************************************************
*  SCPI Get Range Reading                                                   *
*****************************************************************************/
void scpi_get_range_q(char *arg, IO_pointers_t IO)
{
    fprintf(IO.USB_stream, "%fmm\r\n", (double)distance_mm);
}
/****************************************************************************
*  SCPI Get Range and Max Range Readings - comma separated                  *
*****************************************************************************/
void scpi_get_range_and_maxrange_q(char *arg, IO_pointers_t IO)
{
    fprintf(IO.USB_stream, "{\"RANGE\":%f,\"MAXRANGE\":%f}\r\n", (double)distance_mm, (double)max_distance_mm);
}
/****************************************************************************
*  Clear Port                                                               *
*****************************************************************************/
void clr_i2c (char *arg, IO_pointers_t IO)
{
    reset_i2c();
}
/****************************************************************************
*  SCPI Water Auto                                                          *
*****************************************************************************/
void scpi_water_auto (char *arg, IO_pointers_t IO)
{
    water_auto = true;
	water_on(false);
}
/****************************************************************************
*  SCPI Water On                                                            *
*****************************************************************************/
void scpi_water_on (char *arg, IO_pointers_t IO)
{
    water_auto = false;
	water_on(true);
}
/****************************************************************************
*  SCPI Water Off                                                           *
*****************************************************************************/
void scpi_water_off (char *arg, IO_pointers_t IO)
{
    water_auto = false;
	water_on(false);
}
/****************************************************************************
*  SCPI Water State Query                                                   *
*****************************************************************************/
void scpi_water_state_q (char *arg, IO_pointers_t IO)
{
    fprintf(IO.USB_stream, "%u\r\n", WATERPORT & WATER ? 1 : 0);
}
/****************************************************************************
*  Store Laser Power to EEPROM                                              *
*****************************************************************************/
void scpi_set_laserpower( char *arg, IO_pointers_t IO )
{
    char *endptr;
    uint8_t value;
	if (strlen(arg) <= MAX_ARG_LEN)
    {
        remove_ws(arg);
        value = (uint8_t)strtol(arg, &endptr, 10);
        if (strlen(arg) <= MAX_ARG_LEN && strlen(arg) >= 1)
        {
            eeprom_busy_wait();
            eeprom_write_block((const void *)&value, &LASER_POWER, sizeof(uint8_t));
            retrieve_laserpower_setting();
        }
        else
            scpi_add_error_P(error_arg_too_long, IO);
    }
	else
		scpi_add_error_P(error_arg_too_long, IO);
}
/****************************************************************************
*  Update Laser Power from EEPROM                                           *
*****************************************************************************/
void retrieve_laserpower_setting()
{
    eeprom_busy_wait();
    eeprom_read_block((void*)&laser_power, (const void *)&LASER_POWER, sizeof(uint8_t));
    set_laserpower();
}
/****************************************************************************
*  SCPI Get Laser Power Setting                                             *
*****************************************************************************/
void scpi_get_laserpower_q( char *arg, IO_pointers_t IO )
{
	fprintf(IO.USB_stream, "%u\r\n", laser_power);
}
/****************************************************************************
*  Store Water Debounce Timeout to EEPROM                                   *
*****************************************************************************/
void scpi_set_water_debounce_timeout( char *arg, IO_pointers_t IO )
{
    char *endptr;
    uint16_t value;
	if (strlen(arg) <= MAX_ARG_LEN)
    {
        remove_ws(arg);
        value = (uint16_t)strtol(arg, &endptr, 10);
        if (strlen(arg) <= MAX_ARG_LEN && strlen(arg) >= 1)
        {
            eeprom_busy_wait();
            eeprom_write_block((const void *)&value, &WATER_DEBOUNCE_TIMEOUT, sizeof(uint16_t));
            retrieve_water_debounce_timeout();
        }
        else
            scpi_add_error_P(error_arg_too_long, IO);
    }
	else
		scpi_add_error_P(error_arg_too_long, IO);
}
/****************************************************************************
*  Update Water Debounce Timeout from EEPROM                                *
*****************************************************************************/
void retrieve_water_debounce_timeout()
{
    eeprom_busy_wait();
    eeprom_read_block((void*)&water_debounce_timeout, (const void *)&WATER_DEBOUNCE_TIMEOUT, sizeof(uint16_t));
}
/****************************************************************************
*  SCPI Water Debounce Timeout Setting                                      *
*****************************************************************************/
void scpi_get_water_debounce_timeout_q( char *arg, IO_pointers_t IO )
{
	fprintf(IO.USB_stream, "%u\r\n", water_debounce_timeout);
}
/****************************************************************************
*  Store IIR Factor Alpha to EEPROM                                         *
*****************************************************************************/
void scpi_set_IIR_alpha( char *arg, IO_pointers_t IO )
{
    char *endptr;
    float value;
	if (strlen(arg) <= MAX_ARG_LEN)
    {
        remove_ws(arg);
        value = strtod(arg, &endptr);
        if (strlen(arg) <= MAX_ARG_LEN && strlen(arg) >= 1)
        {
            eeprom_busy_wait();
            eeprom_write_block((const void *)&value, &IIR_ALPHA, sizeof(float));
            retrieve_IIR_alpha();
        }
        else
            scpi_add_error_P(error_arg_too_long, IO);
    }
	else
		scpi_add_error_P(error_arg_too_long, IO);
}
/****************************************************************************
*  Update IIR Factor Alpha from EEPROM                                      *
*****************************************************************************/
void retrieve_IIR_alpha()
{
    eeprom_busy_wait();
    eeprom_read_block((void*)&iir_alpha, (const void*)IIR_ALPHA, sizeof(float));
    compute_iir_gain();
}
/****************************************************************************
*  SCPI Print IIR Factor Alpha                                              *
*****************************************************************************/
void scpi_get_IIR_alpha_q( char *arg, IO_pointers_t IO )
{
	fprintf(IO.USB_stream, "%f\r\n", (double)iir_alpha);
}
/****************************************************************************
*  Store IIR Factor Beta to EEPROM                                          *
*****************************************************************************/
void scpi_set_IIR_beta( char *arg, IO_pointers_t IO )
{
    char *endptr;
    float value;
	if (strlen(arg) <= MAX_ARG_LEN)
    {
        remove_ws(arg);
        value = strtod(arg, &endptr);
        if (strlen(arg) <= MAX_ARG_LEN && strlen(arg) >= 1)
        {
            eeprom_busy_wait();
            eeprom_write_block((const void *)&value, &IIR_BETA, sizeof(float));
            retrieve_IIR_beta();
        }
        else
            scpi_add_error_P(error_arg_too_long, IO);
    }
	else
		scpi_add_error_P(error_arg_too_long, IO);
}
/****************************************************************************
*  Update IIR Factor Beta from EEPROM                                       *
*****************************************************************************/
void retrieve_IIR_beta()
{
    eeprom_busy_wait();
    eeprom_read_block((void*)&iir_beta, (const void*)IIR_BETA, sizeof(float));
    compute_iir_gain();
}
/****************************************************************************
*  SCPI Print IIR Factor Beta                                               *
*****************************************************************************/
void scpi_get_IIR_beta_q( char *arg, IO_pointers_t IO )
{
	fprintf(IO.USB_stream, "%f\r\n", (double)iir_beta);
}
/****************************************************************************
*  Store Detection Threshold to EEPROM                                      *
*****************************************************************************/
void scpi_set_detection_threshold_mm( char *arg, IO_pointers_t IO )
{
    char *endptr;
    float value;
	if (strlen(arg) <= MAX_ARG_LEN)
    {
        remove_ws(arg);
        value = strtod(arg, &endptr);
        if (strlen(arg) <= MAX_ARG_LEN && strlen(arg) >= 1)
        {
            eeprom_busy_wait();
            eeprom_write_block((const void *)&value, &THRESHOLD_MM, sizeof(float));
            retrieve_detection_threshold_mm();
        }
        else
            scpi_add_error_P(error_arg_too_long, IO);
    }
	else
		scpi_add_error_P(error_arg_too_long, IO);
}
/****************************************************************************
*  Update Detection Threshold from EEPROM                                   *
*****************************************************************************/
void retrieve_detection_threshold_mm()
{
    eeprom_busy_wait();
    eeprom_read_block((void*)&threshold_mm, (const void*)THRESHOLD_MM, sizeof(float));
}
/****************************************************************************
*  SCPI Detection Threshold                                                 *
*****************************************************************************/
void scpi_get_detection_threshold_mm_q( char *arg, IO_pointers_t IO )
{
	fprintf(IO.USB_stream, "%f\r\n", (double)threshold_mm);
}
/****************************************************************************
*  Store Max Distance Leakage to EEPROM                                     *
*****************************************************************************/
void scpi_set_max_distance_leakage( char *arg, IO_pointers_t IO )
{
    char *endptr;
    float value;
	if (strlen(arg) <= MAX_ARG_LEN)
    {
        remove_ws(arg);
        value = strtod(arg, &endptr);
        if (strlen(arg) <= MAX_ARG_LEN && strlen(arg) >= 1)
        {
            eeprom_busy_wait();
            eeprom_write_block((const void *)&value, &MAX_DISTANCE_LEAKAGE, sizeof(float));
            retrieve_max_distance_leakage();
        }
        else
            scpi_add_error_P(error_arg_too_long, IO);
    }
	else
		scpi_add_error_P(error_arg_too_long, IO);
}
/****************************************************************************
*  Store Max Distance Reset Rate to EEPROM                                  *
*****************************************************************************/
void scpi_set_max_distance_mm_reset_rate( char *arg, IO_pointers_t IO )
{
    char *endptr;
    float value;
	if (strlen(arg) <= MAX_ARG_LEN)
    {
        remove_ws(arg);
        value = strtod(arg, &endptr);
        if (strlen(arg) <= MAX_ARG_LEN && strlen(arg) >= 1)
        {
            eeprom_busy_wait();
            eeprom_write_block((const void *)&value, &MAX_DISTANCE_MM_RESET_RATE, sizeof(float));
            retrieve_max_distance_mm_reset_rate();
        }
        else
            scpi_add_error_P(error_arg_too_long, IO);
    }
	else
		scpi_add_error_P(error_arg_too_long, IO);
}
/****************************************************************************
*  Update Max Distance Reset Rate from EEPROM                               *
*****************************************************************************/
void retrieve_max_distance_mm_reset_rate()
{
    eeprom_busy_wait();
    eeprom_read_block((void*)&max_distance_mm_reset_rate, (const void*)MAX_DISTANCE_MM_RESET_RATE, sizeof(float));
}
/****************************************************************************
*  Update Max Distance Leakage Factor from EEPROM                           *
*****************************************************************************/
void retrieve_max_distance_leakage()
{
    eeprom_busy_wait();
    eeprom_read_block((void*)&max_distance_leakage, (const void*)MAX_DISTANCE_LEAKAGE, sizeof(float));
}
/******************************************************************************
*  SCPI Print Max Distance Leakage Factor                                     *
*******************************************************************************/
void scpi_get_max_distance_leakage_q( char *arg, IO_pointers_t IO )
{
	fprintf(IO.USB_stream, "%f\r\n", (double)max_distance_leakage);
}
/****************************************************************************
*  Compute IIR Gain                                                         *
*****************************************************************************/
void compute_iir_gain()
{
	iir_gain = 1.0f / (1.0f + iir_alpha + iir_beta);
}
/****************************************************************************
*  SCPI Get IIR Gain                                                        *
*****************************************************************************/
void scpi_get_IIR_gain_q( char *arg, IO_pointers_t IO )
{
	fprintf(IO.USB_stream, "%f\r\n", (double)iir_gain);
}
/****************************************************************************
*  SCPI Get Blackbox Distances (mm)                                         *
*****************************************************************************/
void scpi_get_blackbox_q( char *arg, IO_pointers_t IO )
{
    fprintf(IO.USB_stream, "{\"RECORDING\": %s, ", record ? "True" : "False");
    fprintf(IO.USB_stream, "\"DISTANCE_mm\": [");
    uint8_t count = 0;
    while (count < BLACKBOX_BUFFER_SIZE)
        fprintf(IO.USB_stream, "%.3f,", (double)blackbox[(blackbox_index + count++) & (BLACKBOX_BUFFER_SIZE - 1)].distance_mm);
    fprintf(IO.USB_stream, "], \"MAX_DISTANCE_mm\": [");
    count = 0;
    while (count < BLACKBOX_BUFFER_SIZE)
        fprintf(IO.USB_stream, "%.3f,", (double)blackbox[(blackbox_index + count++) & (BLACKBOX_BUFFER_SIZE - 1)].max_distance_mm);
    fprintf(IO.USB_stream, "]}\r\n");
}
/****************************************************************************
*  Record (enable Black Box)                                                *
*****************************************************************************/
void scpi_record( char *arg, IO_pointers_t IO )
{
	record = true;
}
/****************************************************************************
*  SCPI Get Max Distance Reset Rate                                         *
*****************************************************************************/
void scpi_get_max_distance_mm_reset_rate_q( char *arg, IO_pointers_t IO )
{
	fprintf(IO.USB_stream, "%f\r\n", (double)max_distance_mm_reset_rate);
}