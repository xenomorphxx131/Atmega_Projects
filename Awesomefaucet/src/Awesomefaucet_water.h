/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Water                                                     *
 *                                                                          *
 ****************************************************************************/
 
#ifndef AWESOMEFAUCET_WATER_H_
#define AWESOMEFAUCET_WATER_H_

#include <stdbool.h>                // Needed for booleans
#include <stdint.h>                 // Needed for integers
#include "Awesomefaucet_board.h"	// Needed for water IO setting

void update_water(void);
void water_on(bool);

#endif