/*! \file LCD_7032i.h \brief Software I²C interface using port pins. */
//*****************************************************************************
// File Name    : 'LCD_7032i.h'
// Title        : Software I²C interface using port pins
// Author       : Steve Martin
// Created      : 7/1/2012
// Revised      : 7/1/2012
// Version      : 1
// Target MCU   : Atmel AVR series
//
//*****************************************************************************
#ifndef LCD_7032i_H
#define LCD_7032i_H
#include "USB_Load_Box_Board.h"
#include "i2c_twi.h"
/****************************************************************************
 *                    Commands                                              *
 ****************************************************************************/
#define COMMAND                     0x00 // The first byte needs to be either COMMAND or DATA
#define COMPOUND_COMMAND            0x80 // Command continuation
#define DATA                        0x40 // The first byte needs to be either COMMAND or DATA
#define CLEAR                       0x01
#define HOME                        0x02
#define ENTRY_MODE                  0x04
#define DISPLAY_MODE                0x08
#define BIAS_AND_OSCFREQ            0x10 // Instruction table 1
#define SHIFT_DISPLAY_OR_CURSOR     0x10 // Instruction table 0
#define FUNCTION_SET                0x20
#define SET_DDRAM_ADDRESS           0x80
#define SET_CGRAM_ADDRESS           0x40
#define ICON_BOOST_AND_CONTRAST_HI  0x50
#define CONTRAST_LO                 0x70
#define FOLLOWER_CONTROL            0x60
/****************************************************************************
 *                        Values                                            *
 ****************************************************************************/
#define LCD_ADDRESS                 0x7C // [0x7C]    I²C LCD_Address

#define BIASLO                      0x00 // Bias = 1/5
#define BIASHI                      0x04 // Bias = 1/4

#define OSCFREQ_122HZ               0x00
#define OSCFREQ_131HZ               0x01
#define OSCFREQ_144HZ               0x02
#define OSCFREQ_161HZ               0x03
#define OSCFREQ_183HZ               0x04
#define OSCFREQ_221HZ               0x05
#define OSCFREQ_274HZ               0x06
#define OSCFREQ_347HZ               0x07

#define CONTRAST_HI_VALUE_0         0x00
#define CONTRAST_HI_VALUE_1         0x01
#define CONTRAST_HI_VALUE_2         0x02
#define CONTRAST_HI_VALUE_3         0x03

#define CONTRAST_LOW_VALUE_0        0x00
#define CONTRAST_LOW_VALUE_1        0x01
#define CONTRAST_LOW_VALUE_2        0x02
#define CONTRAST_LOW_VALUE_3        0x03
#define CONTRAST_LOW_VALUE_4        0x04
#define CONTRAST_LOW_VALUE_5        0x05
#define CONTRAST_LOW_VALUE_6        0x06
#define CONTRAST_LOW_VALUE_7        0x07
#define CONTRAST_LOW_VALUE_8        0x08
#define CONTRAST_LOW_VALUE_9        0x09
#define CONTRAST_LOW_VALUE_A        0x0A
#define CONTRAST_LOW_VALUE_B        0x0B
#define CONTRAST_LOW_VALUE_C        0x0C
#define CONTRAST_LOW_VALUE_D        0x0D
#define CONTRAST_LOW_VALUE_E        0x0E
#define CONTRAST_LOW_VALUE_F        0x0F

#define ICON_OFF                    0x00
#define ICON_ON                     0x08

#define BOOST_OFF                   0x00
#define BOOST_ON                    0x04

#define FOUR_BIT_MODE               0x00
#define EIGHT_BIT_MODE              0x10

#define INSTRUCTION_TABLE_0         0x00
#define INSTRUCTION_TABLE_1         0x01

#define DOUBLE_HEIGHT_OFF           0x00
#define DOUBLE_HEIGHT_ON            0x04

#define NUMBER_OF_LINES_1           0x00
#define NUMBER_OF_LINES_2           0x08

#define FOLLOWER_OFF                0x00
#define FOLLOWER_ON                 0x08

#define FOLLOWER_RATIO_0            0x00
#define FOLLOWER_RATIO_1            0x01
#define FOLLOWER_RATIO_2            0x02
#define FOLLOWER_RATIO_3            0x03
#define FOLLOWER_RATIO_4            0x04
#define FOLLOWER_RATIO_5            0x05
#define FOLLOWER_RATIO_6            0x06
#define FOLLOWER_RATIO_7            0x07

#define DISPLAY_DISABLE             0x00
#define DISPLAY_ENABLE              0x04

#define CURSOR_OFF                  0x00
#define CURSOR_ON                   0x02

#define CURSOR_BLINK_OFF            0x00
#define CURSOR_BLINK_ON             0x01

#define CURSOR_DECREMENT            0x00 //     Cursor moves [LEFT ] DDRAM is [DECREMENTED]
#define CURSOR_INCREMENT            0x02 //     Cursor moves [RIGHT] DDRAM is [INCREMENTED]

#define DISPLAY_SHIFT_DISABLE       0x00
#define DISPLAY_SHIFT_ENABLE        0x01

#define SHIFT_SCREEN                0x08
#define SHIFT_CURSOR                0x00
#define RIGHT                       0x04
#define LEFT                        0x00

#define LINE1                       0x00
#define LINE2                       0x40

#define POSITION1                   0x00
#define POSITION2                   0x01
#define POSITION3                   0x02
#define POSITION4                   0x03
#define POSITION5                   0x04
#define POSITION6                   0x05
#define POSITION7                   0x06
#define POSITION8                   0x07
#define POSITION9                   0x08
#define POSITION10                  0x09
#define POSITION11                  0x0A
#define POSITION12                  0x0B
#define POSITION13                  0x0C
#define POSITION14                  0x0D
#define POSITION15                  0x0E
#define POSITION16                  0x0F

#define CGRAM_CHAR0                 0
#define CGRAM_CHAR1                 1
#define CGRAM_CHAR2                 2
#define CGRAM_CHAR3                 3
#define CGRAM_CHAR4                 4
#define CGRAM_CHAR5                 5
#define CGRAM_CHAR6                 6
#define CGRAM_CHAR7                 7

#define ON                          0x01
#define OFF                         0x00
/****************************************************************************
 *                    Special Characters                                    *
 ****************************************************************************/
#define    HORIZONTAL_LINE         0xB0
#define    RIGHT_ARROW             0x7E
#define    LEFT_ARROW              0x7F
#define    DEGREE_BUBBLE_LCD       0xDF
/****************************************************************************
 *            Function Declarations                                         *
 ****************************************************************************/
 void LCD_Display_Shift_Left(void);
 void LCD_Display_Shift_Right(void);
 void LCD_Clear(void);
 void LCD_Home(void);
 void LCD_Backspace(void);
 void LCD_Cursor(uint8_t);
 void LCD_Set_Position(uint8_t line_in, uint8_t position);
 void LCD_Display_Char(unsigned char the_char);
 void CGRAM_Setup(uint8_t cgram_chars[8][8]);
 void LCD_Init(void);
 void LCD_Backlight(uint8_t backlight_setting);

#endif // LCD_7032i_H