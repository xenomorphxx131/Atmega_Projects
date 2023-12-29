/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Range                                                     *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_range.h"
double largest_reading = 0; 		// Extern'ed in timers to create "leakage"
bool foot_present = false;			// Extern'ed in water to enable water

// #define iir_value 235 // Was 0.88 which is more like 225

/****************************************************************************
*    Process Range Reading                                                  *
*****************************************************************************/
void process_range_reading()
{
	static uint8_t steady_foot_counter = 0;
	static double IIR_range_reading = 0;
    bool foot_present_now = false;
	uint8_t iir_value = 0;
	
	iir_value = get_IIR_value();

    if (range_measurement_ready())
    {
        IIR_range_reading = iir_value * (IIR_range_reading / 256) + (256 - iir_value) * ((double)get_range() / 256);
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
}