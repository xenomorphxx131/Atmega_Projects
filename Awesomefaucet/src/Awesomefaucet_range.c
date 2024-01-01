/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Range                                                     *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_range.h"

bool foot_present = false;				// Extern'ed in water to enable water
extern bool leakage_update_time;
/************************************
 *    IIR Value from EEPROM         *
 ************************************/
extern uint8_t iir_value;
// iir_value is an 8 bit number so 256 - minus this is the input or "new data" value coefficient.
// For iir_value use something heavy like 235 so 91.7% - this is the "present value" coefficient.		
/************************************
 *                                  *
 ************************************/
#define SCALEFACTOR 65536
#define LEAKAGE_RATE 16 				// When was floats was 0.00024 or 1 part in 4167, now 16/65536
#define DETECTION_HEIGHT_mm 2.5
#define DETECTION_HEIGHT_SCALED round(DETECTION_HEIGHT_mm * SCALEFACTOR)
/****************************************************************************
*    Process Range Reading                                                  *
*****************************************************************************/
void process_range_reading()
{
	static long long max_reading = 0;
	static long long IIR_range_reading = 0;

    if (range_measurement_ready())
    {
		IIR_range_reading = (iir_value * IIR_range_reading + (256 - iir_value) * get_range() * SCALEFACTOR ) / 256;	
        if (IIR_range_reading > max_reading) max_reading = IIR_range_reading;
        foot_present = (IIR_range_reading < (max_reading - DETECTION_HEIGHT_SCALED)) ? true : false;
    }
    if (!range_sensor_busy()) start_range_measurement();
	/***********************************************************
	 *    Apply leakage or "downward" bias to largest reading  *
	 ***********************************************************/
	if (leakage_update_time)
	{
		if (max_reading - LEAKAGE_RATE >= 0)
			max_reading -= LEAKAGE_RATE;
		leakage_update_time = false;
	}
}