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

    #define round(x) ((x)>=0 ? (long)((x)+0.5):(long)((x)-0.5))
    #define ONE_SECOND round(1 / 0.001024) // Should be 977-ish

	void process_soft_timers(void);

#endif