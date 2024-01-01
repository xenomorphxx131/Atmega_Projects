/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Timers                                                    *
 *                                                                          *
 ****************************************************************************/

#ifndef AWESOMEFAUCET_TIMERS_H_
#define AWESOMEFAUCET_TIMERS_H_

	#include <stdbool.h>                // Needed for booleans
	#include <stdint.h>                 // Needed for integers
	#include "Awesomefaucet_sensor.h"   // Needed for sensor readings
	#include "Awesomefaucet_water.h"   	// Needed to control water
	// #include "Awesomefaucet_round.h"   	// Needed for preprocessor round()

    #define ONE_SECOND (uint16_t)(1 / 0.001024 + 0.5) // Should be about 977

	void process_soft_timers(void);

#endif