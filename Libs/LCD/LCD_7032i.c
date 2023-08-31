/*
 * LCD_7032i.h
 *
 * Created: 7/1/2012 10:06:56 AM
 *  Author: Steve
 */ 

#include "LCD_7032i.h"
/****************************************************************************
 *            Shift Entire Display Left - Moves HOME Off Screen             *
 ****************************************************************************/
void LCD_Display_Shift_Left()
{
    START();
    i2cPutbyte(LCD_ADDRESS);
    i2cPutbyte(COMPOUND_COMMAND);
    i2cPutbyte(FUNCTION_SET | EIGHT_BIT_MODE | DOUBLE_HEIGHT_OFF | NUMBER_OF_LINES_2 | INSTRUCTION_TABLE_0);
    i2cPutbyte(COMMAND);
    i2cPutbyte(SHIFT_DISPLAY_OR_CURSOR | SHIFT_SCREEN | LEFT);
    STOP();
}
/****************************************************************************
 *            Shift Entire Display Right - Moves HOME Off Screen            *
 ****************************************************************************/
void LCD_Display_Shift_Right()
{
    START();
    i2cPutbyte(LCD_ADDRESS);
    i2cPutbyte(COMPOUND_COMMAND);
    i2cPutbyte(FUNCTION_SET | EIGHT_BIT_MODE | DOUBLE_HEIGHT_OFF | NUMBER_OF_LINES_2 | INSTRUCTION_TABLE_0);
    i2cPutbyte(COMMAND);
    i2cPutbyte(SHIFT_DISPLAY_OR_CURSOR | SHIFT_SCREEN | RIGHT);
    STOP();
}
/****************************************************************************
 *            Clear Display                                                 *
 ****************************************************************************/
void LCD_Clear()
{
    START();
    i2cPutbyte(LCD_ADDRESS);
    i2cPutbyte(COMMAND);
    i2cPutbyte(CLEAR);
    STOP();
}
/****************************************************************************
 *            Move Cursor to HOME                                           *
 ****************************************************************************/
void LCD_Home()
{
    LCD_Set_Position(1, 0);
    // For some reason the native HOME command works like crap???
    // START();
    // i2cPutbyte(LCD_ADDRESS);
    // i2cPutbyte(COMMAND);
    // i2cPutbyte(HOME);
    // STOP();
}
/****************************************************************************
 *            Move Cursor Back One                                          *
 ****************************************************************************/
void LCD_Backspace()
{
    START();
    i2cPutbyte(LCD_ADDRESS);
    i2cPutbyte(COMMAND);
    i2cPutbyte(SHIFT_DISPLAY_OR_CURSOR | SHIFT_CURSOR | LEFT);
    STOP();
    
    LCD_Display_Char(' ');  // Overwrite what's there but this will index pointer
    
    START();
    i2cPutbyte(LCD_ADDRESS);
    i2cPutbyte(COMMAND);    // So go back again.
    i2cPutbyte(SHIFT_DISPLAY_OR_CURSOR | SHIFT_CURSOR | LEFT);
    STOP();
}
/****************************************************************************
 *            Cursor ON/OFF                                                 *
 ****************************************************************************/
void LCD_Cursor(uint8_t value)
{
    START();
    i2cPutbyte(LCD_ADDRESS);
    i2cPutbyte(COMMAND);
    i2cPutbyte(DISPLAY_MODE | DISPLAY_ENABLE | (value ? CURSOR_ON : CURSOR_OFF)  | (value ? CURSOR_BLINK_ON : CURSOR_BLINK_OFF));
    STOP();
}
/****************************************************************************
 *            Set Cursor to any Position                                    *
 ****************************************************************************/
void LCD_Set_Position(uint8_t line_in, uint8_t position)
{
    START();
    i2cPutbyte(LCD_ADDRESS);
    i2cPutbyte(COMMAND);
    i2cPutbyte(SET_DDRAM_ADDRESS | ((line_in == 1) ? LINE1 : LINE2) | position);
    STOP();
}
/****************************************************************************
 *            Output a Single Character                                     *
 ****************************************************************************/
void LCD_Display_Char(unsigned char the_char)
{
    START();
    i2cPutbyte(LCD_ADDRESS);
    i2cPutbyte(DATA);
    i2cPutbyte(the_char);
    STOP();
}
/****************************************************************************
 *          Set Up character Generator Ram                                  *
 ****************************************************************************/
void CGRAM_Setup(uint8_t cgram_chars[8][8])
{
    uint8_t    character = 0;
    uint8_t    line_in_char = 0;
    
    START();
    i2cPutbyte(LCD_ADDRESS);
    i2cPutbyte(COMPOUND_COMMAND);
    i2cPutbyte(FUNCTION_SET | EIGHT_BIT_MODE | DOUBLE_HEIGHT_OFF | NUMBER_OF_LINES_2 | INSTRUCTION_TABLE_0);    //go to instruction table 0
    i2cPutbyte(COMMAND);
    i2cPutbyte(SET_CGRAM_ADDRESS);                                                                              //Set CGRAM LCD_ADDRESS to 0x00
    STOP();
    
    START();
    i2cPutbyte(LCD_ADDRESS);
    i2cPutbyte(DATA);
    for ( character = 0; character <= 7; character++)               //8 characters
        for ( line_in_char = 0; line_in_char <= 7; line_in_char++)  //8 lines per character
            i2cPutbyte(cgram_chars[character][line_in_char]);
    STOP();
}
/****************************************************************************
 *            Initialization For ST7032i                                    *
 ****************************************************************************/
void LCD_Init()
{
    START();
    i2cPutbyte(LCD_ADDRESS);
    i2cPutbyte(COMPOUND_COMMAND);
    i2cPutbyte(FUNCTION_SET | EIGHT_BIT_MODE | DOUBLE_HEIGHT_OFF | NUMBER_OF_LINES_2 | INSTRUCTION_TABLE_1);
    i2cPutbyte(COMPOUND_COMMAND);
    i2cPutbyte(BIAS_AND_OSCFREQ | BIASHI | OSCFREQ_183HZ);
    i2cPutbyte(COMPOUND_COMMAND);
    i2cPutbyte(ICON_BOOST_AND_CONTRAST_HI | ICON_ON | BOOST_ON | CONTRAST_HI_VALUE_1);
    i2cPutbyte(COMPOUND_COMMAND);
    i2cPutbyte(CONTRAST_LO | CONTRAST_LOW_VALUE_7);
    i2cPutbyte(COMPOUND_COMMAND);
    i2cPutbyte(FOLLOWER_CONTROL | FOLLOWER_ON | FOLLOWER_RATIO_5);      // Just won't acknowledge this command!
    // Follower Control needs a 200ms delay!!!!!!
    for (long j = 0 ; j < 15e3 ; j++) // This comes out to 209ms
        asm volatile("nop");
    i2cPutbyte(COMPOUND_COMMAND);
    i2cPutbyte(DISPLAY_MODE | DISPLAY_ENABLE | CURSOR_OFF | CURSOR_BLINK_OFF);
    i2cPutbyte(COMPOUND_COMMAND);
    i2cPutbyte(CLEAR);
    i2cPutbyte(COMPOUND_COMMAND);
    i2cPutbyte(HOME);
    i2cPutbyte(COMMAND);
    i2cPutbyte(ENTRY_MODE | CURSOR_INCREMENT | DISPLAY_SHIFT_ENABLE);
    STOP();
}
/****************************************************************************
 *            Energize LCD Backlight                                        *
 ****************************************************************************/
void LCD_Backlight(uint8_t backlight_setting)
{
    if (backlight_setting == ON)
            LCD_LED_PORT |= LCD_LED;
    else    
            LCD_LED_PORT &= ~LCD_LED;
}
