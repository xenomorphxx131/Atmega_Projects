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

#endif