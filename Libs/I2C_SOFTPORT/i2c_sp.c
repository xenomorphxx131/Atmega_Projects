#include "i2c_sp.h"

// Initialize Port
// ---------------

void i2c_sp_init ( i2c_soft_port* i2c_p )
{
  *i2c_p->sdaddr  &= ~i2c_p->sda;  // Initialze the pin direction to input for HIGH by resistor pull up
  *i2c_p->sclddr  &= ~i2c_p->scl;  // Initialze the pin direction to input for HIGH by resistor pull up
  *i2c_p->sdaport &= ~i2c_p->sda;  // Permanently set the pin state to low. Use the direction bit
  *i2c_p->sclport &= ~i2c_p->scl;  // to set high Z. Requires a pull up resistor to make a "HIGH"
  i2c_p->enabled = 1;              // Enable port
  i2c_p->internal_pullup = 0;      // No internal pullup external pullup required
}

void i2c_sp_init_wpullup ( i2c_soft_port* i2c_p )
{
  *i2c_p->sdaddr  &= ~i2c_p->sda; // Initialze the pin direction to input for HIGH by resistor pull up
  *i2c_p->sclddr  &= ~i2c_p->scl; // Initialze the pin direction to input for HIGH by resistor pull up
  *i2c_p->sdaport |= i2c_p->sda;  //  Set to Pullup
  *i2c_p->sclport |= i2c_p->scl;  // Set to Pullup
  i2c_p->enabled = 1;             // Enable port
  i2c_p->internal_pullup = 1;     // Internal pullup no external pullup required
}

void i2c_sp_disable ( i2c_soft_port* i2c_p ) { i2c_p->enabled = 0; } // Disable port


void i2c_sp_delay ( i2c_soft_port* i2c_p, uint8_t d) { i2c_p->delay = d; } // Set delay variable


// Primitives
// ----------

// Half Delay
void I2C_SP_HALF_DELAY ( uint8_t d ) { for (uint16_t j = 0 ; j < (uint16_t)d+1 ; j++) asm volatile("nop"); }

// Unit Delay
void I2C_SP_UNIT_DELAY ( uint8_t d ) { I2C_SP_HALF_DELAY(d); I2C_SP_HALF_DELAY(d); }

// Delay for Debug using an analog oscilloscope
void I2C_SP_DELAY ( uint8_t d ) { I2C_SP_UNIT_DELAY(d); I2C_SP_UNIT_DELAY(d); I2C_SP_UNIT_DELAY(d); }

// SCL Low
void I2C_SP_SCL_LOW ( i2c_soft_port* i2c_p ) {
  if (i2c_p->enabled) {
    *i2c_p->sclddr  |= i2c_p->scl;
    *i2c_p->sclport &= ~i2c_p->scl;
  }
}

// SCL High
void I2C_SP_SCL_HIGH ( i2c_soft_port* i2c_p ) {
  if (i2c_p->enabled) {
    if (i2c_p->internal_pullup) {
      *i2c_p->sclddr  &= ~i2c_p->scl;
      *i2c_p->sclport |= i2c_p->scl;
    } else {
      *i2c_p->sclddr &= ~i2c_p->scl;
    }
  }
}

// SDA Low
void I2C_SP_SDA_LOW ( i2c_soft_port* i2c_p ) {
  if (i2c_p->enabled) {
    *i2c_p->sdaddr  |= i2c_p->sda;
    *i2c_p->sdaport &= ~i2c_p->sda;
  }
}

// SDA High
void I2C_SP_SDA_HIGH ( i2c_soft_port* i2c_p ) {
  if (i2c_p->enabled) {
    if (i2c_p->internal_pullup) {
      *i2c_p->sdaddr  &= ~i2c_p->sda;
      *i2c_p->sdaport |= i2c_p->sda;
    } else {
      *i2c_p->sdaddr &= ~i2c_p->sda;
    }
  }
}

// SCL Toggle
uint8_t I2C_SP_SCL_TOGGLE ( i2c_soft_port* i2c_p )
{
    uint8_t the_sda_pin = 0;
    uint8_t the_scl_pin = 0;

  if (i2c_p->enabled) { 
    I2C_SP_UNIT_DELAY(i2c_p->delay);
    I2C_SP_SCL_HIGH(i2c_p);
    the_sda_pin = *i2c_p->sdapin & i2c_p->sda;
    the_scl_pin = *i2c_p->sclpin & i2c_p->scl;
    I2C_SP_UNIT_DELAY(i2c_p->delay);
    I2C_SP_SCL_LOW(i2c_p);
    I2C_SP_UNIT_DELAY(i2c_p->delay);
  }
  return the_sda_pin;
}


/**************************************************************************
*                                                                         *
*      Complete I²C Tokens                                                *
*                                                                         *
***************************************************************************/

// Start
char I2C_SP_START ( i2c_soft_port* i2c_p )
{
  uint8_t the_sda_pin;
  uint8_t the_scl_pin = 0;
  char what_happened = 0;

  if (i2c_p->enabled) { 
    I2C_SP_HALF_DELAY(i2c_p->delay);
    // Need to get Bus HIGH in case of a RESTART
    I2C_SP_SDA_HIGH(i2c_p);
    I2C_SP_HALF_DELAY(i2c_p->delay);
    I2C_SP_SCL_HIGH(i2c_p);
    I2C_SP_HALF_DELAY(i2c_p->delay);
    the_sda_pin = *i2c_p->sdapin & i2c_p->sda;
    the_scl_pin = *i2c_p->sclpin & i2c_p->scl;
    if (the_sda_pin && the_scl_pin)
      what_happened = 'S';  // Both high, so far so good
    else
      what_happened = '?';  // Huh?, one of them is low
    I2C_SP_SDA_LOW(i2c_p);
    the_sda_pin = *i2c_p->sdapin & i2c_p->sda;
    the_scl_pin = *i2c_p->sclpin & i2c_p->scl;
    if (!the_sda_pin && the_scl_pin && what_happened == 'S')
      what_happened = 'S';  // SDA low and SCL still high
    else
      what_happened = '?';  // Huh?, one of them is wrong
    I2C_SP_HALF_DELAY(i2c_p->delay);
    I2C_SP_SCL_LOW(i2c_p);
    the_sda_pin = *i2c_p->sdapin & i2c_p->sda;
    the_scl_pin = *i2c_p->sclpin & i2c_p->scl;
    if (!the_sda_pin && !the_scl_pin && what_happened == 'S')
      what_happened = 'S';  // SDA low and SCL now low
    else
      what_happened = '?';  // Huh?, one of them is wrong
    I2C_SP_HALF_DELAY(i2c_p->delay);
  }
  return what_happened;
}

// Stop
char I2C_SP_STOP ( i2c_soft_port* i2c_p )
{
  uint8_t the_sda_pin, the_scl_pin = 0;
  char what_happened = 0;
  if (i2c_p->enabled) { 
    I2C_SP_HALF_DELAY(i2c_p->delay);
    I2C_SP_SCL_LOW(i2c_p);
    I2C_SP_HALF_DELAY(i2c_p->delay);
    I2C_SP_SDA_LOW(i2c_p);
    I2C_SP_HALF_DELAY(i2c_p->delay);
    the_sda_pin = *i2c_p->sdapin & i2c_p->sda;
    the_scl_pin = *i2c_p->sclpin & i2c_p->scl;
    if (!the_sda_pin && !the_scl_pin)
      what_happened = 'P';  // Both low, so far so good
    else
      what_happened = '?';  // Huh?, one of them is high
    I2C_SP_SCL_HIGH(i2c_p);
    I2C_SP_HALF_DELAY(i2c_p->delay);
    the_sda_pin = *i2c_p->sdapin & i2c_p->sda;
    the_scl_pin = *i2c_p->sclpin & i2c_p->scl;
    if (!the_sda_pin && the_scl_pin && what_happened == 'P')
      what_happened = 'P';  // SDA low and SCL now high
    else
      what_happened = '?';  // Huh?, one of them is wrong
    I2C_SP_SDA_HIGH(i2c_p);
    I2C_SP_HALF_DELAY(i2c_p->delay);
    the_sda_pin = *i2c_p->sdapin & i2c_p->sda;
    the_scl_pin = *i2c_p->sclpin & i2c_p->scl;
    if (the_sda_pin && the_scl_pin && what_happened == 'P')
      what_happened = 'P';  // SDA high and SCL high
    else
      what_happened = '?';  // Oops, one of them is wrong
  } else what_happened = '?'; // port not enabled
  return what_happened;
}


// Ack
char I2C_SP_ACK ( i2c_soft_port* i2c_p )
{
  char what_happened = 0;
  if (i2c_p->enabled) { 
    I2C_SP_SDA_LOW(i2c_p);
    if (I2C_SP_SCL_TOGGLE(i2c_p))
      what_happened = 'N';
    else
      what_happened = 'K';
  } else  what_happened = '?'; // port not enabled
  return what_happened;
}


// Nack
char I2C_SP_NACK ( i2c_soft_port* i2c_p )
{
  char what_happened = 0;
  if (i2c_p->enabled) { 
    I2C_SP_SDA_HIGH(i2c_p);
    if (I2C_SP_SCL_TOGGLE(i2c_p))
      what_happened = 'N';
    else
      what_happened = 'K';
  } else  what_happened = '?'; // port not enabled
  return what_happened;
}


// Send a Nibble with Readback
uint8_t I2C_SP_SEND_RECV_I2C_HEX( i2c_soft_port* i2c_p, uint8_t input_nibble )
{
  uint8_t the_sda_pin, output_nibble = 0;
  if (i2c_p->enabled) { 
    for (int i = 3 ; i >= 0 ; i-- ) {
      output_nibble <<= 1;
      if ( input_nibble & (1<<i) )
        I2C_SP_SDA_HIGH(i2c_p);
      else
        I2C_SP_SDA_LOW(i2c_p);
      the_sda_pin = I2C_SP_SCL_TOGGLE(i2c_p);
      if (the_sda_pin) output_nibble |= 1;
    }
  }
  return output_nibble;
}


// Put out a BYTE with Acknowledge
char i2c_sp_Putbyte ( i2c_soft_port* i2c_p, uint8_t the_byte)
{
  char what_happened = 0;
  if (i2c_p->enabled) { 
    for (int i = 7 ; i >= 0 ; i--) {
      if ( the_byte & (1<<i) )
        I2C_SP_SDA_HIGH(i2c_p);
      else 
        I2C_SP_SDA_LOW(i2c_p);
      I2C_SP_SCL_TOGGLE(i2c_p);
    }
    // return K for ACk and N for NACK
    what_happened = I2C_SP_NACK(i2c_p);
  } else  what_happened = '?'; // port not enabled
  return what_happened;
}


// Get a BYTE with Acknowledge or NACK if last
uint8_t i2c_sp_Getbyte ( i2c_soft_port* i2c_p, uint8_t last_byte )
{
  uint8_t b = 0;
  if (i2c_p->enabled) { 
    b = I2C_SP_SEND_RECV_I2C_HEX(i2c_p, 0x0F);
    b <<= 4;
    b |= I2C_SP_SEND_RECV_I2C_HEX(i2c_p, 0x0F);
    if (last_byte)
      I2C_SP_NACK(i2c_p); // set NAK
    else
      I2C_SP_ACK(i2c_p);  // set ACK
    // return received byte
  }
  return b;
}


/**************************************************************************
*                                                                         *
*        SMBUS Sanctioned Transactions                                    *
*                                                                         *
***************************************************************************/


// SMBUS Receive BYTE
uint8_t SMBUS_sp_Receive_Byte ( i2c_soft_port* i2c_p, uint8_t address )
{
  uint8_t success = 1;
  uint8_t the_byte = 0xFF;
  if (i2c_p->enabled) { 
    success = (I2C_SP_START(i2c_p) == 'S');  // START
    if (success)
      success = (i2c_sp_Putbyte(i2c_p, address|READ) == 'K'); // READ ADDRESS
    if (success)
      the_byte = (i2c_sp_Getbyte(i2c_p, LAST)); // PULL THE BYTE W/NACK
    if (success) (I2C_SP_STOP(i2c_p)); // STOP
    if (!success) {
      // RELEASE THE BUS
      I2C_SP_SDA_HIGH(i2c_p);
      I2C_SP_SCL_HIGH(i2c_p);
    }
  }
  return the_byte;
}

// SMBUS Send BYTE
void SMBUS_sp_Send_Byte ( i2c_soft_port* i2c_p, uint8_t address, uint8_t data )
{
  uint8_t success = 1;
  if (i2c_p->enabled) { 
    success = (I2C_SP_START(i2c_p) == 'S'); // START
    if (success)  // WRITE ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, address) == 'K');
    if (success)  // PUSH THE BYTE
      (i2c_sp_Putbyte(i2c_p, data));
    if (success)  // STOP
      (I2C_SP_STOP(i2c_p));
    if (!success) { // RELEASE THE BUS
      I2C_SP_SDA_HIGH(i2c_p);
      I2C_SP_SCL_HIGH(i2c_p);
    }
  }
}

// SMBUS ARA
uint8_t SMBUS_sp_Alert_Responce ( i2c_soft_port* i2c_p )
{
  uint8_t success = 1;
  uint8_t the_byte = 0xFF;
  uint8_t ara_addr7b = 0x0C;

  if (i2c_p->enabled) { 
    success = (I2C_SP_START(i2c_p) == 'S'); // START
    if (success)    // wRITE ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, ara_addr7b << 1 | READ) == 'K');
    if (success)    // PULL THE BYTE W/NACK
      the_byte = (i2c_sp_Getbyte(i2c_p, LAST)); 
    if (success)    // STOP
      (I2C_SP_STOP(i2c_p));
    if (!success) { // RELEASE THE BUS
      I2C_SP_SDA_HIGH(i2c_p);
      I2C_SP_SCL_HIGH(i2c_p);
    }
  }
  return the_byte;
}


// SMBUS ARA with PEC
uint16_t SMBUS_sp_Alert_Responce_PEC ( i2c_soft_port* i2c_p )
{
  uint8_t success = 1;
  uint8_t the_byte = 0xFF;
  uint8_t pec_byte = 0xFF;
  uint8_t ara_addr7b = 0x0C;

  if (i2c_p->enabled) { 
    success = (I2C_SP_START(i2c_p) == 'S'); // START
    if (success)    // wRITE ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, ara_addr7b << 1 | READ) == 'K');
    if (success)    // PULL THE BYTE
      the_byte = (i2c_sp_Getbyte(i2c_p, !LAST)); 
    if (success)    // PULL THE PEC BYTE W/NACK
      pec_byte = (i2c_sp_Getbyte(i2c_p, LAST)); 
    if (success)    // STOP
      (I2C_SP_STOP(i2c_p));
    if (!success) { // RELEASE THE BUS
      I2C_SP_SDA_HIGH(i2c_p);
      I2C_SP_SCL_HIGH(i2c_p);
    }
  }
  return ((uint16_t)pec_byte << 8 | (uint16_t)the_byte);
}


// SMBUS Read BYTE
uint8_t SMBUS_sp_Read_Byte ( i2c_soft_port* i2c_p, uint8_t address, uint8_t subaddress )
{
  uint8_t success = 1;
  uint8_t the_byte = 0xFF;
  if (i2c_p->enabled) { 
    success = (I2C_SP_START(i2c_p) == 'S'); // START
    if (success)    // wRITE ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, address) == 'K');
    if (success)    // SUB ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, subaddress) == 'K');
    if (success)    // RESTART
      success = (I2C_SP_START(i2c_p) == 'S');
    if (success)    // READ ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, address|READ) == 'K');
    if (success)    // PULL THE BYTE W/NACK
      the_byte = (i2c_sp_Getbyte(i2c_p, LAST)); 
    if (success)    // STOP
      (I2C_SP_STOP(i2c_p));
    if (!success) { // RELEASE THE BUS
      I2C_SP_SDA_HIGH(i2c_p);
      I2C_SP_SCL_HIGH(i2c_p);
    }
  }
  return the_byte;
}


// SMBUS Read BYTE with PEC
uint16_t SMBUS_sp_Read_Byte_PEC ( i2c_soft_port* i2c_p, uint8_t address, uint8_t subaddress )
{
  uint8_t success = 1;
  uint8_t the_byte = 0xFF;
  uint8_t pec_byte = 0xFF;
  if (i2c_p->enabled) { 
    success = (I2C_SP_START(i2c_p) == 'S'); // START
    if (success)    // wRITE ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, address) == 'K');
    if (success)    // SUB ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, subaddress) == 'K');
    if (success)    // RESTART
      success = (I2C_SP_START(i2c_p) == 'S');
    if (success)    // READ ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, address|READ) == 'K');
    if (success)    // PULL THE BYTE W/NACK
      the_byte = (i2c_sp_Getbyte(i2c_p, LAST)); 
    if (success)    // STOP
      (I2C_SP_STOP(i2c_p));
    if (!success) { // RELEASE THE BUS
      I2C_SP_SDA_HIGH(i2c_p);
      I2C_SP_SCL_HIGH(i2c_p);
    }
  }
  return ((uint16_t)pec_byte << 8 | (uint16_t)the_byte) ;
}


// SMBUS Write BYTE
uint8_t SMBUS_sp_Write_Byte ( i2c_soft_port* i2c_p, uint8_t address, uint8_t subaddress, uint8_t data )
{
  uint8_t success = 1;
  if (i2c_p->enabled) { 
    success = (I2C_SP_START(i2c_p) == 'S'); // START
    if (success)    // WRITE ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, address) == 'K');
    if (success)    // SUB ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, subaddress) == 'K');
    if (success)    // PUSH THE BYTE
      success = (i2c_sp_Putbyte(i2c_p, data) == 'K');
    if (success)    // STOP
      (I2C_SP_STOP(i2c_p));
    if (!success) { // RELEASE THE BUS
      I2C_SP_SDA_HIGH(i2c_p);
      I2C_SP_SCL_HIGH(i2c_p);
    }
  } else success = 0; // port not enabled
  return success;
}


// SMBUS Write BYTE with PEC
uint8_t SMBUS_sp_Write_Byte_PEC ( i2c_soft_port* i2c_p, uint8_t address, uint8_t subaddress, uint8_t data, uint8_t pec_byte )
{
  uint8_t success = 1;
  if (i2c_p->enabled) { 
    success = (I2C_SP_START(i2c_p) == 'S'); // START
    if (success)    // WRITE ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, address) == 'K');
    if (success)    // SUB ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, subaddress) == 'K');
    if (success)    // PUSH THE BYTE
      success = (i2c_sp_Putbyte(i2c_p, data) == 'K');
    if (success)    // PUSH PEC
      success = (i2c_sp_Putbyte(i2c_p, pec_byte) == 'K');
    if (success)    // STOP
      (I2C_SP_STOP(i2c_p));
    if (!success) { // RELEASE THE BUS
      I2C_SP_SDA_HIGH(i2c_p);
      I2C_SP_SCL_HIGH(i2c_p);
    }
  } else success = 0; // port not enabled
  return success;
}


// SMBUS Read WORD
uint16_t SMBUS_sp_Read_Word ( i2c_soft_port* i2c_p, uint8_t address, uint8_t subaddress )
{
  uint8_t success = 1;
  uint8_t hi_byte, lo_byte = 0;
  uint16_t the_word = 0xFFFF;
  if (i2c_p->enabled) { 
    success = (I2C_SP_START(i2c_p) == 'S'); // START
    if (success)    // WRITE ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, address) == 'K');
    if (success)    // SUB ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, subaddress) == 'K');
    if (success)    // RESTART
      success = (I2C_SP_START(i2c_p) == 'S');
    if (success)    // READ ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, address|READ) == 'K');
    if (success)    // PULL THE LOW BYTE
      lo_byte = (i2c_sp_Getbyte(i2c_p, !LAST));
    if (success)    // PULL THE HIGH BYTE W/NACK
      hi_byte = (i2c_sp_Getbyte(i2c_p, LAST));
    if (success)    // STOP
      (I2C_SP_STOP(i2c_p));
    if (!success) { // RELEASE THE BUS
      I2C_SP_SDA_HIGH(i2c_p);
      I2C_SP_SCL_HIGH(i2c_p);
    }
    if (success)    // ASSEMBLED WORD
      the_word = ((uint16_t)hi_byte << 8 | (uint16_t)lo_byte);
  }
  return the_word;
}


// SMBUS Read WORD with PEC
uint32_t SMBUS_sp_Read_Word_PEC ( i2c_soft_port* i2c_p, uint8_t address, uint8_t subaddress )
{
  uint8_t success = 1;
  uint8_t hi_byte = 0xFF;
  uint8_t lo_byte = 0xFF;
  uint32_t results = 0x00FFFFFF;
  uint8_t pec_byte = 0xFF;
  if (i2c_p->enabled) { 
    success = (I2C_SP_START(i2c_p) == 'S'); // START
    if (success)    // WRITE ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, address) == 'K');
    if (success)    // SUB ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, subaddress) == 'K');
    if (success)    // RESTART
      success = (I2C_SP_START(i2c_p) == 'S');
    if (success)    // READ ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, address|READ) == 'K');
    if (success)    // PULL THE LOW BYTE
      lo_byte = (i2c_sp_Getbyte(i2c_p, !LAST));
    if (success)    // PULL THE HIGH BYTE
      hi_byte = (i2c_sp_Getbyte(i2c_p, !LAST));
    if (success)    // PULL THE PEC BYTE W/NACK
      pec_byte = (i2c_sp_Getbyte(i2c_p, LAST));
    if (success)    // STOP
      (I2C_SP_STOP(i2c_p));
    if (!success) { // RELEASE THE BUS
      I2C_SP_SDA_HIGH(i2c_p);
      I2C_SP_SCL_HIGH(i2c_p);
    }
  }
  // Return ASSEMBLED WORD
  return (results &
          ((uint32_t)pec_byte << 16 | (uint32_t)hi_byte << 8 | (uint32_t)lo_byte));
}


// SMBUS Write WORD
uint8_t SMBUS_sp_Write_Word ( i2c_soft_port* i2c_p, uint8_t address, uint8_t subaddress, uint8_t hi_byte, uint8_t lo_byte )
{
  uint8_t success = 1;
  if (i2c_p->enabled) { 
    success = (I2C_SP_START(i2c_p) == 'S'); // START
    if (success)    // WRITE ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, address) == 'K');
    if (success)    // SUB ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, subaddress) == 'K');
    if (success)    // PUSH THE LOW BYTE
      success = (i2c_sp_Putbyte(i2c_p, lo_byte) == 'K');
    if (success)    // PUSH THE HIGH BYTE
      success = (i2c_sp_Putbyte(i2c_p, hi_byte) == 'K');
    if (success)    // STOP
      (I2C_SP_STOP(i2c_p));
    if (!success) { // RELEASE THE BUS
      I2C_SP_SDA_HIGH(i2c_p);
      I2C_SP_SCL_HIGH(i2c_p);
    }
  } else success = 0; // Port not enabled
  return success;
}

// SMBUS Write WORD with PEC
uint8_t SMBUS_sp_Write_Word_PEC ( i2c_soft_port* i2c_p, uint8_t address, uint8_t subaddress, uint8_t hi_byte, uint8_t lo_byte, uint8_t pec_byte )
{
  uint8_t success = 1;
  if (i2c_p->enabled) { 
    success = (I2C_SP_START(i2c_p) == 'S'); // START
    if (success)    // WRITE ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, address) == 'K');
    if (success)    // SUB ADDRESS
      success = (i2c_sp_Putbyte(i2c_p, subaddress) == 'K');
    if (success)    // PUSH THE LOW BYTE
      success = (i2c_sp_Putbyte(i2c_p, lo_byte) == 'K');
    if (success)    // PUSH THE HIGH BYTE
      success = (i2c_sp_Putbyte(i2c_p, hi_byte) == 'K');
    if (success)    // PUSH THE PEC BYTE
      success = (i2c_sp_Putbyte(i2c_p, pec_byte) == 'K');
    if (success)    // STOP
      (I2C_SP_STOP(i2c_p));
    if (!success) { // RELEASE THE BUS
      I2C_SP_SDA_HIGH(i2c_p);
      I2C_SP_SCL_HIGH(i2c_p);
    }
  } else success = 0; // Port not enabled
  return success;
}
