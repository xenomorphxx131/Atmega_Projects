/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Timers                                                    *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_timers.h"

extern bool update_timers;
extern bool water_auto;
bool update_laser_value;
uint16_t water_on_debounce_timer = 0;
bool water_debounce_timer_en = false;
bool range_leakage_timeout = false;

/****************************************************************************
*     Process Soft Timers                                                   *
*****************************************************************************/
void process_soft_timers()
{

	static uint16_t laser_brightness_timer = 0;

    if (update_timers) //  Should happen every 1.024ms
    {
        update_timers = false;
        range_leakage_timeout = true;
        laser_brightness_timer++;
		
        if (water_debounce_timer_en)
            water_on_debounce_timer++;
		
		/*******************************************
		 *     Laser Brightness Timer              *
		 *******************************************/
        if (laser_brightness_timer >= ONE_SECOND * 5)
        {
            start_ALS_measurement();
            update_laser_value = true;
            laser_brightness_timer = 0;
        }

		/*******************************************
		 *     Water Debounce Timer                *
		 *******************************************/
        if (water_on_debounce_timer >= ONE_SECOND * 2)
        {
            if (water_auto) water_on(false);
            water_debounce_timer_en = false;
            water_on_debounce_timer = 0;
        }
    }
}