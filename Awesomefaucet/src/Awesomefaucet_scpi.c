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
extern int error_count;
extern SCPI_Error_t error_log[ERROR_QUEUE_LENGTH];
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
void setup_scpi_commands(SCPI_Node_t **scpi_nodes, IO_pointers_t IO)
{
    OPEN_SCPI_REGISTRY();

  /*REGISTER_SCPI(.node,           .value,                             .parent,    .function,                              .implied)
    ------------------------------------------------------------------------------------------------------------------------------- */
    REGISTER_SCPI(OPC,              "*OPC?",                            NULL,       &st_OPC_q,                              false);
    REGISTER_SCPI(IDN,              "*IDN?",                            NULL,       &scpi_IDN_q,                            false);
    REGISTER_SCPI(CLRI2C,           "CLRI2C",                           NULL,       &clr_i2c,                               false);
    REGISTER_SCPI(DEBUG,            "DEBUG?",                           NULL,       &debug,                                 false);

    REGISTER_SCPI(SYSTEM,           "SYSTem",                           NULL,       NULL,                                   true);
    REGISTER_SCPI(RST,                  "RST",                          &SYSTEM,    NULL,                                   false);
    REGISTER_SCPI(BTLOADER,                 "BTLOader",                 &RST,       &sys_rst_btloader,                      false);
    REGISTER_SCPI(ERROR,                "ERRor?",                       &SYSTEM,    &sys_error_q,                           false);
    REGISTER_SCPI(VERSION,              "VERSion?",                     &SYSTEM,    &scpi_get_version_q,                    false);
    REGISTER_SCPI(RECORD,               "RECORD",                       &SYSTEM,    &scpi_record,                           false);

    REGISTER_SCPI(GET,              "GET",                              NULL,       NULL,                                   false);
    REGISTER_SCPI(RANGEq,               "RANGE?",                       &GET,       &scpi_get_range_q,                      false);
    REGISTER_SCPI(IIR_ALPHAq,           "IIR_ALPHA?",                   &GET,       &scpi_get_IIR_alpha_q,                  false);
    REGISTER_SCPI(IIR_BETAq,            "IIR_BETA?",                    &GET,       &scpi_get_IIR_beta_q,                   false);
    REGISTER_SCPI(IIR_GAINq,            "IIR_GAIN?",                    &GET,       &scpi_get_IIR_gain_q,                   false);
    REGISTER_SCPI(LASER_POWERq,         "LASERPOWER?",                  &GET,       &scpi_get_laserpower_q,                 false);
    REGISTER_SCPI(DET_THRESHOLDq,       "DETECTION_THRESHOLD_MM?",      &GET,       &scpi_get_detection_threshold_mm_q,     false);
    REGISTER_SCPI(MAX_DIS_LEAKAGEq,     "MAX_DISTANCE_LEAKAGE?",        &GET,       &scpi_get_max_distance_leakage_q,       false);
    REGISTER_SCPI(WATER_DEB_TIMEOUTq,   "WATER_DEBOUNCE_TIMEOUT?",      &GET,       &scpi_get_water_debounce_timeout_q,     false);
    REGISTER_SCPI(BLACKBOXq,            "BLACKBOX?",                    &GET,       &scpi_get_blackbox_q,                   false);
    REGISTER_SCPI(MX_DIST_MM_RST_RATEq, "MAX_DISTANCE_MM_RESET_RATE?",  &GET,       &scpi_get_max_distance_mm_reset_rate_q, false);
    REGISTER_SCPI(RANGESq,              "RANGE_AND_MAXRANGE?",          &GET,       &scpi_get_range_and_maxrange_q,         false);
    REGISTER_SCPI(WATER_STATEq,         "WATER_STATE?",                 &GET,       &scpi_water_state_q,                    false);

    REGISTER_SCPI(SET,              "SET",                              NULL,       NULL,                                   false);
    REGISTER_SCPI(IIR_ALPHA,            "IIR_ALPHA",                    &SET,       &scpi_set_IIR_alpha,                    false);
    REGISTER_SCPI(IIR_BETA,             "IIR_BETA",                     &SET,       &scpi_set_IIR_beta,                     false);
    REGISTER_SCPI(LASER_POWER,          "LASERPOWER",                   &SET,       &scpi_set_laserpower,                   false);
    REGISTER_SCPI(DET_THRESHOLD,        "DETECTION_THRESHOLD_MM",       &SET,       &scpi_set_detection_threshold_mm,       false);
    REGISTER_SCPI(MAX_DIS_LEAKAGE,      "MAX_DISTANCE_LEAKAGE",         &SET,       &scpi_set_max_distance_leakage,         false);
    REGISTER_SCPI(WATER_DEB_TIMEOUT,    "WATER_DEBOUNCE_TIMEOUT",       &SET,       &scpi_set_water_debounce_timeout,       false);
    REGISTER_SCPI(MX_DIST_MM_RST_RATE,  "MAX_DISTANCE_MM_RESET_RATE",   &SET,       &scpi_set_max_distance_mm_reset_rate,   false);
    REGISTER_SCPI(WATER_v,              "WATER",                        &SET,       NULL,                                   false);
    REGISTER_SCPI(ON,                       "ON",                       &WATER_v,   &scpi_water_on,                         false);
    REGISTER_SCPI(OFF,                      "OFF",                      &WATER_v,   &scpi_water_off,                        false);
    REGISTER_SCPI(AUTO,                     "AUTO",                     &WATER_v,   &scpi_water_auto,                       false);

    CLOSE_SCPI_REGISTRY();
}
/****************************************************************************
*  *OPC (Operation Complete Query) function                                 *
*****************************************************************************/
void st_OPC_q ( char *arg, IO_pointers_t IO )
{
    scpi_prStr_P(PSTR("1"), IO);
    scpi_prStr_P_cr_nl(IO);
}
/****************************************************************************
*  Write to the bootloader start address                                    *
*****************************************************************************/
void sys_rst_btloader(char *arg, IO_pointers_t IO)
{
    Jump_To_Bootloader();
}
/****************************************************************************
*  VERSION? function                                                        *
*****************************************************************************/
void scpi_get_version_q( char *arg, IO_pointers_t IO )
{
    scpi_prStr_P(PSTR(FIRMWARE_VERSION),IO);
    scpi_prStr_P_cr_nl(IO);
}
/****************************************************************************
*  *IDN? function                                                           *
*****************************************************************************/
void scpi_IDN_q( char *arg, IO_pointers_t IO )
{
    scpi_prStr_P(PSTR(COMPANY_NAME), IO);
    scpi_prStr_P(PSTR(" | "), IO);
    scpi_prStr_P(PSTR(PROJECT_NAME), IO);
    scpi_prStr_P(PSTR(" | "), IO);
    scpi_prStr_P(PSTR("Firmware Revision: "), IO);
    scpi_prStr_P(PSTR(FIRMWARE_VERSION), IO);
    scpi_prStr_P_cr_nl(IO);
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
*  SCPI Get Range Reading                                                   *
*****************************************************************************/
void scpi_get_range_q(char *arg, IO_pointers_t IO)
{
    fprintf(IO.USB_stream, "%fmm", (double)distance_mm);
    scpi_prStr_P_cr_nl(IO);
}
/****************************************************************************
*  SCPI Get Range and Max Range Readings - comma separated                  *
*****************************************************************************/
void scpi_get_range_and_maxrange_q(char *arg, IO_pointers_t IO)
{
    scpi_prStr_P(PSTR("{\"RANGE\":"), IO);
    fprintf(IO.USB_stream, "%f", (double)distance_mm);
    scpi_prStr_P(PSTR(",\"MAXRANGE\":"), IO);
    fprintf(IO.USB_stream, "%f", (double)max_distance_mm);
    scpi_prStr_P(PSTR("}"), IO);
    scpi_prStr_P_cr_nl(IO);
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
    fprintf(IO.USB_stream, "%u", WATERPORT & WATER ? 1 : 0);
    scpi_prStr_P_cr_nl(IO);
}
/****************************************************************************
*  Store Laser Power to EEPROM                                              *
*****************************************************************************/
void scpi_set_laserpower(char *arg, IO_pointers_t IO)
{
    char *endptr;
    uint8_t value;

    if (strlen(arg) == 0)
        scpi_add_error_too_few_parameters();
    else
    {
        value = (uint8_t)strtol(arg, &endptr, 10);
        eeprom_busy_wait();
        eeprom_write_block((const void *)&value, &LASER_POWER, sizeof(uint8_t));
        retrieve_laserpower_setting();
    }
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
void scpi_get_laserpower_q(char *arg, IO_pointers_t IO)
{
    fprintf(IO.USB_stream, "%u", laser_power);
    scpi_prStr_P_cr_nl(IO);
}
/****************************************************************************
*  Store Water Debounce Timeout to EEPROM                                   *
*****************************************************************************/
void scpi_set_water_debounce_timeout(char *arg, IO_pointers_t IO)
{
    char *endptr;
    uint16_t value;

    if (strlen(arg) == 0)
        scpi_add_error_too_few_parameters();
    else
    {
        value = (uint16_t)strtol(arg, &endptr, 10);
        eeprom_busy_wait();
        eeprom_write_block((const void *)&value, &WATER_DEBOUNCE_TIMEOUT, sizeof(uint16_t));
        retrieve_water_debounce_timeout();
    }
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
    fprintf(IO.USB_stream, "%u", water_debounce_timeout);
    scpi_prStr_P_cr_nl(IO);
}
/****************************************************************************
*  Store IIR Factor Alpha to EEPROM                                         *
*****************************************************************************/
void scpi_set_IIR_alpha( char *arg, IO_pointers_t IO )
{
    char *endptr;
    float value;

    if (strlen(arg) == 0)
        scpi_add_error_too_few_parameters();
    else
    {
        value = strtod(arg, &endptr);
        eeprom_busy_wait();
        eeprom_write_block((const void *)&value, &IIR_ALPHA, sizeof(float));
        retrieve_IIR_alpha();
    }
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
    fprintf(IO.USB_stream, "%f", (double)iir_alpha);
    scpi_prStr_P_cr_nl(IO);
}
/****************************************************************************
*  Store IIR Factor Beta to EEPROM                                          *
*****************************************************************************/
void scpi_set_IIR_beta( char *arg, IO_pointers_t IO )
{
    char *endptr;
    float value;

    if (strlen(arg) == 1)
        scpi_add_error_too_few_parameters();
    else
    {
        value = strtod(arg, &endptr);
        eeprom_busy_wait();
        eeprom_write_block((const void *)&value, &IIR_BETA, sizeof(float));
        retrieve_IIR_beta();
    }
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
    fprintf(IO.USB_stream, "%f", (double)iir_beta);
    scpi_prStr_P_cr_nl(IO);
}
/****************************************************************************
*  Store Detection Threshold to EEPROM                                      *
*****************************************************************************/
void scpi_set_detection_threshold_mm( char *arg, IO_pointers_t IO )
{
    char *endptr;
    float value;

    if (strlen(arg) == 0)
        scpi_add_error_too_few_parameters();
    else
    {
        value = strtod(arg, &endptr);
        eeprom_busy_wait();
        eeprom_write_block((const void *)&value, &THRESHOLD_MM, sizeof(float));
        retrieve_detection_threshold_mm();
    }
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
    fprintf(IO.USB_stream, "%f", (double)threshold_mm);
    scpi_prStr_P_cr_nl(IO);
}
/****************************************************************************
*  Store Max Distance Leakage to EEPROM                                     *
*****************************************************************************/
void scpi_set_max_distance_leakage( char *arg, IO_pointers_t IO )
{
    char *endptr;
    float value;

    if (strlen(arg) == 0)
        scpi_add_error_too_few_parameters();
    else
    {
        value = strtod(arg, &endptr);
        eeprom_busy_wait();
        eeprom_write_block((const void *)&value, &MAX_DISTANCE_LEAKAGE, sizeof(float));
        retrieve_max_distance_leakage();
    }
}
/****************************************************************************
*  Store Max Distance Reset Rate to EEPROM                                  *
*****************************************************************************/
void scpi_set_max_distance_mm_reset_rate( char *arg, IO_pointers_t IO )
{
    char *endptr;
    float value;

    if (strlen(arg) == 0)
        scpi_add_error_too_few_parameters();
    else
    {
        value = strtod(arg, &endptr);
        eeprom_busy_wait();
        eeprom_write_block((const void *)&value, &MAX_DISTANCE_MM_RESET_RATE, sizeof(float));
        retrieve_max_distance_mm_reset_rate();
    }
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
    fprintf(IO.USB_stream, "%f", (double)max_distance_leakage);
    scpi_prStr_P_cr_nl(IO);
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
    fprintf(IO.USB_stream, "%f", (double)iir_gain);
    scpi_prStr_P_cr_nl(IO);
}
/****************************************************************************
*  SCPI Get Blackbox Distances (mm)                                         *
*****************************************************************************/
void scpi_get_blackbox_q( char *arg, IO_pointers_t IO )
{
    scpi_prStr_P(PSTR("{\"RECORDING\": "), IO);
    fprintf(IO.USB_stream, "%s, ", record ? "True" : "False");
    scpi_prStr_P(PSTR("\"DISTANCE_mm\": ["), IO);
    uint8_t count = 0;
    while (count < BLACKBOX_BUFFER_SIZE)
        fprintf(IO.USB_stream, "%.3f,", (double)blackbox[(blackbox_index + count++) & (BLACKBOX_BUFFER_SIZE - 1)].distance_mm);
    scpi_prStr_P(PSTR("], \"MAX_DISTANCE_mm\": ["), IO);
    count = 0;
    while (count < BLACKBOX_BUFFER_SIZE)
        fprintf(IO.USB_stream, "%.3f,", (double)blackbox[(blackbox_index + count++) & (BLACKBOX_BUFFER_SIZE - 1)].max_distance_mm);
    scpi_prStr_P(PSTR("]}"), IO);
    scpi_prStr_P_cr_nl(IO);
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
    fprintf(IO.USB_stream, "%f", (double)max_distance_mm_reset_rate);
    scpi_prStr_P_cr_nl(IO);
}
/****************************************************************************
*  Debug function                                                           *
*****************************************************************************/
void debug(char *arg, IO_pointers_t IO)
{
    // fprintf(IO.USB_stream, "%s\n", ramStr);     // Should always work
    // scpi_prStr_P(flashStr, IO);
}