#ifndef I2C_SP_H
#define I2C_SP_H

#include <avrlibdefs.h>
#include <avrlibtypes.h>
#include <avr/io.h>
#include "board.h"

#define WRITE 0x00 // I²C WRITE bit
#define READ  0x01 // I²C READ bit
#define LAST  0x01 // A flag for the last byte to NACK


// Create a data structure to contain the port and pin information
typedef struct i2c_soft_port_node
{
  uint8_t enabled;
  uint8_t internal_pullup;
  volatile uint8_t* sdaport;
  volatile uint8_t* sclport;
  volatile uint8_t* sdaddr;
  volatile uint8_t* sclddr;
  volatile uint8_t* sdapin;
  volatile uint8_t* sclpin;
  uint8_t sda;
  uint8_t scl;
  uint8_t delay;
} i2c_soft_port;


// functions
void i2c_sp_init ( i2c_soft_port* );
void i2c_sp_init_wpullup ( i2c_soft_port* );
void i2c_sp_disable ( i2c_soft_port* );
void i2c_sp_delay ( i2c_soft_port*, uint8_t );
void I2C_SP_HALF_DELAY ( uint8_t );
void I2C_SP_UNIT_DELAY ( uint8_t );
void I2C_SP_DELAY ( uint8_t );
void I2C_SP_SCL_LOW ( i2c_soft_port* );
void I2C_SP_SCL_HIGH ( i2c_soft_port* );
void I2C_SP_SDA_LOW ( i2c_soft_port* );
void I2C_SP_SDA_HIGH ( i2c_soft_port* );
uint8_t I2C_SP_SCL_TOGGLE ( i2c_soft_port* );
char I2C_SP_START ( i2c_soft_port* );
char I2C_SP_STOP ( i2c_soft_port* );
char I2C_SP_ACK ( i2c_soft_port* );
char I2C_SP_NACK ( i2c_soft_port* );
uint8_t I2C_SP_SEND_RECV_I2C_HEX ( i2c_soft_port*, uint8_t );
char i2c_sp_Putbyte ( i2c_soft_port*, uint8_t );
uint8_t i2c_sp_Getbyte ( i2c_soft_port*, uint8_t );
uint8_t SMBUS_sp_Receive_Byte ( i2c_soft_port*, uint8_t );
void SMBUS_sp_Send_Byte ( i2c_soft_port*, uint8_t, uint8_t );
uint8_t SMBUS_sp_Read_Byte ( i2c_soft_port*, uint8_t, uint8_t );
uint8_t SMBUS_sp_Write_Byte ( i2c_soft_port*, uint8_t, uint8_t, uint8_t );
uint16_t SMBUS_sp_Read_Word ( i2c_soft_port*, uint8_t, uint8_t );
uint8_t SMBUS_sp_Write_Word ( i2c_soft_port*, uint8_t, uint8_t, uint8_t, uint8_t );
uint16_t SMBUS_sp_Read_Byte_PEC ( i2c_soft_port*, uint8_t, uint8_t );
uint8_t SMBUS_sp_Write_Byte_PEC ( i2c_soft_port*, uint8_t, uint8_t, uint8_t , uint8_t );
uint32_t SMBUS_sp_Read_Word_PEC ( i2c_soft_port*, uint8_t, uint8_t );
uint8_t SMBUS_sp_Write_Word_PEC ( i2c_soft_port*, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t );
uint8_t SMBUS_sp_Alert_Responce ( i2c_soft_port* );
uint16_t SMBUS_sp_Alert_Responce_PEC ( i2c_soft_port* );
#endif
