/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Sensor                                                    *
 *                                                                          *
 ****************************************************************************/

#ifndef AWESOMEFAUCET_SENSOR_H_
#define AWESOMEFAUCET_SENSOR_H_
	   
	#include <stdint.h>                 // Needed for integers
	#include <stdbool.h>                // Needed for booleans
	#include <VL6180X_SM.h>				// Needed for sensor readings

    // uint8_t get_range(void);
    // void start_range_measurement(void);
    // bool range__measurement_ready(void);
    // bool range__range_device_ready(void);
    uint8_t read_range_blocking(void);
    uint16_t get_ALS_blocking(void);
    uint16_t read_ALS(void);

    void start_ALS_measurement(void);
    bool ALS_measurement_ready(void);
    bool ALS_sensor_busy(void);

#endif