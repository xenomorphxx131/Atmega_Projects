#ifndef I2C_TWI_H
#define I2C_TWI_H

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <avr/io.h>
#include <util/delay.h>	// Uses delay_us()
#include <util/twi.h>
#define WRITE 0x00 		// I²C WRITE bit
#define READ  0x01 		// I²C READ bit
#define LAST  0x01 		// A flag for the last byte to NACK
#define MAX_ITER 200

//                     CPU Freq 
// SCL Freq  =  ------------------------
//                               (TWPS)
//               16 + 2(TWBR) * 4

// SCL Freq = 16,000,000 / (16 + 2(12)*4^0) = 400,000
// TWBR (8 bits)
#define INIT_TWBR 0x0C
#define INIT_TWBR_100KHz 0x48
// TWPS (2 bits)
#define INIT_TWPS 0

#define I2C_DELAY asm volatile("nop")

// Create a data structure to contain the port and pin information
typedef struct i2c_twi_node
{
  volatile uint8_t* sdaport;
  volatile uint8_t* sclport;
  volatile uint8_t* sdaddr;
  volatile uint8_t* sclddr;
  volatile uint8_t* sdapin;
  volatile uint8_t* sclpin;
  uint8_t sda;
  uint8_t scl;
  uint8_t twbr;
  uint8_t twps;
} i2c_twi_port_t;

// functions

char START2(void);
char STOP2(void);

void i2cTwiInit ( i2c_twi_port_t* );
void i2cSetBusSpeed ( i2c_twi_port_t* );
void disableTwi ( void );
void enableTwi ( void );
void reset_i2c(void);
char START ( void );
char STOP ( void );
char i2cPutbyte ( uint8_t );
uint8_t i2cGetbyte ( uint8_t );
uint8_t SMBUS_Receive_Byte ( uint8_t );
void SMBUS_Send_Byte ( uint8_t, uint8_t );
uint8_t SMBUS_Read_Byte ( uint8_t, uint8_t );
uint8_t I2C16_Read_Byte( uint8_t, uint16_t );
uint8_t I2C16_Write_Byte( uint8_t, uint16_t, uint8_t );
uint16_t I2C16_Read_Word( uint8_t, uint16_t );
uint16_t SMBUS_Read_Byte_PEC ( uint8_t, uint8_t );
uint8_t SMBUS_Write_Byte ( uint8_t, uint8_t, uint8_t );
uint8_t SMBUS_Write_Byte_PEC ( uint8_t, uint8_t, uint8_t, uint8_t );
uint16_t SMBUS_Read_Word ( uint8_t, uint8_t );
uint32_t SMBUS_Read_Word_PEC ( uint8_t, uint8_t );
uint8_t SMBUS_Write_Word ( uint8_t, uint8_t, uint8_t, uint8_t );
uint8_t SMBUS_Write_Word_PEC ( uint8_t, uint8_t, uint8_t, uint8_t, uint8_t );
uint8_t SMBUS_Alert_Responce ( void );
uint16_t SMBUS_Alert_Responce_PEC ( void );
int twcr ( void );
int twsr ( void );
void clear_bus_err ( void );

uint8_t I2C_16BITSUB_Read_Byte( uint8_t address, uint16_t subaddress, uint8_t *byte);
uint8_t I2C_16BITSUB_Read_Word( uint8_t address, uint16_t subaddress, uint16_t *word);
uint8_t I2C_16BITSUB_Read_DWord( uint8_t address, uint16_t subaddress, uint32_t *dword);
uint8_t I2C_16BITSUB_Write_Byte( uint8_t address, uint16_t subaddress, uint8_t byte );
uint8_t I2C_16BITSUB_Write_Word( uint8_t address, uint16_t subaddress, uint16_t word );
uint8_t I2C_16BITSUB_Write_DWord( uint8_t address, uint16_t subaddress, uint32_t dword );

#endif