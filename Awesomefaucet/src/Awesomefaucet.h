/****************************************************************************
 *                                                                          *
 *  Awesomefaucet                                                           *
 *                                                                          *
 ****************************************************************************/

#ifndef AWESOMEFAUCET_H_
#define AWESOMEFAUCET_H_

    #include "Awesomefaucet_scpi.h"
    #include "Awesomefaucet_laser.h"
    #include "Awesomefaucet_water.h"
    #include "Awesomefaucet_timers.h"

    #define ONE_mm 1
    #define DETECTION_HEIGHT 2.5
    #define STEADY_FOOT_COUNT 2

    void process_range_reading(void);
#endif
