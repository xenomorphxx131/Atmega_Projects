/*
     Copyright (C) Steve Martin, 2013.
*/

/*! \file USB_Load_Box_Board.h \brief Define each of the pins for the USB load box project. */
//*****************************************************************************
//
// File Name        : 'USB_Load_Box_Board.h'
// Author            : Steve Martin - Copyright (C) 2013
// Created            : 11/22/2011
// Revised            : 1/27/2013
// Version            : 1
// Target MCU        : Atmel AVR series
// Editor Tabs        : 4
//
//*****************************************************************************

#ifndef USB_LOAD_BOX_BOARD_H
#define USB_LOAD_BOX_BOARD_H

/****************************************************************************
*                    I²C Addresses on the Board                             *
*****************************************************************************/
#define DISPLAYADDRS                    0x7C     // The LCD Display
/****************************************************************************
*                    SPI Select Lines on the Board                          *
*****************************************************************************/
#define DACPORT                         PORTB
#define LDAC                           _BV(PB0)
#define MOSI                           _BV(PB2)
#define SCLK                           _BV(PB1)
#define DACDDR                          DDRB
#define DACPIN                          PINB
/****************************************************************************
*               I²C Pins SDA and SCL                                        *
*****************************************************************************/
#define SDAPORT                         PORTD
#define SDA                            _BV(PD1)
#define SDADDR                          DDRD
#define SDAPIN                          PIND

#define SCLPORT                         PORTD
#define SCL                            _BV(PD0)
#define SCLDDR                          DDRD
#define SCLPIN                          PIND
/****************************************************************************
*                    LOW CURRENT ASSIST                                     *
*****************************************************************************/
#define LOW_CUR_ASSIST_PORT              PORTB
#define LOW_CUR_ASSIST                  _BV(PB4)
#define LOW_CUR_ASSIST_DDR               DDRB
#define LOW_CUR_ASSIST_PIN               PINB
/****************************************************************************
*                    TRUE SHUT DOWN                                         *
*****************************************************************************/
#define ZERO_CURRENT_PORT               PORTD
#define ZERO_CURRENT                   _BV(PD6)
#define ZERO_CURRENT_DDR                DDRD
#define ZERO_CURRENT_PIN                PIND
/****************************************************************************
*                    LCD Reset Pin                                          *
*****************************************************************************/
#define LCD_RST_PORT                    PORTC
#define LCD_RST                        _BV(PC7)
#define LCD_RST_DDR                     DDRC
#define LCD_RST_PIN                     PINC
/****************************************************************************
*                    LCD Backlight LED Control Pin                          *
*****************************************************************************/
#define LCD_LED_PORT                    PORTB
#define LCD_LED                        _BV(PB5)
#define LCD_LED_DDR                     DDRB
#define LCD_LED_PIN                     PINB
/****************************************************************************
*                    2.5A Range Transistor Enable                           *
*****************************************************************************/
#define I2P5A_PORT                      PORTD
#define I2P5A                          _BV(PD7)
#define I2P5A_DDR                       DDRD
#define I2P5A_PIN                       PIND
/****************************************************************************
*                    40mm Fan                                               *
*****************************************************************************/
#define FAN_PORT                        PORTC
#define FAN                            _BV(PC6)
#define FAN_DDR                         DDRC
#define FAN_PIN                         PINC
/****************************************************************************
*                    ADC Presets                                            *
*****************************************************************************/
// To build up ADMUX: REFERENCE | ADLAR | MUX[4:0]
#define EXTERNAL_REF                    (0 << REFS1) | (0 << REFS0)
#define AVCC_REF                        (0 << REFS1) | (1 << REFS0)
#define DONTUSE                         (1 << REFS1) | (0 << REFS0)
#define INTERNAL_REF                    (1 << REFS1) | (1 << REFS0)

#define NTC_ADC_INPUTA  (0 << MUX4) | (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (1 << MUX0)
#define NTC_ADC_INPUTB  (1 << MUX5)

#define DIETEMP_ADC_INPUTA  (0 << MUX4) | (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0)
#define DIETEMP_ADC_INPUTB  (1 << MUX5)

#define ADC_DIDR                        DIDR2
#define ADC_DIDR_BIT                   _BV(ADC13D)
#define NTC_ADC_DDR                     DDRB
#define NTC_ADC_PORT                    PORTB
#define NTC_ADC_PIN                     PINB
#define NTC_ADC                        _BV(PB6)
/****************************************************************************
*                    Dropout Detector                                       *
*****************************************************************************/
#define DROPOUT_DTECTR_PORT             PORTD
#define DROPOUT_DTECTR_DDR              DDRD     
#define DROPOUT_DTECTR_PIN             _BV(PD5)
#define DROPOUT_DTECTR                  PIND
/****************************************************************************
*                    Keypad Pins                                            *
*****************************************************************************/
#define KEYPAD_SENSOR_PORT              PORTD
#define KEYPAD_SENSOR_DDR               DDRD
#define KEYPAD_SENSOR_PIN              _BV(PD4)
#define KEYPAD_SENSOR                   PIND
//---------------------------------------------------------------------------
#define KEYPAD_ADDR_PORT                PORTF
#define KEYPAD_ADDR_DDR                 DDRF
#define KEYPAD_ADDR0                   _BV(PF4)
#define KEYPAD_ADDR1                   _BV(PF5)
#define KEYPAD_ADDR2                   _BV(PF6)
#define KEYPAD_ADDR3                   _BV(PF7)
#define KEYPAD_ADDR                     PINF
#define KEYPAD_DISABLE                  0

#define BACKSPACE                       (0  << 4) // These are just enums but shifting
#define NINE                            (1  << 4) // also allows their use as a direct
#define EIGHT                           (2  << 4) // input to the keyboard scanning mux
#define SEVEN                           (3  << 4) // hardware.
#define mA                              (4  << 4)
#define HI_LO                           (5  << 4) // Specifically, they now line up with the
#define SIX                             (6  << 4) // port pins.
#define FIVE                            (7  << 4)
#define FOUR                            (8  << 4)
#define THREE                           (9  << 4)
#define TWO                             (10 << 4)
#define ONE                             (11 << 4)
#define AMPS                            (12 << 4)
#define DECIMAL                         (13 << 4)
#define ZERO                            (14 << 4)
#define uA                              (15 << 4)

#endif  // USB_LOAD_BOX_BOARD_H























