/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Sensor                                                    *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_sensor.h"
/**************************************************************************
*  Range Reading                                                          *
***************************************************************************/
// uint8_t get_range()
// {
    // I2C16_Write_Byte( VL6180X_ADDR7b, VL6180X_SYSTEM__INTERRUPT_CLEAR, VL6180X_CLEAR_RANGE_INT );
    // return I2C16_Read_Byte( VL6180X_ADDR7b, VL6180X_RESULT__RANGE_RAW );
// }
/**************************************************************************
*  Range Reading Blocking                                                 *
***************************************************************************/
uint8_t read_range_blocking(void)
{
    uint8_t range;
    
    while (! (I2C16_Read_Byte( VL6180X_ADDR7b, VL6180X_RESULT__RANGE_STATUS)) & VL6180X_RESULT__RANGE_DEVICE_READY);                    // wait for device to be ready for range measurement
    I2C16_Write_Byte( VL6180X_ADDR7b, VL6180X_SYSRANGE__START, VL6180X_SYSRANGE_STARTSTOP);                                             // Start a range measurement
    while (! (I2C16_Read_Byte( VL6180X_ADDR7b, VL6180X_RESULT__INTERRUPT_STATUS_GPIO) & VL6180X_NEW_SAMPLE_READY_THRESHOLD_EVENT));     // Poll until bit 2 is set
    range = I2C16_Read_Byte( VL6180X_ADDR7b, VL6180X_RESULT__RANGE_VAL );                                                               // read range in mm                 //read8(VL6180X_REG_RESULT_RANGE_VAL);
    I2C16_Write_Byte( VL6180X_ADDR7b, VL6180X_SYSTEM__INTERRUPT_CLEAR, VL6180X_CLEAR_ALL_INTS);                                         // clear all interrupts
    return range;
}
/**************************************************************************
*  Start Measurement                                                      *
***************************************************************************/
// void start_range_measurement()
// {
    // I2C16_Write_Byte( VL6180X_ADDR7b, VL6180X_SYSRANGE__START, VL6180X_SYSRANGE_STARTSTOP );
// }
/**************************************************************************
*  Measurement Ready                                                      *
***************************************************************************/
// bool range__measurement_ready()
// {
    // return (I2C16_Read_Byte( VL6180X_ADDR7b, VL6180X_RESULT__INTERRUPT_STATUS_GPIO ) & 0x07)  == 4;
    // // return (I2C16_Read_Byte( VL6180X_ADDR7b, VL6180X_RESULT__RANGE_STATUS ) & 0x02)  == 0x02;
// }
/**************************************************************************
*  Measurement Running                                                    *
***************************************************************************/
// bool range__range_device_ready()
// {
    // return (I2C16_Read_Byte( VL6180X_ADDR7b, VL6180X_RESULT__RANGE_STATUS ) & VL6180X_RESULT__RANGE_DEVICE_READY) == VL6180X_RESULT__RANGE_DEVICE_READY;
// }
/**************************************************************************
*  Blocking Get ALS Reading                                               *
***************************************************************************/
// uint16_t get_ALS_blocking()
// {	// This is a blocking function
	// Be sure to call cli() entering and sei() and exiting calls to this.
	// Possible deadlock if don't?

    // while(ALS_sensor_busy()) {;}    		// Just wait it out, it will finish.
    // start_ALS_measurement();        		// Kick off a fresh reading and
	// while(!ALS_measurement_ready()) {;}		// Wait for the new reading
	// return read_ALS();
// }
/**************************************************************************
*  ALS Reading                                                            *
***************************************************************************/
uint16_t read_ALS()
{
    I2C16_Write_Byte( VL6180X_ADDR7b, VL6180X_SYSTEM__INTERRUPT_CLEAR, VL6180X_CLEAR_ALS_INT );
    return I2C16_Read_Word( VL6180X_ADDR7b, VL6180X_RESULT__ALS_VAL );
}
/**************************************************************************
*  Start Measurement                                                      *
***************************************************************************/
void start_ALS_measurement()
{
    I2C16_Write_Byte( VL6180X_ADDR7b, VL6180X_SYSALS__START, VL6180X_SYSALS_STARTSTOP );
}
/**************************************************************************
*  Measurement Ready                                                      *
***************************************************************************/
bool ALS_measurement_ready()
{
    return (I2C16_Read_Byte( VL6180X_ADDR7b, VL6180X_RESULT__INTERRUPT_STATUS_GPIO ) & 0x38)  == (4 << 3);
}
/**************************************************************************
*  Measurement Running                                                    *
***************************************************************************/
bool ALS_sensor_busy()
{
    return (I2C16_Read_Byte( VL6180X_ADDR7b, VL6180X_RESULT__ALS_STATUS ) & 0x01) == 0;
}