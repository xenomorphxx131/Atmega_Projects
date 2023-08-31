#ifndef AWESOMEFAUCET_H_
#define AWESOMEFAUCET_H_

    #include "Awesomefaucet_scpi.h"
    
    #define round(x) ((x)>=0 ? (long)((x)+0.5):(long)((x)-0.5))
    #define ONE_SECOND round(1 / 0.001024) // Should be 977-ish
    #define ONE_MINUTE ONE_SECOND * 60
    #define ONE_mm 1
    #define DETECTION_HEIGHT 2.5
    #define STEADY_FOOT_COUNT 2
    #define LEAKAGE_RATE 0.00024
    #define MAX_LASER_POWER 4
    
    void process_soft_timers(void);
    void process_range_reading(void);
    void update_laser(IO_pointers_t);
    void update_water(void);
#endif
