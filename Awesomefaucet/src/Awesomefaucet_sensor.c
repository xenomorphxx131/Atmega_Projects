/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Sensor                                                    *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_sensor.h"

/**************************************************************************
*  Range Reading                                                          *
***************************************************************************/
uint8_t get_range()
{
    I2C16_Write_Byte( VL6180X_ADDR7b, VL6180X_REG_SYSTEM_INTERRUPT_CLEAR, VL6180X_CLEAR_RANGE_INT );
    return I2C16_Read_Byte( VL6180X_ADDR7b, VL6180X_REG_RESULT_RANGE_RAW );
}
/**************************************************************************
*  Start Measurement                                                      *
***************************************************************************/
void start_range_measurement()
{
    I2C16_Write_Byte( VL6180X_ADDR7b, VL6180X_REG_SYSRANGE_START, VL6180X_SYSRANGE_STARTSTOP );
}
/**************************************************************************
*  Measurement Ready                                                      *
***************************************************************************/
bool range_measurement_ready()
{
    return (I2C16_Read_Byte( VL6180X_ADDR7b, VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO ) & 0x07)  == 4;
}
/**************************************************************************
*  Measurement Running                                                    *
***************************************************************************/
bool range_sensor_busy()
{
    return (I2C16_Read_Byte( VL6180X_ADDR7b, VL6180X_REG_RESULT_RANGE_STATUS ) & 0x01) == 0;
}
/**************************************************************************
*  Blocking Get ALS Reading                                               *
***************************************************************************/
uint16_t get_als_blocking()
{
    while(ALS_sensor_busy()) {;}    // Just wait it out, it will finish.
    start_ALS_measurement();        // Kick off a fresh reading and
    while(ALS_sensor_busy()) {;}    // Wait it out
    return read_als();
}
/**************************************************************************
*  ALS Reading                                                            *
***************************************************************************/
uint16_t read_als()
{
    I2C16_Write_Byte( VL6180X_ADDR7b, VL6180X_REG_SYSTEM_INTERRUPT_CLEAR, VL6180X_CLEAR_ALS_INT );
    return I2C16_Read_Word( VL6180X_ADDR7b, VL6180X_REG_RESULT_ALS );
}
/**************************************************************************
*  Start Measurement                                                      *
***************************************************************************/
void start_ALS_measurement()
{
    I2C16_Write_Byte( VL6180X_ADDR7b, VL6180X_REG_SYSALS_START, VL6180X_SYSALS_STARTSTOP );
}
/**************************************************************************
*  Measurement Ready                                                      *
***************************************************************************/
bool ALS_measurement_ready()
{
    return (I2C16_Read_Byte( VL6180X_ADDR7b, VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO ) & 0x38)  == 4 << 3;
}
/**************************************************************************
*  Measurement Running                                                    *
***************************************************************************/
bool ALS_sensor_busy()
{
    return (I2C16_Read_Byte( VL6180X_ADDR7b, VL6180X_REG_RESULT_ALS_STATUS ) & 0x01) == 0;
}