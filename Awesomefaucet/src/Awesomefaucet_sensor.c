/****************************************************************************
 *                                                                          *
 *  Awesomerfaucet Sensor                                                   *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_sensor.h"

extern float iir_alpha;
extern float iir_beta;
extern float iir_gain;
extern float max_distance_mm_reset_rate;    // for example: 0.1 is 10% of the difference observed.
extern float max_distance_leakage;          // For example: 1(mm) / 30 seconds. 30 seconds ≈ 600 cycles at 50ms/cycle. 1(mm) / 600 = 0.001666.
float threshold_mm;
float distance_mm;
float max_distance_mm = 0;
float distance_mm_m1 = 100;
float distance_mm_m2 = 100;
bool foot_present = false;
Blackbox blackbox[BLACKBOX_BUFFER_SIZE];
uint8_t blackbox_index = 0;
bool record = 1;
/****************************************************************************
*  Get Sensor Range Reading                                                 *
*****************************************************************************/
void process_sensor()
{
	uint8_t isReady;
    uint8_t reading_mm;
    
    #ifdef VL6180X
        I2C_16BITSUB_Read_Byte(VL6180X_ADDR7, VL6180X_RESULT__INTERRUPT_STATUS_GPIO, &isReady);
        if(isReady & VL6180X_NEW_SAMPLE_READY_THRESHOLD_EVENT)               // Should happen on a roughly 24ms cadence as controlled by the sensor settings.
    #elif defined(VL534CD)
        VL53L4CD_ResultsData_t results;
        VL53L4CD_CheckForDataReady(VL53L4CD_ADDR7, &isReady);
        if(isReady)                                                         // Should happen on a roughly 50ms cadence as controlled by the sensor settings.
    #endif
    {   /****************************************************************
         *                                                              *
         * Get the sensor value and compute IIR filtered version        *
         *                                                              *
         ****************************************************************/
        distance_mm_m2 = distance_mm_m1;
        distance_mm_m1 = distance_mm;
        #ifdef VL6180X
            I2C_16BITSUB_Read_Byte(VL6180X_ADDR7, VL6180X_RESULT__RANGE_VAL, &reading_mm);
            I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_SYSTEM__INTERRUPT_CLEAR, VL6180X_CLEAR_ALL_INTS ); // clear all interrupts
            I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_SYSRANGE__START, VL6180X_SYSRANGE_STARTSTOP );
        #elif defined(VL534CD)
            VL53L4CD_GetResult(VL53L4CD_ADDR7, &results);     // Read measured distance. RangeStatus = 0 means valid data
            VL53L4CD_ClearInterrupt(VL53L4CD_ADDR7);          // (Mandatory) Clear HW interrupt to restart measurements
            reading_mm = results.distance_mm;
        #endif
        distance_mm = (float)reading_mm / iir_gain - iir_alpha*distance_mm_m1 - iir_beta*distance_mm_m2;
        /****************************************************************
         *                                                              *
         * Rapidly adjust maximum to the highest value observed         *
         *                                                              *
         ****************************************************************/
         max_distance_mm -= max_distance_leakage;
         if (distance_mm > max_distance_mm)
             max_distance_mm += max_distance_mm_reset_rate * (distance_mm - max_distance_mm); // Increase by only a portion of the difference
        /****************************************************************
         *                                                              *
         * Look for a foot                                              *
         *                                                              *
         ****************************************************************/
         foot_present = distance_mm < max_distance_mm - threshold_mm ? true : false;
        /****************************************************************
         *                                                              *
         * Black Box - Tracks readings to look for Misfires             *
         *                                                              *
         ****************************************************************/
        record = !foot_present && record; // If we stop, stay stopped until "SYSTem:RECORD" is re-enabled by SCPI
        if (record)
        {
            blackbox[blackbox_index].distance_mm = distance_mm;
            blackbox[blackbox_index].max_distance_mm = max_distance_mm;
            blackbox_index = (blackbox_index + 1) & (BLACKBOX_BUFFER_SIZE - 1);
        }
    }
}