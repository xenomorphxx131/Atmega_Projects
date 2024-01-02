/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Laser                                                     *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_laser.h"
 
extern bool update_laser_value;
extern bool laser_auto;
extern uint8_t laser_power;

/****************************************************************************
 *    Laser Control                                                         *
 ****************************************************************************/
void update_laser()
{
    // ALS goes from about 1 or 2 in low light to 50 in moderate indoor light to 150 in relatively bright light.
	// On a bright piece of white paper is goes 1300
	// The first order tweak to this is floor darkness.
	// Set darkness to 10 for a dark floor and 1 for a light floor.
	uint16_t setting = 0;
    if (update_laser_value)
	{
		if (laser_auto)
		{
			start_ALS_measurement();
			setting = get_als_blocking() / 8;// * get_darkness_setting()
			if (setting > 255) setting = 255;
			if (setting == 0) setting = 1;
			set_laser_power((uint8_t)setting);
		}
		else
			set_laser_power(laser_power);
		update_laser_value = false;
	}
}
/**************************************************************************
*  Laser Power                                                            *
***************************************************************************/
void set_laser_power(uint8_t power)
{
    OCR4D = power;
}