/****************************************************************************
 *                                                                          *
 *  Awesomerfaucet Sensor                                                   *
 *                                                                          *
 ****************************************************************************/
#include "Awesomerfaucet_sensor.h"
/****************************************************************************
*  Get Sensor Range Reading                                                 *
*****************************************************************************/
void process_sensor()
{
	uint8_t                 isReady;
	VL53L4CD_ResultsData_t  results;

    VL53L4CD_CheckForDataReady(0x52, &isReady);
    if(isReady)
    {
        VL53L4CD_ClearInterrupt(0x52);      // (Mandatory) Clear HW interrupt to restart measurements
        VL53L4CD_GetResult(0x52, &results); // Read measured distance. RangeStatus = 0 means valid data
    }
}

// results.range_status,
// results.distance_mm,
// results.signal_per_spad_kcps);