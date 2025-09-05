#ifndef I2C_TWI_H
#define I2C_TWI_H

#include <util/delay.h> // Uses delay_us()
#include <util/twi.h>

#define WRITE 0x00 // I²C WRITE bit
#define READ  0x01 // I²C READ bit
#define LAST  0x01 // A flag for the last byte to NACK
#define MAX_ITER 100

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
}   i2c_twi_port_t;

void i2cTwiInit ( i2c_twi_port_t* );
void i2cSetBusSpeed ( i2c_twi_port_t* );
void disableTwi ( void );
void enableTwi ( void );
char START ( void );
char STOP ( void );
char i2cPutbyte ( uint8_t );
void clear_bus_err ( void );
#endif