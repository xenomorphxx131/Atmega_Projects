/****************************************************************************
 *                                                                          *
 *  Awesomerfaucet Sensor                                                   *
 *                                                                          *
 ****************************************************************************/

#ifndef AWESOMERFAUCET_SENSOR_H_
#define AWESOMERFAUCET_SENSOR_H_

	#include <VL6180X_SM.h>     // Needed for sensor readings
	#include <stdint.h>         // Needed for integers
	#include <stdbool.h>        // Needed for booleans

    void process_sensor(void);
    
    #define BLACKBOX_BUFFER_SIZE 128     // must be a power of two for index wrap efficiency

    typedef struct {
        float distance_mm;
        float max_distance_mm;
    } Blackbox;

#endif