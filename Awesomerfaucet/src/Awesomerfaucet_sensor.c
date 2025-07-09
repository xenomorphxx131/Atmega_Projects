/****************************************************************************
 *                                                                          *
 *  Awesomerfaucet Sensor                                                   *
 *                                                                          *
 ****************************************************************************/
#include "Awesomerfaucet_sensor.h"

extern uint16_t iir_alpha;
uint8_t threshold_mm = 2.0f;            // TODO extern and make an EEPROM value 
float distance_mm;
float max_distance_mm = 0;
float max_distance_leakage = 0.00167f; // max_distance_leakage = 1mm / 30 seconds. 30 seconds ≈ 600 cycles at 50ms/cycle ------- TODO make an EEPROM value
bool foot_present = false;
 
/****************************************************************************
*  Get Sensor Range Reading                                                 *
*****************************************************************************/
void process_sensor()
{
	uint8_t isReady;
	VL53L4CD_ResultsData_t results;

    VL53L4CD_CheckForDataReady(0x52, &isReady);
    if(isReady) // Should happen on a roughly 50ms cadence as controlled by the sensor settings.
        
    {   /****************************************************************
         *                                                              *
         * Get the sensor value and compute IIR filtered version        *
         *                                                              *
         ****************************************************************/
        VL53L4CD_GetResult(0x52, &results);     // Read measured distance. RangeStatus = 0 means valid data
        VL53L4CD_ClearInterrupt(0x52);          // (Mandatory) Clear HW interrupt to restart measurements
        distance_mm = (distance_mm * (float)(65535.0f - (float)iir_alpha) + results.distance_mm * (float)iir_alpha) / 65535.0f;
        /****************************************************************
         *                                                              *
         * Instantly adjust maximum to the highest value observed       *
         *                                                              *
         ****************************************************************/
         if (distance_mm > max_distance_mm)
             max_distance_mm = distance_mm;
        /****************************************************************
         *                                                              *
         * Look for a foot                                              *
         *                                                              *
         ****************************************************************/
         if (distance_mm < max_distance_mm - threshold_mm)
         {
             foot_present = true;
             max_distance_mm -= max_distance_leakage;
         }
         else
             foot_present = false;
    }
}