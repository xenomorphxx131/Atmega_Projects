/****************************************************************************
 *                                                                          *
 *  Awesomerfaucet Timers                                                   *
 *                                                                          *
 ****************************************************************************/
#include "Awesomerfaucet_timers.h"

extern bool update_timers;
extern bool water_auto;
uint16_t water_on_debounce_timer = 0;
bool water_debounce_timer_en = false;
bool range_leakage_timeout = false;

#define ONE_SECOND (long)(1 / 0.001024 + 0.5) // Should be about 977

/****************************************************************************
*     Process Soft Timers                                                   *
*****************************************************************************/
void process_soft_timers()
{    
    if (update_timers) //  Should happen every 1.024ms
    {
        update_timers = false;
        range_leakage_timeout = true;
		
        if (water_debounce_timer_en)
            water_on_debounce_timer++;

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