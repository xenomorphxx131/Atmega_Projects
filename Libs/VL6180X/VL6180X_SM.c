#include "VL6180X_SM.h"

/**************************************************************************
*  Super Secret Private Registers from AN4545 Page 24                     *
***************************************************************************/
void VL6180X_Setup()
{
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x207, 0x01);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x208, 0x01);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x096, 0x00);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x097, 0xfd);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0e3, 0x00);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0e4, 0x04);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0e5, 0x02);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0e6, 0x01);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0e7, 0x03);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0f5, 0x02);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0d9, 0x05);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0db, 0xce);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0dc, 0x03);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0dd, 0xf8);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x09f, 0x00);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0a3, 0x3c);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0b7, 0x00);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0bb, 0x3c);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0b2, 0x09);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0ca, 0x09);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x198, 0x01);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x1b0, 0x17);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x1ad, 0x00);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x0ff, 0x05);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x100, 0x05);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x199, 0x05);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x1a6, 0x1b);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x1ac, 0x3e);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x1a7, 0x1f);
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, 0x030, 0x00);
                                                                                                
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_SYSTEM__MODE_GPIO1, 0x10);              // Recommended : Public registers - See data sheet for more detail Enables polling for 'New Sample ready' when measurement completes
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_READOUT__AVERAGING_SAMPLE_PERIOD, 255); // Set the averaging sample period (compromise between lower noise and increased execution time) (was 0x30)
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_SYSALS__ANALOGUE_GAIN, 0x46);           // Sets the light and dark gain (upper nibble). Dark gain should not be changed.
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_SYSALS__INTEGRATION_PERIOD, 0x63);      // Set ALS integration time to 100ms perform a single temperature calibration of the ranging sensor
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_SYSALS__INTERMEASUREMENT_PERIOD, 0x31); // Set default ALS inter-measurement periodto 500ms

    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_SYSRANGE__VHV_REPEAT_RATE, 0);          // sets the # of range measurements after which auto calibration of system is performed, 0 = off (was 255)
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_SYSRANGE__VHV_RECALIBRATE, 0x01);       // One time calibrate of the VCEL laser
    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_SYSRANGE__INTERMEASUREMENT_PERIOD, 0);  // SYSRANGE__INTERMEASUREMENT_PERIOD: Time delay between measurements in Ranging continuous mode. Range 0-254 (0 = 10ms). Step size = 10ms
    // I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_SYSRANGE__MAX_CONVERGENCE_TIME, 50);    //  Maximum time to run measurement in Ranging modes. Range 1 - 63 ms (1 code = 1 ms); Measurement aborted when limit reached to aid power reduction. For example, 0x01 = 1ms, 0x0a = 10ms.

    I2C_16BITSUB_Write_Byte( VL6180X_ADDR7, VL6180X_SYSTEM__INTERRUPT_CONFIG_GPIO, 0x24);   // Configures interrupt on 'New Sample Ready threshold event' for both ALS and Range
}