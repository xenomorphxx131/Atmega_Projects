/*
     Copyright (C) Steve Martin, 2019.
*/

/*! \file Awesomefaucet_board.h \brief Define each of the pins for the USB load box project. */
//*****************************************************************************
//
// File Name            : 'Awesomefaucet_board.h'
// Author               : Steve Martin - Copyright (C) 2019
// Created              : 4/6/2019
// Revised              : ??/??/??
// Version              : 1
// Target MCU           : Atmel AVR series
// Editor Tabs          : 4
//
//*****************************************************************************
#ifndef AWESOMEFAUCET_BOARD_H
#define AWESOMEFAUCET_BOARD_H
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

#endif  // AWESOMEFAUCET_BOARD_H























