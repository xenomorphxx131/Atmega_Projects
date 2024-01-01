/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Timers                                                    *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_timers.h"

extern bool update_timers;
bool update_laser_value;
bool hold_water_debounce_timer = false;
bool leakage_update_time = false;
bool water_time_expired = true;

/****************************************************************************
*     Process Soft Timers                                                   *
*****************************************************************************/
void process_soft_timers()
{
	static uint16_t laser_brightness_timer = 0;
	static uint16_t water_on_debounce_timer = 0;

    if (update_timers) //  Should happen every 1.024ms
    {
		update_timers = false;
		/*******************************************
		 *     Range Reading Leakage Timer         *
		 *******************************************/
        leakage_update_time = true;
		/*******************************************
		 *     Laser Brightness Timer              *
		 *******************************************/
		laser_brightness_timer++;
        if (laser_brightness_timer >= 5 * ONE_SECOND)
        {
            update_laser_value = true;
            laser_brightness_timer = 0;
        }
		/*******************************************
		 *     Water Debounce Timer                *
		 *******************************************/
		if (hold_water_debounce_timer)
		{
			water_on_debounce_timer = 0;
			water_time_expired = false;
		}
		else
			water_on_debounce_timer++;
		
		if (water_on_debounce_timer >= 2 * ONE_SECOND)
		{
			water_time_expired = true;
			hold_water_debounce_timer = true;
		}
    }
}