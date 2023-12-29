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
    // Brightness seems to go from about 1 or 2 in low light to 50 in moderate indoor light.
    // Set darkness setting in the range to 63
    uint16_t brightness = 0;
    uint8_t power = 0;
    if (update_laser_value)
	{
		if (laser_auto)
		{
			brightness = get_als_blocking();
			power = (uint8_t)round(brightness / 50 * MAX_LASER_POWER);
			if (power < 1) power = 1;
			if (power > MAX_LASER_POWER) power = MAX_LASER_POWER;
			set_laser_power(power * get_darkness_setting());
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