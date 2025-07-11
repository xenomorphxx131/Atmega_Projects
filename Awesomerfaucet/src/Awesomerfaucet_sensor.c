/****************************************************************************
 *                                                                          *
 *  Awesomerfaucet Sensor                                                   *
 *                                                                          *
 ****************************************************************************/
#include "Awesomerfaucet_sensor.h"

extern float iir_alpha;
extern float iir_beta;
extern float iir_gain;
extern float max_distance_leakage; // For example: 1mm / 30 seconds. 30 seconds ≈ 600 cycles at 50ms/cycle. 1(mm) / 600 = 0.001666.
float threshold_mm;
float distance_mm;
float distance_mm_m1 = 100;
float distance_mm_m2 = 100;
float max_distance_mm = 0;
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
        distance_mm_m2 = distance_mm_m1;
        distance_mm_m1 = distance_mm;
        distance_mm = (float)results.distance_mm / iir_gain - iir_alpha*distance_mm_m1 - iir_beta*distance_mm_m2;
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