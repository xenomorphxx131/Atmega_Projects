/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Board                                                     *
 *                                                                          *
 ****************************************************************************/
#ifndef AWESOMEFAUCET_BOARD_H
#define AWESOMEFAUCET_BOARD_H

#include <avr/io.h>
/****************************************************************************
*               I²C Pins SDA and SCL                                        *
*****************************************************************************/
#define SDAPORT                         PORTD
#define SDA                            _BV(PD1)
#define SDADDR                          DDRD
#define SDAPIN                          PIND

#define SCLPORT                         PORTD
#define SCL                            _BV(PD0)
#define SCLDDR                          DDRD
#define SCLPIN                          PIND
/****************************************************************************
*               Water Pin                                                   *
*****************************************************************************/
#define WATERPORT                       PORTD
#define WATER                           _BV(PD5)
#define WATERDDR                        DDRD
#define WATERPIN                        PIND
/****************************************************************************
*               Laser Pin                                                   *
*****************************************************************************/
#define LASERPORT                       PORTD
#define LASER                           _BV(PD7)
#define LASERDDR                        DDRD
#define LASERPIN                        PIND

#endif