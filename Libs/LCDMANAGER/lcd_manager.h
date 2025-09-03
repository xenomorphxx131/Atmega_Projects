/*! \file LCD_manager.h \brief LCD Manager */
//*****************************************************************************
// File Name    : 'LCD_manager.h'
// Title        : LCD manager
// Author       : Steve Martin
// Version      : 1
// Target MCU   : Atmel AVR series
//*****************************************************************************
#ifndef LCD_MANAGER_H
#define LCD_MANAGER_H

#include <LCD_7032i.h>
#include <scpi.h> //  IO_pointers_t data type definition ???
    
/**************************************************************************
* GCRAM Defined Characters                                                *
***************************************************************************/
#define LITTLE_H  0
#define LITTLE_L  1
#define LOCK_ICON 2
    
void delay_100us(uint16_t);
void setup_icons(void);
void Run_Intro_Screen (IO_pointers_t);
void setup_entry_mode_screen(IO_pointers_t);
void draw_status_screen(double current, double temperature, bool locked, bool range, IO_pointers_t IO);
void set_entry_mode_screen_active(bool);
bool get_entry_mode_screen_active(void);
    
#endif // LCD_7032i_H