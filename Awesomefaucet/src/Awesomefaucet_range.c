/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Range                                                     *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_range.h"

bool foot_present = false;		// Extern'ed in water to enable water
extern bool range_leakage_timeout;
extern float iir_value;

// #define iir_value 235 		// Was 0.88 which is more like 225

/****************************************************************************
*    Process Range Reading                                                  *
*****************************************************************************/
void process_range_reading()
{
	static float largest_reading = 0;
	static float IIR_range_reading = 0;
	static uint8_t steady_foot_counter = 0;
    bool foot_present_now = false;
	// float iir_value = 0;
	
	// iir_value = (float)get_IIR_value();

    if (range_measurement_ready())
    {
        IIR_range_reading = iir_value * IIR_range_reading + (1 - iir_value) * get_range();
        if (IIR_range_reading > largest_reading)
            largest_reading = IIR_range_reading;

        foot_present_now = (IIR_range_reading < largest_reading - DETECTION_HEIGHT_mm) ? true : false;
        
        if (!foot_present_now)
        {
            steady_foot_counter = 0;
            foot_present = false;
        }
        
        if (foot_present_now && (steady_foot_counter <= STEADY_FOOT_COUNT))
            steady_foot_counter++;
        
        if (steady_foot_counter > STEADY_FOOT_COUNT)
            foot_present = true;
    }
    else if (!range_sensor_busy())
        start_range_measurement();
	
	/***********************************************************
	 *    Apply leakage or "downward" bias to largest reading  *
	 ***********************************************************/
	if (range_leakage_timeout)
	{
		if (largest_reading >= LEAKAGE_RATE && largest_reading > 0)
			largest_reading -= LEAKAGE_RATE; // Leaky integrator leakage rate
		range_leakage_timeout = false;
	}
}