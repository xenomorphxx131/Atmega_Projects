/****************************************************************************
 *                                                                          *
 *  Awesomefaucet Laser                                                     *
 *                                                                          *
 ****************************************************************************/
#include "Awesomefaucet_laser.h"
 
extern bool update_laser_value;

/****************************************************************************
 *    Laser Control                                                         *
 ****************************************************************************/
void update_laser()
{
    // Brightness seems to go from about 1 or 2 in low light to 50 in moderate indoor light.
    uint16_t brightness = 0;
    uint8_t power = 0;
    if (update_laser_value)
    {
        brightness = get_als_blocking();
        power = (uint8_t)round(brightness / 50 * MAX_LASER_POWER);
        if (power < 1) power = 1;
        if (power > MAX_LASER_POWER) power = MAX_LASER_POWER;
        laser_power(power * get_darkness_setting());
        update_laser_value = false;
    }
}