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
	#include "Awesomefaucet_scpi.h"   	// Needed for IIR setting

	#define DETECTION_HEIGHT_mm 2.5
	#define STEADY_FOOT_COUNT 2

	void process_range_reading(void);

#endif