/****************************************************************************
 *                                                                          *
 *  Awesomerfaucet Laser                                                    *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_laser.h"

uint8_t laser_power;

/**************************************************************************
*  Set Laser Power                                                        *
***************************************************************************/
void set_laserpower()
{
    OCR4D = laser_power;
}