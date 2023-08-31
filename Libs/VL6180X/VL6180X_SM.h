#ifndef VL6180X_SM_H_
#define VL6180X_SM_H_

#include <i2c_twi.h>

#define VL6180X_ADDR7b                              0x29  // The fixed 7 bit I²C addres

#define VL6180X_REG_IDENTIFICATION_MODEL_ID         0x000
#define VL6180X_REG_SYSTEM_INTERRUPT_CONFIG         0x014
#define VL6180X_REG_SYSTEM_INTERRUPT_CLEAR          0x015
#define VL6180X_REG_SYSTEM_FRESH_OUT_OF_RESET       0x016
#define VL6180X_REG_SYSRANGE_START                  0x018
#define VL6180X_REG_SYSALS_START                    0x038
#define VL6180X_REG_SYSALS_ANALOGUE_GAIN            0x03F
#define VL6180X_REG_SYSALS_INTEGRATION_PERIOD_HI    0x040
#define VL6180X_REG_SYSALS_INTEGRATION_PERIOD_LO    0x041
#define VL6180X_REG_RESULT_RANGE_STATUS             0x04D
#define VL6180X_REG_RESULT_ALS_STATUS               0x04E
#define VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO    0x04F
#define VL6180X_REG_RESULT_ALS                      0x050
#define VL6180X_REG_RESULT_RANGE                    0x062
#define VL6180X_REG_RESULT_RANGE_RAW                0x064

#define VL6180X_ALS_GAIN_1                          0x06  // 1x gain
#define VL6180X_ALS_GAIN_1_25                       0x05  // 1.25x gain
#define VL6180X_ALS_GAIN_1_67                       0x04  // 1.67x gain
#define VL6180X_ALS_GAIN_2_5                        0x03  // 2.5x gain
#define VL6180X_ALS_GAIN_5                          0x02  // 5x gain
#define VL6180X_ALS_GAIN_10                         0x01  // 10x gain
#define VL6180X_ALS_GAIN_20                         0x00  // 20x gain
#define VL6180X_ALS_GAIN_40                         0x07  // 40x gain

#define VL6180X_ERROR_NONE                          0   // Success!
#define VL6180X_ERROR_SYSERR_1                      1   // System error
#define VL6180X_ERROR_SYSERR_5                      5   // Sysem error
#define VL6180X_ERROR_ECEFAIL                       6   // Early convergence estimate fail
#define VL6180X_ERROR_NOCONVERGE                    7   // No target detected
#define VL6180X_ERROR_RANGEIGNORE                   8   // Ignore threshold check failed
#define VL6180X_ERROR_SNR                           11  // Ambient conditions too high
#define VL6180X_ERROR_RAWUFLOW                      12  // Raw range algo underflow
#define VL6180X_ERROR_RAWOFLOW                      13  // Raw range algo overflow
#define VL6180X_ERROR_RANGEUFLOW                    14  // Raw range algo underflow
#define VL6180X_ERROR_RANGEOFLOW                    15  // Raw range algo overflow

#define VL6180X_CLEAR_RANGE_INT                     _BV(0)
#define VL6180X_CLEAR_ALS_INT                       _BV(1)
#define VL6180X_CLEAR_ERROR_INT                     _BV(2)
#define VL6180X_SYSRANGE_STARTSTOP                  _BV(0)
#define VL6180X_SYSALS_STARTSTOP                    _BV(0)

void VL6180X_Setup(void);

#endif