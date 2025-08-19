#ifndef SCPI_CONFIG_H_
#define SCPI_CONFIG_H_

/****************************************************************************
*  VERSIONS and other Meta Data                                             *
*****************************************************************************/
    #define FIRMWARE_VERSION "3.2"
    #define COMPANY_NAME "Hypertronix"
    #define PROJECT_NAME "Awesome Faucet"
    #ifdef VL6180X
        #define SENSOR "VL6180X"
    #elif defined(VL53L4CD)
        #define SENSOR "VL53L4CD"
    #endif
#endif