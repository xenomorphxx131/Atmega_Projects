/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Timers                                                    *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_timers.h"

extern bool update_timers;
bool update_laser_value;
uint16_t water_on_debounce_timer = 0;
uint16_t laser_brightness_timer = 0;
extern double largest_reading;
bool water_debounce_timer_en = false;

/****************************************************************************
*     Process Soft Timers                                                   *
*****************************************************************************/
void process_soft_timers()
{    
    if (update_timers) //  Should happen every 1.024ms
    {
        update_timers = false;
        
        laser_brightness_timer++;
        if (laser_brightness_timer == ONE_SECOND * 5) // change to ONE_MINUTE after debug??
        {
            start_ALS_measurement();
            update_laser_value = true;
            laser_brightness_timer = 0;
        }
        
        if (water_debounce_timer_en)
            water_on_debounce_timer++;
        
        if (water_on_debounce_timer == ONE_SECOND * 2)
        {
            water_on(false);
            water_debounce_timer_en = false;
            water_on_debounce_timer = 0;
        }
        
        if (largest_reading >= LEAKAGE_RATE)
            largest_reading -= LEAKAGE_RATE; // Leaky integrator leakage rate
    }
}