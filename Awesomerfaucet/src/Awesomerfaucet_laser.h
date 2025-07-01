/****************************************************************************
 *                                                                          *
 *  Awesomerfaucet Laser                                                    *
 *                                                                          *
 ****************************************************************************/
 
#ifndef AWESOMERFAUCET_LASER_H_
#define AWESOMERFAUCET_LASER_H_

#include <stdint.h>                 // Needed for integers
#include <avr/io.h>					// Needed for timer comparator values
void set_laserpower(uint8_t);
uint8_t get_laserpower(void);

#endif