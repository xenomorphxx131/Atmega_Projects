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
#include "Awesomefaucet_scpi.h"     // Needed for VL6180X_SM functions

#define MAX_LASER_POWER     4

void update_laser(void);

#endif