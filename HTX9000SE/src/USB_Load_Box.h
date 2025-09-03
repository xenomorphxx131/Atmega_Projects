#ifndef USBLOADBOX_H_
#define USBLOADBOX_H_

 /* Includes: */
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
	#include "USB_Load_Box_Board.h"
	#include <Loadbox_scpi.h>
	#include <touchpad.h>
	#include <lcd_manager.h>
	#define EVER ;;
    #define INT_CYCLES_PER_LCD_NTC_UPDATE 9766    // at 256uS interrupt interval this is every 2.5 seconds

	void	SetupHardware(void);
	int		LCD_Put_Char (char the_char, FILE *stream);
	void	Run_Intro_Screen (IO_pointers_t IO);
    void    process_soft_timers(IO_pointers_t);
    void    clear_keypad_timer(void);
    long    get_keypad_timer(void);
#endif
