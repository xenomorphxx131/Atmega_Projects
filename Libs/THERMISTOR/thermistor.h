/*
 * Thermistor.c
 *
 * Created: 7/1/2012 10:06:56 AM
 *  Author: Steve
 */ 

#ifndef THERMISTOR_H
#define THERMISTOR_H
#include <stdint.h>
#include <avr/pgmspace.h>

/****************************************************************************
 *			Function Declarations	 										*
 ****************************************************************************/
double		Convert_NTC_To_DEGC (uint16_t NTC_ADC_Val );
#endif // THERMISTOR_H