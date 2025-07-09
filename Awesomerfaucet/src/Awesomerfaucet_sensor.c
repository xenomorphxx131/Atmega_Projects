/****************************************************************************
 *                                                                          *
 *  Awesomerfaucet Sensor                                                   *
 *                                                                          *
 ****************************************************************************/
#include "Awesomerfaucet_sensor.h"

float distance_mm;
extern uint16_t iir_alpha;

/****************************************************************************
*  Get Sensor Range Reading                                                 *
*****************************************************************************/
void process_sensor()
{
	uint8_t isReady;
	VL53L4CD_ResultsData_t results;

    VL53L4CD_CheckForDataReady(0x52, &isReady);
    if(isReady)
    {
        VL53L4CD_GetResult(0x52, &results);     // Read measured distance. RangeStatus = 0 means valid data
        VL53L4CD_ClearInterrupt(0x52);          // (Mandatory) Clear HW interrupt to restart measurements
        distance_mm = (distance_mm * (float)(65535.0f - (float)iir_alpha) + results.distance_mm * (float)iir_alpha) / 65535.0f;

    }
}