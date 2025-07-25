/****************************************************************************
 *                                                                          *
 *  Awesomefaucet                                                           *
 *                                                                          *
 ****************************************************************************/

#ifndef AWESOMERFAUCET_H_
#define AWESOMERFAUCET_H_

	#include <util/delay.h> 				// Needed for _delay_ms()
	#include <avr/interrupt.h>              // Needed for cli() and sei()
	#include <VL6180X_SM.h>                 // Needed for sensor readings
	#include <scpi_parser.h>                // scpi_process_cmd_P
    #include <vl53l4cd_api.h>
    #include "Awesomefaucet_scpi.h"
    #include "Awesomefaucet_laser.h"
    #include "Awesomefaucet_water.h"
    #include "Awesomefaucet_timers.h"
    #include "Awesomefaucet_sensor.h"

#endif