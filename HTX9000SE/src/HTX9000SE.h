/**************************************************************************
*                                                                         *
* HTX9000SE                                                               *
*                                                                         *
***************************************************************************/

#ifndef HTX9000SE_H_
#define HTX9000SE_H_

    #include "HTX9000SE_touchpad.h"
    #define INT_CYCLES_PER_LCD_NTC_UPDATE 9766  // at 256uS interrupt interval this is every 2.5 seconds

    int LCD_Put_Char(char the_char, FILE *stream);
    void Run_Intro_Screen(IO_pointers_t IO);
    void process_soft_timers(IO_pointers_t);
#endif