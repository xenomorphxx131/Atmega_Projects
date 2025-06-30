#ifndef VL6180X_SM_H_
#define VL6180X_SM_H_

#include <i2c_twi.h>

#define VL6180X_ADDR7b                                  0x29  // The fixed 7 bit I²C addres
#define VL6180X_IDENTIFICATION__MODEL_ID                0x000
#define VL6180X_IDENTIFICATION__MODEL_REV_MAJOR         0x001
#define VL6180X_IDENTIFICATION__MODEL_REV_MINOR         0x002
#define VL6180X_IDENTIFICATION__MODULE_REV_MAJOR        0x003
#define VL6180X_IDENTIFICATION__MODULE_REV_MINOR        0x004
#define VL6180X_IDENTIFICATION__DATE_HI                 0x006
#define VL6180X_IDENTIFICATION__DATE_LO                 0x007
#define VL6180X_IDENTIFICATION__TIME_LO                 0x008
#define VL6180X_IDENTIFICATION__TIME_HI                 0x009
#define VL6180X_SYSTEM__MODE_GPIO0                      0x010
#define VL6180X_SYSTEM__MODE_GPIO1                      0x011
#define VL6180X_SYSTEM__HISTORY_CTRL                    0x012
#define VL6180X_SYSTEM__INTERRUPT_CONFIG_GPIO           0x014
#define VL6180X_SYSTEM__INTERRUPT_CLEAR                 0x015
#define VL6180X_SYSTEM__FRESH_OUT_OF_RESET              0x016
#define VL6180X_SYSTEM__GROUPED_PARAMETER_HOLD          0x017
#define VL6180X_SYSRANGE__START                         0x018
#define VL6180X_SYSRANGE__THRESH_HIGH                   0x019
#define VL6180X_SYSRANGE__THRESH_LOW                    0x01A
#define VL6180X_SYSRANGE__INTERMEASUREMENT_PERIOD       0x01B
#define VL6180X_SYSRANGE__MAX_CONVERGENCE_TIME          0x01C
#define VL6180X_SYSRANGE__CROSSTALK_COMPENSATION_RATE   0x01E
#define VL6180X_SYSRANGE__CROSSTALK_VALID_HEIGHT        0x021
#define VL6180X_SYSRANGE__EARLY_CONVERGENCE_ESTIMATE    0x022
#define VL6180X_SYSRANGE__PART_TO_PART_RANGE_OFFSET     0x024
#define VL6180X_SYSRANGE__RANGE_IGNORE_VALID_HEIGHT     0x025
#define VL6180X_SYSRANGE__RANGE_IGNORE_THRESHOLD        0x026
#define VL6180X_SYSRANGE__MAX_AMBIENT_LEVEL_MULT        0x02C
#define VL6180X_SYSRANGE__RANGE_CHECK_ENABLES           0x02D
#define VL6180X_SYSRANGE__VHV_RECALIBRATE               0x02E
#define VL6180X_SYSRANGE__VHV_REPEAT_RATE               0x031
#define VL6180X_SYSALS__START                           0x038
#define VL6180X_SYSALS__THRESH_HIGH                     0x03A
#define VL6180X_SYSALS__THRESH_LOW                      0x03C
#define VL6180X_SYSALS__INTERMEASUREMENT_PERIOD         0x03E
#define VL6180X_SYSALS__ANALOGUE_GAIN                   0x03F
#define VL6180X_SYSALS__INTEGRATION_PERIOD              0x040
#define VL6180X_RESULT__RANGE_STATUS                    0x04D
#define VL6180X_RESULT__ALS_STATUS                      0x04E
#define VL6180X_RESULT__INTERRUPT_STATUS_GPIO           0x04F
#define VL6180X_RESULT__ALS_VAL                         0x050
#define VL6180X_RESULT__HISTORY_BUFFER_START            0x052 // to 0x60
#define VL6180X_RESULT__RANGE_VAL                       0x062
#define VL6180X_RESULT__RANGE_RAW                       0x064
#define VL6180X_RESULT__RANGE_RETURN_RATE               0x066
#define VL6180X_RESULT__RANGE_REFERENCE_RATE            0x068
#define VL6180X_RESULT__RANGE_RETURN_SIGNAL_COUNT       0x06C
#define VL6180X_RESULT__RANGE_REFERENCE_SIGNAL_COUNT    0x070
#define VL6180X_RESULT__RANGE_RETURN_AMB_COUNT          0x074
#define VL6180X_RESULT__RANGE_REFERENCE_AMB_COUNT       0x078
#define VL6180X_RESULT__RANGE_RETURN_CONV_TIME          0x07C
#define VL6180X_RESULT__RANGE_REFERENCE_CONV_TIME       0x080
#define VL6180X_READOUT__AVERAGING_SAMPLE_PERIOD        0x10A
#define VL6180X_FIRMWARE__BOOTUP                        0x119
#define VL6180X_FIRMWARE__RESULT_SCALER                 0x120
#define VL6180X_I2C_SLAVE__DEVICE_ADDRESS               0x212
#define VL6180X_INTERLEAVED_MODE__ENABLE                0x2A3

#define VL6180X_ALS_GAIN_1                              0x06  // 1x gain
#define VL6180X_ALS_GAIN_1_25                           0x05  // 1.25x gain
#define VL6180X_ALS_GAIN_1_67                           0x04  // 1.67x gain
#define VL6180X_ALS_GAIN_2_5                            0x03  // 2.5x gain
#define VL6180X_ALS_GAIN_5                              0x02  // 5x gain
#define VL6180X_ALS_GAIN_10                             0x01  // 10x gain
#define VL6180X_ALS_GAIN_20                             0x00  // 20x gain
#define VL6180X_ALS_GAIN_40                             0x07  // 40x gain
    
#define VL6180X_ERROR_NONE                              0   // Success!
#define VL6180X_ERROR_SYSERR_1                          1   // System error
#define VL6180X_ERROR_SYSERR_5                          5   // Sysem error
#define VL6180X_ERROR_ECEFAIL                           6   // Early convergence estimate fail
#define VL6180X_ERROR_NOCONVERGE                        7   // No target detected
#define VL6180X_ERROR_RANGEIGNORE                       8   // Ignore threshold check failed
#define VL6180X_ERROR_SNR                               11  // Ambient conditions too high
#define VL6180X_ERROR_RAWUFLOW                          12  // Raw range algo underflow
#define VL6180X_ERROR_RAWOFLOW                          13  // Raw range algo overflow
#define VL6180X_ERROR_RANGEUFLOW                        14  // Raw range algo underflow
#define VL6180X_ERROR_RANGEOFLOW                        15  // Raw range algo overflow
    
#define VL6180X_CLEAR_RANGE_INT                         _BV(0)
#define VL6180X_CLEAR_ALS_INT                           _BV(1)
#define VL6180X_CLEAR_ERROR_INT                         _BV(2)
#define VL6180X_CLEAR_ALL_INTS                          (VL6180X_CLEAR_RANGE_INT | VL6180X_CLEAR_ALS_INT | VL6180X_CLEAR_ERROR_INT)
#define VL6180X_SYSRANGE_STARTSTOP                      _BV(0)
#define VL6180X_SYSALS_STARTSTOP                        _BV(0)
#define VL6180X_NEW_SAMPLE_READY_THRESHOLD_EVENT        _BV(2)
#define VL6180X_RESULT__RANGE_DEVICE_READY              _BV(0)
#define VL6180X_RESULT__RANGE_MEASUREMENT_READY         _BV(1)

void VL6180X_Setup(void);

#endif