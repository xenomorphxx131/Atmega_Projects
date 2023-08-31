#ifndef AWESOMEFAUCET_H_
#define AWESOMEFAUCET_H_

    #include <avr/io.h>
    #include <avr/wdt.h>
    #include <avr/power.h>
    #include <avr/interrupt.h>
    #include <avr/pgmspace.h>
    #include <avr/eeprom.h>
    #include <string.h>
    #include <stdio.h>
    #include <ctype.h>
    #include <util/delay.h>
    #include <LUFA/Version.h>
    #include <LUFA/Drivers/USB/USB.h>
    #include <process_USB.h>
    #include <i2c_twi.h>
    #include <VL6180X_SM.h>
    #include "Awesomefaucet_board.h"
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
