/****************************************************************************
 *                                                                          *
 *  Awesomerfaucet Water                                                    *
 *                                                                          *
 ****************************************************************************/
 
#ifndef AWESOMERFAUCET_WATER_H_
#define AWESOMERFAUCET_WATER_H_

#include <stdbool.h>                // Needed for booleans
#include <stdint.h>                 // Needed for integers
#include "Awesomefaucet_board.h"    // Needed for water IO setting
#include "Awesomefaucet_sensor.h"   // Needed for foor_present

void update_water(void);
void water_on(bool);

#endif