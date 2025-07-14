/****************************************************************************
 *                                                                          *
 *  Awesomerfaucet Sensor                                                   *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_sensor.h"

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
    uint8_t reading_mm;
	uint8_t status;
    I2C_16BITSUB_Read_Byte(VL6180X_ADDR7, VL6180X_RESULT__INTERRUPT_STATUS_GPIO, &status);
    if(status & VL6180X_NEW_SAMPLE_READY_THRESHOLD_EVENT)               // Should happen on a roughly 24ms cadence as controlled by the sensor settings.
    {   /****************************************************************
         *                                                              *
         * Get the sensor value and compute IIR filtered version        *
         *                                                              *
         ****************************************************************/
        distance_mm_m2 = distance_mm_m1;
        distance_mm_m1 = distance_mm;
        I2C_16BITSUB_Read_Byte(VL6180X_ADDR7, VL6180X_RESULT__RANGE_VAL, &reading_mm);
        I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_SYSTEM__INTERRUPT_CLEAR, VL6180X_CLEAR_ALL_INTS ); // clear all interrupts
        I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_SYSRANGE__START, VL6180X_SYSRANGE_STARTSTOP );
        distance_mm = (float)reading_mm / iir_gain - iir_alpha*distance_mm_m1 - iir_beta*distance_mm_m2;
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