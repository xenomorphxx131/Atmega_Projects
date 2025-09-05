/**************************************************************************
*                                                                         *
* SCPI Commands                                                           *
*                                                                         *
***************************************************************************/

#include "HTX9000SE_scpi.h"

uint8_t     EEMEM HARDWARE_REV[CAL_STRING_SIZE];
uint8_t     EEMEM CAL_DATE[CAL_STRING_SIZE];
uint16_t    EEMEM UPPER_RANGE_DAC_CAL_VALUE;
uint16_t    EEMEM LOWER_RANGE_DAC_CAL_VALUE;
uint8_t     EEMEM LOWER_RANGE_CAL_STRING[CAL_STRING_SIZE];
uint8_t     EEMEM UPPER_RANGE_CAL_STRING[CAL_STRING_SIZE];
uint16_t    EEMEM CALIBRATION_LO_CHECK_SUM;
uint16_t    EEMEM CALIBRATION_HI_CHECK_SUM;
extern int error_count;
bool fan_off_override = 0;
char const DATE_ARG_TOO_LONG[] PROGMEM = "+10,\"ERR Date argument too long - max 15\"";
char const CAL_ARG_TOO_LONG[] PROGMEM = "+10,\"ERR Cal argument too long - max 15\"";

settings_t settings = { .range = LOW_RANGE,
                        .dropout = true,
                        .dac_value = 0,
                        .upper_range_dac_cal_value = 0,
                        .lower_range_dac_cal_value = 0,
                        .touchpad_locked = false,
                        .screen_up_to_date = false,
                        .checksum_lo = 0,
                        .checksum_hi = 0,
                        .upper_range_cal_string = "",
                        .lower_range_cal_string = "",
                        .cal_date_string = ""};
temperatures_t temperatures = {.ntc_temp = 21,
                               .die_temp = 21};
/****************************************************************************
*  Setup Awesomfaucet Specific SCPI commands and functions                  *
*****************************************************************************/
void setup_scpi_commands(SCPI_Node_t **command_array)
{
    SCPI_OPEN_REGISTRY(command_array, Jump_To_Bootloader);
/*  SCPI(.node,         .value,                             parent,            .function,                      .implied */
/*  --------------------------------------------------------------------------------------------------------------------*/
    SCPI(OPC,               "*OPC?",                        NULL,               &st_OPC_q,                      false);
    SCPI(IDN,               "*IDN?",                        NULL,               &st_IDN_q,                      false);

    SCPI(SYSTEM,            "SYSTem",                       NULL,               NULL,                           true );
    SCPI(ERROR,                 "ERRor?",                   &SYSTEM,            &sys_error_q,                   false);
    SCPI(LOCK,                  "LOCK",                     &SYSTEM,            &scpi_lock_touchpad,            false);
    SCPI(RELEASE,                   "RELease",              &LOCK,              &scpi_unlock_touchpad,          false);
    SCPI(RST,                  "RST",                       &SYSTEM,            NULL,                           false);
    SCPI(BTLOADER,                 "BTLOader",              &RST,               &sys_rst_btloader,              false);
    SCPI(VERSION,                  "VERSion?",              &SYSTEM,            &scpi_get_version_q,            false);

    SCPI(SOURCE,            "SOURce",                       NULL,               NULL,                           true);
    SCPI(CURRENT,               "CURRent",                  &SOURCE,            &scpi_set_crnt_auto,            false);
    SCPI(CURRENT_Q,             "CURRent?",                 &SOURCE,            &scpi_get_current_q,            false);
    SCPI(RANGE,                     "RANGe",                &CURRENT,           NULL,                           false);
    SCPI(RANGE_Q,                   "RANGe?",               &CURRENT,           &scpi_get_range_q,              false);
    SCPI(LOW,                           "LOw",              &RANGE,             &scpi_set_crnt_low,             false);
    SCPI(HIGH,                          "HIgh",             &RANGE,             &scpi_set_crnt_high,            false);

    SCPI(TEMP,              "TEMP",                         NULL,               NULL,                           false);
    SCPI(HEATSINK_Q,            "HEATsink?",                &TEMP,              &scpi_get_heatsink_temp_q,      false);
    SCPI(BOARD_Q,               "BOARD?",                   &TEMP,              &scpi_get_board_temp_q,         false);
    SCPI(DROPOUTQ,          "DROPout?",                     NULL,               &scpi_get_dropout_state_q,      false);

    SCPI(FAN_C,             "FAN",                          NULL,               NULL,                           false);
    SCPI(FAN_Q,             "FAN?",                         NULL,               &scpi_get_fan_state,            false);
    SCPI(FAN_ON,                "ON",                       &FAN_C,             &scpi_enable_fan,               false);
    SCPI(FAN_OFF,               "OFF",                      &FAN_C,             &scpi_disable_fan,              false);

    SCPI(CALIBRATE,         "CALibrate",                    NULL,               NULL,                           false);
    SCPI(OUTPUT_CAL,            "OUTPut",                   &CALIBRATE,         NULL,                           false);
    SCPI(CURRENT_CAL,               "CURRent",              &OUTPUT_CAL,        NULL,                           false);
    SCPI(VALUE,                         "VALue",            &CURRENT_CAL,       NULL,                           false);
    SCPI(RANGE_CAL,                         "RANGe",        &VALUE,             NULL,                           false);
    SCPI(RANGE_CAL_HI,                          "HIgh",     &RANGE_CAL,         &scpi_store_cal_val_hi_range,   false);
    SCPI(RANGE_CAL_HI_Q,                        "HIgh?",    &RANGE_CAL,         &scpi_read_cal_val_hi_range_q,  false);
    SCPI(RANGE_CAL_LO,                          "LOw",      &RANGE_CAL,         &scpi_store_cal_val_lo_range,   false);
    SCPI(RANGE_CAL_LO_Q,                        "LOw?",     &RANGE_CAL,         &scpi_read_cal_val_lo_range_q,  false);
    SCPI(DAC_CAL,                       "DAC",              &CURRENT_CAL,       NULL,                           false);
    SCPI(DAC_RANGE_CAL,                     "RANGe",        &DAC_CAL,           NULL,                           false);
    SCPI(DAC_RANGE_HI_CAL,                      "HIgh",     &DAC_RANGE_CAL,     &scpi_store_cal_dac_hi_range,   false);
    SCPI(DAC_RANGE_HI_CAL_Q,                    "HIgh?",    &DAC_RANGE_CAL,     &scpi_read_cal_dac_hi_range_q,  false);
    SCPI(DAC_RANGE_LO_CAL,                      "LOw",      &DAC_RANGE_CAL,     &scpi_store_cal_dac_lo_range,   false);
    SCPI(DAC_RANGE_LO_CAL_Q,                    "LOw?",     &DAC_RANGE_CAL,     &scpi_read_cal_dac_lo_range_q,  false);
    SCPI(DATE_CAL,              "DATE",                     &CALIBRATE,         &scpi_store_cal_date,           false);
    SCPI(DATE_CAL_Q,            "DATE?",                    &CALIBRATE,         &scpi_read_cal_date_q,          false);
    SCPI(BOARDREV,              "BOArdrev",                 &CALIBRATE,         &scpi_store_board_rev,          false);
    SCPI(CHECKSUM,              "CHECKsum",                 &CALIBRATE,         NULL,                           false);
    SCPI(HIGH_CAL,                  "HIgh?",                &CHECKSUM,          &scpi_read_checksum_high_q,     false);
    SCPI(LOW_CAL,                   "LOw?",                 &CHECKSUM,          &scpi_read_checksum_low_q,      false);

    SCPI_CLOSE_REGISTRY();
}
/**************************************************************************
*  Save Calibration date to EEPROM                                        *
***************************************************************************/
void scpi_store_cal_date(char *arg, IO_pointers_t IO)
{
    if (strlen(arg) <= MAX_ARG_LEN)
    {
        eeprom_busy_wait();
        eeprom_write_block(arg, &CAL_DATE, CAL_STRING_SIZE);
    }
    else
        scpi_add_error_P(DATE_ARG_TOO_LONG, "", 0);
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
        scpi_add_error_P(CAL_ARG_TOO_LONG, "", 0);
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
void scpi_store_cal_val_lo_range(char *arg, IO_pointers_t IO)
{
    uint16_t checksum;
    if (strlen(arg) <= MAX_ARG_LEN)
    {
        eeprom_busy_wait();
        eeprom_write_block(arg, &LOWER_RANGE_CAL_STRING, CAL_STRING_SIZE);
        eeprom_busy_wait();
        eeprom_write_word (&LOWER_RANGE_DAC_CAL_VALUE, settings.dac_value);
        checksum = get_checksum(settings.cal_date_string, arg, settings.dac_value);
        eeprom_write_word (&CALIBRATION_LO_CHECK_SUM, checksum);
    }
    else
        scpi_add_error_P(CAL_ARG_TOO_LONG, "", 0);
    settings.touchpad_locked = false;
    settings.screen_up_to_date = false;
}
/**************************************************************************
*  Store high range DAC value and calibration value to EEPROM             *
***************************************************************************/
void scpi_store_cal_val_hi_range(char *arg, IO_pointers_t IO)
{    
    uint16_t checksum;
    if (strlen(arg) <= MAX_ARG_LEN)
    {
        eeprom_busy_wait();
        eeprom_write_block(arg, &UPPER_RANGE_CAL_STRING, CAL_STRING_SIZE);
        eeprom_busy_wait();
        eeprom_write_word (&UPPER_RANGE_DAC_CAL_VALUE, settings.dac_value);
        checksum = get_checksum(settings.cal_date_string, arg, settings.dac_value);
        eeprom_write_word (&CALIBRATION_HI_CHECK_SUM, checksum);
    }
    else
        scpi_add_error_P(CAL_ARG_TOO_LONG, "", 0);
    settings.touchpad_locked = false;
    settings.screen_up_to_date = false;
}
/**************************************************************************
*  Retrieve Cal Date value from EEPROM                                    *
***************************************************************************/
void scpi_read_cal_date_q(char *arg, IO_pointers_t IO)
{    
    scpi_read_cal_data(IO);
    fprintf(IO.USB_stream, "%s\n", settings.cal_date_string);
}
/**************************************************************************
*  Retrieve high range calibration value from EEPROM                      *
***************************************************************************/
void scpi_read_cal_val_hi_range_q(char *arg, IO_pointers_t IO)
{    
    scpi_read_cal_data(IO);
    fprintf(IO.USB_stream, "%s\n", settings.upper_range_cal_string);
}
/**************************************************************************
*  Retrieve low range calibration value from EEPROM                       *
***************************************************************************/
void scpi_read_cal_val_lo_range_q(char *arg, IO_pointers_t IO)
{    
    scpi_read_cal_data(IO);
    fprintf(IO.USB_stream, "%s\n", settings.lower_range_cal_string);
}
/**************************************************************************
*  Retrieve high range calibration dac setting from EEPROM                *
***************************************************************************/
void scpi_read_cal_dac_hi_range_q(char *arg, IO_pointers_t IO)
{    
    scpi_read_cal_data(IO);
    fprintf(IO.USB_stream, "%X\n", settings.upper_range_dac_cal_value);
}
/**************************************************************************
*  Retrieve low range calibration dac setting from EEPROM                 *
***************************************************************************/
void scpi_read_cal_dac_lo_range_q(char *arg, IO_pointers_t IO)
{    
    scpi_read_cal_data(IO);
    fprintf(IO.USB_stream, "%X\n", settings.lower_range_dac_cal_value);
}
/**************************************************************************
*  Retrieve high range calibration checksum from EEPROM                   *
***************************************************************************/
void scpi_read_checksum_high_q(char *arg, IO_pointers_t IO)
{
    scpi_read_cal_data(IO);
    if (settings.checksum_hi == get_checksum(   settings.cal_date_string,
                                                settings.upper_range_cal_string,
                                                settings.upper_range_dac_cal_value))
        fprintf(IO.USB_stream, "1\n");
    else
        fprintf(IO.USB_stream, "0\n");                        
}
/**************************************************************************
*  Retrieve low range calibration checksum from EEPROM                    *
***************************************************************************/
void scpi_read_checksum_low_q(char *arg, IO_pointers_t IO)
{
    scpi_read_cal_data(IO);
    if (settings.checksum_lo == get_checksum(   settings.cal_date_string,
                                                settings.lower_range_cal_string,
                                                settings.lower_range_dac_cal_value))
        fprintf(IO.USB_stream, "1\n");
    else
        fprintf(IO.USB_stream, "0\n");
}
/**************************************************************************
* CALibrate:DAC:RANGe:HIgh <value> Direct access to the dac in low range  *
***************************************************************************/
void scpi_store_cal_dac_hi_range(char *arg, IO_pointers_t IO)
{
    settings.touchpad_locked = true;
    scpi_set_range(HIGH_RANGE, IO);
    settings.dac_value = atof(arg);
    scpi_set_dac(settings.dac_value);
}
/**************************************************************************
* CALibrate:DAC:RANGe:LOw <value> Direct access to the dac in low range   *
***************************************************************************/
void scpi_store_cal_dac_lo_range(char *arg, IO_pointers_t IO)
{
    settings.touchpad_locked = true;
    scpi_set_range(LOW_RANGE, IO);
    settings.dac_value = atof(arg);
    scpi_set_dac(settings.dac_value);
}
/**************************************************************************
*  Set the Current in Auto Range mode                                     *
***************************************************************************/
void scpi_set_crnt_auto(char *arg, IO_pointers_t IO)
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
    
    if (settings.range == LOW_RANGE && (requested_value + stepsize_lo / 2) * settings.lower_range_dac_cal_value  / parsed_cal_value_lo > 0xFFFF)
        scpi_set_range(HIGH_RANGE, IO);                // Only switch ranges up if necessary otherwise keep old range
    
//    if (settings.range == HIGH && (requested_value * settings.upper_range_dac_cal_value) / parsed_cal_value_hi < 1)
    if (settings.range == HIGH_RANGE && (requested_value * settings.upper_range_dac_cal_value) / parsed_cal_value_hi < 65635 / parsed_cal_value_hi * parsed_cal_value_lo)
        scpi_set_range(LOW_RANGE, IO);                // Only switch ranges down if necessary otherwise keep old range

    if (settings.range == LOW_RANGE)
    {    if ((requested_value + stepsize_lo / 2) / parsed_cal_value_lo * settings.lower_range_dac_cal_value <= 0xFFFF)
            settings.dac_value = (requested_value + stepsize_lo / 2) / parsed_cal_value_lo * settings.lower_range_dac_cal_value;
        else
            settings.dac_value = 0xFFFF;
    }
    else
    {    if ((requested_value + stepsize_hi / 2) / parsed_cal_value_hi * settings.upper_range_dac_cal_value <= 0xFFFF)
            settings.dac_value = (requested_value + stepsize_hi / 2) / parsed_cal_value_hi * settings.upper_range_dac_cal_value;
        else
            settings.dac_value = 0xFFFF;
    }
    scpi_set_dac(settings.dac_value);
    // re-normalize for LCD display with dac resolution and clipped value
    if (settings.range == LOW_RANGE)
        requested_value = settings.dac_value * parsed_cal_value_lo / settings.lower_range_dac_cal_value;
    else
        requested_value = settings.dac_value * parsed_cal_value_hi / settings.upper_range_dac_cal_value;

    draw_status_screen(requested_value, temperatures.ntc_temp, settings.touchpad_locked, settings.range, IO);
}
/**************************************************************************
*  Set the Current Low Range                                              *
***************************************************************************/
void scpi_set_crnt_low(char *arg, IO_pointers_t IO)
{
    double requested_value;
    double parsed_cal_value;
    
    scpi_read_cal_data(IO);
    scpi_set_range(LOW_RANGE, IO);
    requested_value = scpi_parse_current_arg(arg);
    parsed_cal_value = scpi_parse_current_arg(settings.lower_range_cal_string);
    if ((requested_value + STEPSIZE_LO / 2) / parsed_cal_value * settings.lower_range_dac_cal_value <= 0xFFFF)
        settings.dac_value = (requested_value + STEPSIZE_LO / 2) / parsed_cal_value * settings.lower_range_dac_cal_value;
    else
        settings.dac_value = 0xFFFF;
    scpi_set_dac(settings.dac_value);
    // re-normalize for LCD display with dac resolution and clipped value
    requested_value = settings.dac_value * parsed_cal_value / settings.lower_range_dac_cal_value;
    draw_status_screen(requested_value, temperatures.ntc_temp, settings.touchpad_locked, settings.range, IO);
}
/**************************************************************************
*  Set the Current High    Range                                             *
***************************************************************************/
void scpi_set_crnt_high(char *arg, IO_pointers_t IO)
{
    double requested_value;
    double parsed_cal_value;

    scpi_read_cal_data(IO);
    scpi_set_range(HIGH_RANGE, IO);
    requested_value = scpi_parse_current_arg(arg);
    parsed_cal_value = scpi_parse_current_arg(settings.upper_range_cal_string);
    if ((requested_value + STEPSIZE_HI / 2 ) / parsed_cal_value * settings.upper_range_dac_cal_value <= 0xFFFF)
        settings.dac_value = (requested_value + STEPSIZE_HI / 2 ) / parsed_cal_value * settings.upper_range_dac_cal_value;
    else
        settings.dac_value = 0xFFFF;
        
    scpi_set_dac(settings.dac_value);
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
    settings.range = settings.range == HIGH_RANGE ? LOW_RANGE : HIGH_RANGE;
    scpi_set_range(settings.range, IO);
    update_screen(IO);
}
/**************************************************************************
*  Set the Current Range to Low or High                                      *
***************************************************************************/
void scpi_set_range(uint8_t range, IO_pointers_t IO)
{
    if  (range == HIGH_RANGE)
    {    settings.range = HIGH_RANGE;
        scpi_set_dac(0);                // Set the dac to zero before switching
        I2P5A_PORT |=  I2P5A;           // To prevent load dump
    }
    else
    {    settings.range = LOW_RANGE;
        scpi_set_dac(0);                // Set the dac to zero before switching
        I2P5A_PORT &= ~I2P5A;           // To prevent load dump
    }
}
/**************************************************************************
*  Set the Current Range to Low or High                                      *
***************************************************************************/
void scpi_get_current_q(char *arg, IO_pointers_t IO)
{
    if (settings.range == LOW_RANGE)
        fprintf(IO.USB_stream, "%6.4e\n", get_current_value(IO));
    else
        fprintf(IO.USB_stream, "%6.5e\n", get_current_value(IO));
}
/**************************************************************************
*  Set the Current Range to Low or High                                      *
***************************************************************************/
double get_current_value(IO_pointers_t IO)
{
    double parsed_cal_value_lo;
    double parsed_cal_value_hi;

    scpi_read_cal_data(IO);
    parsed_cal_value_lo = scpi_parse_current_arg(settings.lower_range_cal_string);
    parsed_cal_value_hi = scpi_parse_current_arg(settings.upper_range_cal_string);
    
    if (settings.range == LOW_RANGE)
        return settings.dac_value * parsed_cal_value_lo / settings.lower_range_dac_cal_value;
    else
        return settings.dac_value * parsed_cal_value_hi / settings.upper_range_dac_cal_value;
}
/****************************************************************************
*    Output the D/A Value to the D/A                                        *
*****************************************************************************/
void scpi_set_dac(uint16_t dac_setting)
{
    DACPORT &= ~LDAC;                                    // Load line low to receive data
    SPDR = (uint8_t)(dac_setting >> 8);
    while (!SPI_READY)
        ;                                               // Wait for transaction to complete
    SPDR = (uint8_t)(dac_setting);
    while (!SPI_READY)
        ;                                               // Wait for transaction to complete
    DACPORT |= LDAC;                                    // Load high update the D/A data
    
    if (dac_setting == 0)
    {
        ZERO_CURRENT_PORT   |= ZERO_CURRENT;                // Set the Zero Current port pin
        LOW_CUR_ASSIST_PORT    |= LOW_CUR_ASSIST;            // Set the Low Current assist pin
    }
    else if (settings.dac_value <= LOW_CURR_ASSIST_DACVAL)
    {
        ZERO_CURRENT_PORT   &= ~ZERO_CURRENT;                // Clear the Zero Current port pin
        LOW_CUR_ASSIST_PORT    |= LOW_CUR_ASSIST;            // Set the Low Current Assist port pin
    }
    else
    {
        ZERO_CURRENT_PORT   &= ~ZERO_CURRENT;                // Clear the Zero Current port pin
        LOW_CUR_ASSIST_PORT    &= ~LOW_CUR_ASSIST;            // Clear the Low Current Assist port pin
    }
}
/****************************************************************************
*    Set the current from the key pad                                       *
*****************************************************************************/
void set_current_from_keypad(char* value, IO_pointers_t IO)
{
    if (!settings.touchpad_locked)
        scpi_set_crnt_auto(value, IO);
}
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
void st_IDN_q(char *arg, IO_pointers_t IO)
{
    char hardware_rev[CAL_STRING_SIZE] = "";

    eeprom_read_block(&hardware_rev, &HARDWARE_REV, CAL_STRING_SIZE);
    scpi_prStr_P(PSTR(COMPANY_NAME), IO.USB_stream);
    scpi_prStr_P(PSTR(", "), IO.USB_stream);
    scpi_prStr_P(PSTR(PROJECT_NAME), IO.USB_stream);
    scpi_prStr_P(PSTR(", "), IO.USB_stream);
    fprintf(IO.USB_stream, "%s", hardware_rev);
    scpi_prStr_P(PSTR(", "), IO.USB_stream);
    scpi_prStr_P(PSTR(FIRMWARE_VERSION), IO.USB_stream);
    scpi_prStr_P(PSTR("\r\n"), IO.USB_stream);
}
/**************************************************************************
*  Parse argument from user                                               *
***************************************************************************/
double scpi_parse_current_arg(char *arg)
{
    char        units = NUL;                        // Keeps track of the units if any
    uint16_t    strg_len = 0;                       // Defined to ensure strlen only gets called once
    double        value;                            // 1 sign 8 exponents and 23 fractions

    strg_len = strlen(arg);                         // Get initial string length

    if (arg[strg_len-1] == 'A')                     // Check for an 'A'. Look at -1 because string starts at index 0.
    {
        arg[strg_len-1] = NUL;                     // Replace the 'A' with NUL character to end the string
        if (arg[strg_len-2] == 'N'        ||        // Check for Nano
            arg[strg_len-2] == 'U'        ||        // Check for micro
            arg[strg_len-2] == 'M')                 // Check for Milli
        {
            units = arg[strg_len-2];               // If a unit was found get the units.
            arg[strg_len-2] = NUL;                  // Replace trailing [N|U|M] with NUL character to end the string
        }
    }
    else if (arg[strg_len-1] == 'N'        ||       // Check for Nano
             arg[strg_len-1] == 'U'        ||       // Check for micro
             arg[strg_len-1] == 'M')                // Check for Milli
        {
            units = arg[strg_len-1];              // If a unit was found get the units.
            arg[strg_len-1] = NUL;                 // Replace trailing [N|U|M] with NUL character to end the string
        }

    value = atof(arg);                              // Send the remaining string to float
    if (units == 'N') value = value /1e9;           // If units were previously found to be nano then divide by 1e9
    if (units == 'U') value = value /1e6;           // If units were previously found to be micro then divide by 1e6
    if (units == 'M') value = value /1e3;           // If units were previously found to be milli then divide by 1e3
    // TODO Need to parse “AMPS” according to scpi
    return value;
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
void scpi_get_heatsink_temp_q(char *arg, IO_pointers_t IO)
{
    get_temperatures();
    fprintf(IO.USB_stream, "%7.3f\n", temperatures.ntc_temp);
}
/**************************************************************************
*  Report the PC Board Temperature A/D reading                            *
***************************************************************************/
void scpi_get_board_temp_q(char *arg, IO_pointers_t IO)
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
    
    ADMUX         =     AVCC_REF        |                // Set to supply as reference for thermistor.
                    (1 << ADLAR)    |                // Set ADC reading to left justified.
                    NTC_ADC_INPUTA;                    // Set the MUX[4:0] to point to the thermistor single ended (still needs MUX[5] on ADCSRB).
    ADCSRB         =    NTC_ADC_INPUTB;                    // makes up the rest of the MUX[5:0] value.
    for (int i = 1; i <= 2; i++)                    // Take two reading to ensure reference is settled
    {
        ADCSRA        |= (1 << ADSC);                    // Start Conversion
        while (ADCSRA & (1 << ADSC));                // Conversion done when ADSC goes low again. Loop until conversion complete.
    }
    temperatures.ntc_temp
        = Convert_NTC_To_DEGC(ADCL | (ADCH << 8));    // Ensure low byte first to guarantee data coherence (see manual).

    ADMUX         =     INTERNAL_REF    |                // Set to external internal ref for die temp.
                    (0 << ADLAR)    |                // Leave ADC reading right justified.
                    DIETEMP_ADC_INPUTA;                // Set the MUX[4:0] to point to the on board temp sensor single ended (still needs MUX[5] on ADCSRB).
    ADCSRB         =    DIETEMP_ADC_INPUTB;                // makes up the rest of the MUX[5:0] value.
    for (int i = 1; i <= 2; i++)                    // Take two reading to ensure reference is settled
    {
        ADCSRA         |= (1 << ADSC);                    // Start Conversion
        while (ADCSRA & (1 << ADSC));                // Conversion done when ADSC goes low again. Loop until conversion complete.
    }

    die_temp_new
    = Convert_Dietemp_To_DEGC(ADCL | (ADCH << 8));    // Ensure low byte first to guarantee data coherence (see manual).
    temperatures.die_temp
    = temperatures.die_temp * 0/8
    + die_temp_new * 8/8;                            // 7/8 old + 1/8 new IIR filter
}
/**************************************************************************
*       Convert Die Temperature ADC Reading to °C                         *
***************************************************************************/
double Convert_Dietemp_To_DEGC(uint16_t DIE_ADC_Val)
{
    return (DIE_ADC_Val - DIE_TEMP_ADC_OFS) * DIE_TEMP_ADC_SPAN;
}
/**************************************************************************
*  Get State of the Dropout Detector                                      *
***************************************************************************/
void scpi_get_dropout_state_q(char *arg, IO_pointers_t IO)
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
void scpi_get_range_q(char *arg, IO_pointers_t IO)
{
if (settings.range)
    scpi_prStr_P(PSTR("1\n"), IO.USB_stream);
else
    scpi_prStr_P(PSTR("0\n"), IO.USB_stream);
}
/**************************************************************************
*  Lock out the Touch Pad                                                 *
***************************************************************************/
void scpi_lock_touchpad(char *arg, IO_pointers_t IO)
{
    settings.touchpad_locked = true;
    draw_status_screen(get_current_value(IO), temperatures.ntc_temp, settings.touchpad_locked, settings.range, IO);
}
/**************************************************************************
*  Unlock the Touch Pad                                                   *
***************************************************************************/
void scpi_unlock_touchpad(char *arg, IO_pointers_t IO)
{
    settings.touchpad_locked = false;
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
void scpi_disable_fan(char *arg, IO_pointers_t IO)
{
    FAN_PORT &= ~FAN;
    fan_off_override = 0;  // Added 4/9/19 by FL. Re-enable automatic fan shutoff.
}
/**************************************************************************
*  Get the Fan Status                                                     *
***************************************************************************/
void scpi_get_fan_state(char *arg, IO_pointers_t IO)
{
    if (FAN_PIN & FAN)
        scpi_prStr_P(PSTR("1\n"), IO.USB_stream);
    else
        scpi_prStr_P(PSTR("0\n"), IO.USB_stream);
}
/**************************************************************************
*  Compute a Checksum                                                     *
***************************************************************************/
uint16_t get_checksum(char * cal_date_string, char * val_cal_string, uint16_t dac_cal_value)
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
/**************************************************************************
*  *OPC (Operation Complete Query) function                               *
***************************************************************************/
void st_OPC_q(char *arg, IO_pointers_t IO)
{
    fprintf(IO.USB_stream, "1\n");
}
/**************************************************************************
*  VERSION? function                                                      *
***************************************************************************/
void scpi_get_version_q(char *arg, IO_pointers_t IO)
{
    scpi_prStr_P(PSTR(FIRMWARE_VERSION),IO.USB_stream);
    scpi_prStr_P(PSTR("\n"), IO.USB_stream);
}
/****************************************************************************
*  Write to the bootloader start address                                    *
*****************************************************************************/
void sys_rst_btloader(char *arg, IO_pointers_t IO)
{
    Jump_To_Bootloader();
}