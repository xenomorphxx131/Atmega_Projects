/*
 * Thermistor.c
 *
 * Created: 7/1/2012 10:06:56 AM
 *  Author: Steve
 */ 

#include "thermistor.h"

/****************************************************************************
*			Thermistor Look Up Table										*
*			Thermistor is an EPCOS B57891M103K								*
*			The corresponding 34 temperatures for these ADC readings		*
*			start at -40°C and increment in 5°C steps to 125°C.				*
*****************************************************************************/
uint16_t const NTC_Table_Value[] PROGMEM =	{
	0xF825,		// (0)  -40°C
	0xF555,		// (1)  -35°C
	0xF1B4,		// (2)  -30°C
	0xED1C,		// (3)  -25°C
	0xE76A,		// (4)  -20°C
	0xE075,		// (5)  -15°C
	0xD830,		// (6)  -10°C
	0xCE4A,		// (7)   -5°C
	0xC310,		// (8)    0°C
	0xB6E0,		// (9)    5°C
	0xA9C6,		// (10)  10°C
	0x9C11,		// (11)  15°C
	0x8E16,		// (12)  20°C
	0x7FFF,		// (13)  25°C
	0x7267,		// (14)  30°C
	0x657E,		// (15)  35°C
	0x5976,		// (16)  40°C
	0x4E82,		// (17)  45°C
	0x449A,		// (18)  50°C
	0x3BAB,		// (19)  55°C
	0x33C9,		// (20)  60°C
	0x2CEC,		// (21)  65°C
	0x26F1,		// (22)  70°C
	0x21BE,		// (23)  75°C
	0x1D40,		// (24)  80°C
	0x195F,		// (25)  85°C
	0x1608,		// (26)  90°C
	0x1328,		// (27)  95°C
	0x10AF,		// (28) 100°C
	0x0E94,		// (29) 105°C
	0x0CC4,		// (30) 110°C
	0x0B32,		// (31) 115°C
	0x09D6,		// (32) 120°C
	0x08AB		// (33) 125°C
};
/****************************************************************************
*					Convert_NTC_To_DEGC										*
*****************************************************************************/				
double Convert_NTC_To_DEGC (uint16_t NTC_ADC_Val)
{
	double eff_index = 0;
	double result = 0;
	uint8_t i = 1;																		// Start from index 1 so interpolation doesn't mess up

	while (NTC_ADC_Val < pgm_read_word(&NTC_Table_Value[i]) && i <= 33)	i++;			// Stops on the index (i) above (warmer than) the ADC value

	eff_index = (double)i +
				((double)pgm_read_word(&NTC_Table_Value[i]) - (double)NTC_ADC_Val) /
				((double)pgm_read_word(&NTC_Table_Value[i-1]) - (double)pgm_read_word(&NTC_Table_Value[i]));
	result = (eff_index - 8) * 5;														// Temperature in °C.
	return result;
}








