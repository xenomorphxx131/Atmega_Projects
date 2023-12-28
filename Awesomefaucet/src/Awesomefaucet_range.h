/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Range                                                     *
 *                                                                          *
 ****************************************************************************/

#ifndef AWESOMEFAUCET_RANGE_H_
#define AWESOMEFAUCET_RANGE_H_

	#include <stdbool.h>                // Needed for booleans
	#include <stdint.h>                 // Needed for integers
	#include "Awesomefaucet_sensor.h"   // Needed for sensor readings

	#define ONE_mm 1
	#define DETECTION_HEIGHT 2.5
	#define STEADY_FOOT_COUNT 2

	void process_range_reading(void);

#endif