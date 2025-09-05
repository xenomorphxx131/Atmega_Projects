/**************************************************************************
*                                                                         *
* HTX9000SE LCD Manager                                                   *
*                                                                         *
***************************************************************************/

#include "HTX9000SE_lcd_manager.h"
bool entry_mode_screen_active = 0;
/****************************************************************************
 *    Setup Icons                                                           *
 ****************************************************************************/
void setup_icons()
{
    uint8_t    cgram_chars[8][8];        // CGRAM characters [CHAR][5 BIT LINE IN CHAR]

    cgram_chars[LITTLE_H][0] = 0b01010;  //
    cgram_chars[LITTLE_H][1] = 0b01110;  // This will be the little H
    cgram_chars[LITTLE_H][2] = 0b01010;  // for HIGH range
    cgram_chars[LITTLE_H][3] = 0b00000;  // At address (character) 0
    cgram_chars[LITTLE_H][4] = 0b00000;  //
    cgram_chars[LITTLE_H][5] = 0b00000;  //
    cgram_chars[LITTLE_H][6] = 0b00000;  //
    cgram_chars[LITTLE_H][7] = 0b00000;  //
        
    cgram_chars[LITTLE_L][0] = 0b00000;  //
    cgram_chars[LITTLE_L][1] = 0b00000;  // This will be the little L
    cgram_chars[LITTLE_L][2] = 0b00000;  // for LOW range
    cgram_chars[LITTLE_L][3] = 0b00000;  // At address (character) 1
    cgram_chars[LITTLE_L][4] = 0b01000;  //
    cgram_chars[LITTLE_L][5] = 0b01000;  //
    cgram_chars[LITTLE_L][6] = 0b01000;  //
    cgram_chars[LITTLE_L][7] = 0b01110;  //

    cgram_chars[LOCK_ICON][0] = 0b01110; //
    cgram_chars[LOCK_ICON][1] = 0b10001; // This will be the lock
    cgram_chars[LOCK_ICON][2] = 0b10001; // icon for when the touch
    cgram_chars[LOCK_ICON][3] = 0b11111; // pad is locked
    cgram_chars[LOCK_ICON][4] = 0b11111; // At address (character) 2
    cgram_chars[LOCK_ICON][5] = 0b11011; //
    cgram_chars[LOCK_ICON][6] = 0b11111; //
    cgram_chars[LOCK_ICON][7] = 0b00000; //
    
    CGRAM_Setup(cgram_chars);            // Create custom characters for H and L range and the Lock Icon
}
/****************************************************************************
 *    Flashy Intro Screen                                                   *
 ****************************************************************************/
void Run_Intro_Screen (IO_pointers_t IO)
{
    uint16_t duty_cycle = 0, increment = 0;
    uint8_t time = 0;
    LCD_Clear();
    LCD_Home();
    for (uint8_t i = 0; i <= 15 ; i++)
        LCD_Display_Shift_Left();                                   // Print on page 1
    scpi_prStr_P(PSTR("Current      0nA"), IO.LCD_stream);
    LCD_Set_Position(2, 0);
    scpi_prStr_P(PSTR("Heat Sink 21.0 C"), IO.LCD_stream);
    LCD_Set_Position(2, 14);
    LCD_Display_Char(DEGREE_BUBBLE_LCD);
    LCD_Set_Position(1, 16);                                        // Print on page 2
    scpi_prStr_P(PSTR("HyperTronix Inc."), IO.LCD_stream);
    LCD_Set_Position(2, 16);
    scpi_prStr_P(PSTR("   HTX-9000SE   "), IO.LCD_stream);
    for (time = 0; time <= 129 ; time++)                            // Duty Cycles 1/255 - 1/1
    {   
        increment = time >> 5;                                     // Increase by 3%/Step
        if (increment == 0) increment = 1;
        duty_cycle += increment;                                    // Fade back light on expo
        if (duty_cycle > 255) duty_cycle = 255;
        LCD_Backlight(false);
        _delay_ms((255 - duty_cycle)*0.05);
        LCD_Backlight(true);
        _delay_ms(duty_cycle*0.05);
    }
    /**********************************************************************************
    * Splash Screen Dwell                                                             *
    ***********************************************************************************/
    for (uint16_t i = 1; i <= 20000 ; i++)
    {
        _delay_ms(0.5);
        process_USB();
    }
    /**********************************************************************************
    * Splash Screen Left Shift                                                        *
    ***********************************************************************************/
    for (uint8_t i = 0; i <= 15 ; i++)
    {
        LCD_Display_Shift_Right();                                  // Slide screen out from right to left
        _delay_ms(100);
    }
}
/****************************************************************************
*  Entry Mode Screen setup                                                  *
*****************************************************************************/
void setup_entry_mode_screen(IO_pointers_t IO)
{
    LCD_Clear();
    LCD_Set_Position(1, 0);
    scpi_prStr_P(PSTR("Desired Current:"), IO.LCD_stream);
    LCD_Set_Position(2, 0);
    LCD_Cursor(true);

    set_entry_mode_screen_active(true);
}
/****************************************************************************
 *    set_entry_mode_screen_active                                          *
 ****************************************************************************/
void set_entry_mode_screen_active(bool value)
{
    entry_mode_screen_active = value;
}
/****************************************************************************
 *    get_entry_mode_screen_active                                          *
 ****************************************************************************/
bool get_entry_mode_screen_active()
{
    return entry_mode_screen_active;
}
/****************************************************************************
*  Draw Status Screen                                                       *
*****************************************************************************/
void draw_status_screen(double current, double temperature, bool locked, bool range, IO_pointers_t IO)
{
    LCD_Cursor(false);
    LCD_Clear();
    LCD_Home();
    if (locked)
    {
        LCD_Display_Char(LOCK_ICON);
        scpi_prStr_P(PSTR("*LOCK*"), IO.LCD_stream);
    }
    else
        scpi_prStr_P(PSTR("Current"), IO.LCD_stream);
    LCD_Set_Position(1, 8);
    if (current >= 1)
    {   fprintf(IO.LCD_stream, "%6.5f", current);
        scpi_prStr_P(PSTR("A"), IO.LCD_stream);
    }
    else if (current >= 100e-3)
    {
        current = current * 1e3;
        fprintf(IO.LCD_stream, "%5.2f", current);
        scpi_prStr_P(PSTR("mA"), IO.LCD_stream);
    }
    else if (current >= 10e-3)
    {
        current = current * 1e3;
        fprintf(IO.LCD_stream, "%5.3f", current);
        scpi_prStr_P(PSTR("mA"), IO.LCD_stream);
    }
    else if (current >= 1e-3)
    {
        current = current * 1e3;
        fprintf(IO.LCD_stream, "%5.4f", current);
        scpi_prStr_P(PSTR("mA"), IO.LCD_stream);
    }
    else if (current >= 100e-6)
    {
        current = current * 1e6;
        fprintf(IO.LCD_stream, "%5.2f", current);
        scpi_prStr_P(PSTR("uA"), IO.LCD_stream);
    }
    else if (current >= 10e-6)
    {
        current = current * 1e6;
        fprintf(IO.LCD_stream, "%5.3f", current);
        scpi_prStr_P(PSTR("uA"), IO.LCD_stream);
    }
    else if (current >= 1e-6)
    {
        current = current * 1e6;
        fprintf(IO.LCD_stream, "%5.4f", current);
        scpi_prStr_P(PSTR("uA"), IO.LCD_stream);
    }
    else // if (current >= 0)
    {
        current = current * 1e9;
        fprintf(IO.LCD_stream, "%6.0f", current);
        scpi_prStr_P(PSTR("nA"), IO.LCD_stream);
    }
    // The H/L range icons
    LCD_Set_Position(1, 7);
    LCD_Display_Char(range ? LITTLE_H : LITTLE_L);
    // The temperature information
    LCD_Set_Position(2, 0);
    scpi_prStr_P(PSTR("Heat Sink"), IO.LCD_stream);
    LCD_Set_Position(2, 10);
    fprintf(IO.LCD_stream, "%3.1f", temperature);
    LCD_Display_Char(DEGREE_BUBBLE_LCD);
    scpi_prStr_P(PSTR("C"), IO.LCD_stream);
}