/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Water                                                     *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_water.h"

extern bool hold_water_debounce_timer;
extern bool foot_present;
extern bool water_auto;
extern bool water_time_expired;

 /****************************************************************************
 *    Water Control                                                          *
 *****************************************************************************/
void update_water()
{
	if (water_auto)
	{
		if (water_time_expired) water_on(false);
		
		if (foot_present)
		{
			water_on(true);
			hold_water_debounce_timer = true;
		}
		else
			hold_water_debounce_timer = false;
	}
}