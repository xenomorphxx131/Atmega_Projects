/**************************************************************************
*                                                                         *
* HTX9000SE LCD Manager                                                   *
*                                                                         *
***************************************************************************/

#ifndef HTX9000SE_LCD_MANAGER_H
#define HTX9000SE_LCD_MANAGER_H

#include <LCD_7032i.h>
#include <scpi_parser.h> //  IO_pointers_t data type definition
/**************************************************************************
* GCRAM Defined Characters                                                *
***************************************************************************/
#define LITTLE_H  0
#define LITTLE_L  1
#define LOCK_ICON 2

void setup_icons(void);
void Run_Intro_Screen (IO_pointers_t);
void setup_entry_mode_screen(IO_pointers_t);
void draw_status_screen(double current, double temperature, bool locked, bool range, IO_pointers_t IO);
void set_entry_mode_screen_active(bool);
bool get_entry_mode_screen_active(void);

#endif