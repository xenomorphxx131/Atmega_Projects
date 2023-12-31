/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Water                                                     *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_water.h"

extern uint16_t water_on_debounce_timer;
extern bool water_debounce_timer_en;
extern bool foot_present;
extern bool water_auto;

 /****************************************************************************
 *    Water Control                                                          *
 *****************************************************************************/
void update_water()
{
	static bool water_debounce_timer_armed = false;
	
    if (foot_present)
    {
        if (water_auto) water_on(true);
        water_debounce_timer_armed = true;
        water_debounce_timer_en = false;
        water_on_debounce_timer = 0;
    }
    else if (water_debounce_timer_armed)
    {
        water_debounce_timer_armed = false;
        water_debounce_timer_en = true;
    }
}