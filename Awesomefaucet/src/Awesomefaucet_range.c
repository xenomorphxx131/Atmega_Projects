/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Range                                                     *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_range.h"

bool foot_present = false;				// Extern'ed in water to enable water
extern bool range_leakage_timeout;

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
	static uint32_t largest_reading = 0;
	static uint32_t IIR_range_reading = 0;

    if (range_measurement_ready())
    {
		/********************
		 *    IIR Filter    *
		 ********************/	
        IIR_range_reading = (iir_value * IIR_range_reading + (256 - iir_value) * SCALEFACTOR * get_range()) / 256;
		
        if (IIR_range_reading > largest_reading)
            largest_reading = IIR_range_reading;

        foot_present = (IIR_range_reading < (largest_reading - DETECTION_HEIGHT_SCALED)) ? true : false;
    }
    else if (!range_sensor_busy())
        start_range_measurement();
	
	/***********************************************************
	 *    Apply leakage or "downward" bias to largest reading  *
	 ***********************************************************/
	if (range_leakage_timeout)
	{
		if (largest_reading > LEAKAGE_RATE)
			largest_reading -= LEAKAGE_RATE; // Leaky integrator leakage rate
		range_leakage_timeout = false;
	}
}