/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Laser                                                     *
 *                                                                          *
 ****************************************************************************/
 
#ifndef AWESOMEFAUCET_LASER_H_
#define AWESOMEFAUCET_LASER_H_

#include <stdbool.h>                // Needed for booleans
#include <stdint.h>                 // Needed for integers
#include <math.h>                   // Needed for round
#include <avr/interrupt.h>          // Needed for cli() and sei()
#include "Awesomefaucet_scpi.h"     // Needed for the darkness and laser settings
#include "Awesomefaucet_sensor.h"	// Needed for the ALS reading

void update_laser(void);
void set_laser_power(uint8_t);

#endif