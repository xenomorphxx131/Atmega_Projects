/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Range                                                     *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_range.h"

double largest_reading = 0; 		// Extern'ed in timers to create "leakage"
bool foot_present = false;			// Extern'ed in water to enable water

/****************************************************************************
*    Process Range Reading                                                  *
*****************************************************************************/
void process_range_reading()
{
	static uint8_t steady_foot_counter = 0;
	static double IIR_range_reading = 0;
	
    bool foot_present_now = false;
    if (range_measurement_ready())
    {
        IIR_range_reading = 0.88 * IIR_range_reading + 0.12 * get_range();
        if (IIR_range_reading > largest_reading)
            largest_reading = IIR_range_reading;

        foot_present_now = (IIR_range_reading < largest_reading - DETECTION_HEIGHT * ONE_mm) ? true : false;
        
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