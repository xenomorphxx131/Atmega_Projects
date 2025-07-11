/****************************************************************************
 *                                                                          *
 *  Awesomerfaucet Timers                                                   *
 *                                                                          *
 ****************************************************************************/
#include "Awesomerfaucet_timers.h"

extern bool water_auto;
unsigned long debug_time_ms = 0; // more like 1.024ms - up to 50 days
uint16_t water_on_debounce_timer = 0;
uint16_t water_debounce_timeout = 0;
bool water_debounce_timer_en = false;

/****************************************************************************
*     Process Soft Timers                                                   *
*****************************************************************************/
void process_soft_timers()
{    
    debug_time_ms++;
    if (water_debounce_timer_en)
        water_on_debounce_timer++;
    /*******************************************
     *     Water Debounce Timer                *
     *******************************************/
    if (water_on_debounce_timer >= water_debounce_timeout) // units of 1.024ms
    {
        if (water_auto) water_on(false);
        water_debounce_timer_en = false;
        water_on_debounce_timer = 0;
    }
}