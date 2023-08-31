#include "VL6180X_SM.h"

/**************************************************************************
*  Super Secret Private Registers from AN4545 Page 24                     *
***************************************************************************/
void VL6180X_Setup()
{
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x207, 0x01);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x208, 0x01);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x096, 0x00);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x097, 0xfd);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0e3, 0x00);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0e4, 0x04);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0e5, 0x02);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0e6, 0x01);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0e7, 0x03);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0f5, 0x02);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0d9, 0x05);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0db, 0xce);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0dc, 0x03);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0dd, 0xf8);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x09f, 0x00);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0a3, 0x3c);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0b7, 0x00);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0bb, 0x3c);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0b2, 0x09);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0ca, 0x09);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x198, 0x01);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x1b0, 0x17);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x1ad, 0x00);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x0ff, 0x05);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x100, 0x05);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x199, 0x05);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x1a6, 0x1b);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x1ac, 0x3e);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x1a7, 0x1f);
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x030, 0x00);
                                                        // Recommended : Public registers - See data sheet for more detail
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x011, 0x10);     // Enables polling for 'New Sample ready'
                                                        // when measurement completes
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x10a, 255);      // Set the averaging sample period
                                                        // (compromise between lower noise and
                                                        // increased execution time) (was 0x30)
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x03f, 0x46);     // Sets the light and dark gain (upper
                                                        // nibble). Dark gain should not be
                                                        // changed.
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x031, 0);        // sets the # of range measurements after
                                                        // which auto calibration of system is
                                                        // performed, 0 = off (was 255)
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x040, 0x63);     // Set ALS integration time to 100ms
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x02e, 0x01);     // perform a single temperature calibration
                                                        // of the ranging sensor
                                                        // Optional: Public registers - See data sheet for more detail
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x01b, 0);        // Set default ranging inter-measurement
                                                        // period to 10ms (was 0x09 for 100ms)
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x03e, 0x31);     // Set default ALS inter-measurement period
                                                        // to 500ms
    I2C16_Write_Byte( VL6180X_ADDR7b, 0x014, 0x24);     // Configures interrupt on 'New Sample
                                                        // Ready threshold event' for both ALS and Range
}