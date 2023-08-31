//
//     Hardware I²C Driver

#include "i2c_twi.h"

//***************************************************************************
//*                     initialize Port                                     *
//***************************************************************************/

void i2cSetBusSpeed ( i2c_twi_port_t* i2c_p )
{
  disableTwi();

  PRR0 &= ~_BV(PRTWI);      // PRTWI bit in PRR0 must be written to zero to enable TWI
  TWBR = i2c_p->twbr ;      // Bit rate reg. SCL = FCPU / (16 + 2 * TWBR * 4^TWPS)
  switch (i2c_p->twps)      // prescaler
    {
    case 0:
      TWSR &= ~_BV(TWPS0);
      TWSR &= ~_BV(TWPS1);
      break;
    case 1:
      TWSR |=  _BV(TWPS0);
      TWSR &= ~_BV(TWPS1);
    case 2:
      TWSR &= ~_BV(TWPS0);
      TWSR |=  _BV(TWPS1);
      break;
    case 3:
      TWSR |= _BV(TWPS0);
      TWSR |= _BV(TWPS1);
      break;
    default:
      TWSR |=  _BV(TWPS0);
      TWSR &= ~_BV(TWPS1);
    }

  TWCR |= _BV(TWEN); //enable TWI
}

void i2cTwiInit( i2c_twi_port_t* i2c_p )
{
  *i2c_p->sdaddr  &= ~i2c_p->sda; // Initialze the pin direction to input for HIGH by resistor pull up
  *i2c_p->sclddr  &= ~i2c_p->scl; // Initialze the pin direction to input for HIGH by resistor pull up
  *i2c_p->sdaport &= ~i2c_p->sda; // Permanently set the pin state to low. Use the direction bit
  *i2c_p->sclport &= ~i2c_p->scl; // to set high Z. Requires a pull up resistor to make a "HIGH"

  i2cSetBusSpeed ( i2c_p );

}

void disableTwi() { TWCR &= ~_BV(TWEN); } //disable TWI
void enableTwi() { STOP(); } //enable TWI

void reset_i2c()
{
    disableTwi();
    enableTwi();
}

/**************************************************************************
*      Complete I²C Tokens                                                *
***************************************************************************/

// check TWCR
int twcr() { return TWCR; }

// check TWSR
int twsr() { return TWSR; }

// clear error 
void clear_bus_err () {
  if (TWSR != 0) {
    START();
    STOP();
  }
  STOP();
}

// Start
char START()
{
  char results = 0;
  int timout=0;
  for(int n=0;n < MAX_ITER; n++) {
    // Check for Error condition TWSR==0
    if (TWSR == 0) clear_bus_err();

    // Send Start
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

    // wait for Status Bits
    while ((timout < MAX_ITER) && (!(TWCR & _BV(TWINT)))) {
      //Delay_MS(1);
      _delay_us(50);
      timout++;
    }
    if (timout == MAX_ITER){
      results = '?'; // Timout Error
      n = MAX_ITER;
    } else {
      switch (TW_STATUS)
        {
        case TW_REP_START:
        case TW_START:
          results = 'S';  // good
          n = MAX_ITER;
          break;
        case TW_MT_ARB_LOST:
          results = '?';  // Huh?
          break;
        default:
          results = '?';  // Huh?
          n = MAX_ITER;
        }
    }
  }
  return results;
}

char START2()
{
    // TWCR = _BV(TWINT);
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    return 'S';
}

// Stop
char STOP()
{
  // TWCR = _BV(TWINT);
  TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); // send stop condition
  return 'P';
}

// Put out a BYTE with Acknowledge
char i2cPutbyte ( uint8_t the_byte)
{
  int timout=0;
  char results = 0;

  TWDR = the_byte;

  //Clear interrupt to start transmission
  TWCR = _BV(TWINT) | _BV(TWEN);

  // Wait for status bits
  while ((timout < MAX_ITER) && (!(TWCR & _BV(TWINT)))) {
    _delay_us(50);
    timout++;
  }
  if (timout==MAX_ITER)
    results = 'N'; // Timout Error
  else
    switch (TW_STATUS)
      {
      case TW_MR_SLA_ACK:  // MASTER RECIEVE MODE ACK
      case TW_MT_SLA_ACK:  // MASTER TRANSMIT MODE ACK
      case TW_MT_DATA_ACK: // MASTER TRANSMIT DATA MODE ACK
      case TW_MR_DATA_ACK: // MASTER RECIEVE DATA MODE ACK
        results = 'K';
        break;
      case TW_MT_SLA_NACK: // NACK during select: device busy writing
      case TW_MT_ARB_LOST: // Arbitration lost
      default:             // and anything else
        results = 'N';
      }
  return results;
}

// Get a BYTE with Acknowledge or NACK if last
uint8_t i2cGetbyte ( uint8_t last_byte )
{
  int timout = 0;
  uint8_t results = 0;

  if (last_byte)
    TWCR = _BV(TWINT) | _BV(TWEN);              //Set NAK
  else
    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA); // Set ACK

  // Wait for transmission
  while ((timout < MAX_ITER) && (!(TWCR & _BV(TWINT)))) {
    _delay_us(50);
    timout++;
  }
  if (timout == MAX_ITER)
    results = 0xFF; // Timout return 0xFF
  else
    switch (TW_STATUS)
      {
      case TW_MR_DATA_NACK:
      case TW_MR_DATA_ACK:
        results = TWDR;
        break;
      default:
        results = 0xFF;
      }
  return results;
}

/**************************************************************************
*        SMBUS Sanctioned Transactions                                    *
***************************************************************************/

// SMBUS Receive BYTE
uint8_t SMBUS_Receive_Byte ( uint8_t address )
{
  uint8_t success = 1;
  uint8_t results = 0xFF;
  success = (START() == 'S');  // START
  if (success)
    success = (i2cPutbyte(address|READ) == 'K'); // READ ADDRESS
  if (success)
    results = i2cGetbyte(LAST); // PULL THE BYTE W/NACK
  if (success)
    STOP(); // STOP
  else
    clear_bus_err(); // clear error, reset bus

  return results;
}

// SMBUS Send BYTE
void SMBUS_Send_Byte ( uint8_t address, uint8_t data )
{
  uint8_t success = 1;
  success = (START() == 'S'); // START
  if (success)  // WRITE ADDRESS
    success = (i2cPutbyte(address) == 'K');
  if (success)
    i2cPutbyte(data);// PUSH THE BYTE
  if (success)
    STOP(); // STOP
  else
    clear_bus_err(); // clear error, reset bus
}

// SMBUS ARA
uint8_t SMBUS_Alert_Responce ( )
{
  uint8_t success = 1;
  uint8_t results = 0xFF;
  uint8_t ara_addr7b = 0x0C;

  success = (START() == 'S'); // START
  if (success)    // wRITE ADDRESS
    success = (i2cPutbyte(ara_addr7b << 1 | READ) == 'K');
  if (success)
    results = i2cGetbyte(LAST); // PULL THE BYTE W/NACK
  if (success)
    STOP(); // STOP
  else
    clear_bus_err(); // clear error, reset bus
  return results;
}


// SMBUS ARA with PEC
uint16_t SMBUS_Alert_Responce_PEC ( )
{
  uint8_t success = 1;
  uint8_t results = 0xFF;
  uint8_t pec_byte = 0xFF;
  uint8_t ara_addr7b = 0x0C;

  success = (START() == 'S'); // START
  if (success)    // wRITE ADDRESS
    success = (i2cPutbyte(ara_addr7b << 1 | READ) == 'K');
  if (success)
    results = i2cGetbyte(!LAST); // PULL THE BYTE
  if (success)
    pec_byte = i2cGetbyte(LAST); // PULL THE PEC BYTE W/NACK
  if (success)
    STOP(); // STOP
  else
    clear_bus_err(); // clear error, reset bus
  return ((uint16_t)pec_byte << 8 | (uint16_t)results);
}

// SMBUS Read BYTE
uint8_t SMBUS_Read_Byte ( uint8_t address, uint8_t subaddress )
{
  uint8_t success = 1;
  uint8_t results = 0xFF;
  success = (START() == 'S');                               // START
  if (success)
    success = (i2cPutbyte(address) == 'K');                 // WRITE ADDRESS
  if (success)
    success = (i2cPutbyte(subaddress) == 'K');              // SUB ADDRESS
  if (success)    // RESTART
    success = (START() == 'S');
  if (success)    // READ ADDRESS
    success = (i2cPutbyte(address|READ) == 'K');
  if (success)
    results = i2cGetbyte(LAST); // PULL THE BYTE W/NACK
  if (success)
    STOP(); // STOP
  else
    clear_bus_err(); // clear error, reset bus
  return results;
}

// I2C16_Read_Byte foe use with the VL6180 TOF sensor
uint8_t I2C16_Read_Byte( uint8_t addr7b, uint16_t subaddress )
{
  uint8_t success = 1;
  uint8_t results = 0xFF;
  success = (START() == 'S');                               // START
  if (success)
    success = (i2cPutbyte(addr7b * 2) == 'K');              // WRITE ADDRESS
  if (success)
    success = (i2cPutbyte(subaddress >> 8) == 'K');         // SUB ADDRESS HIGH BYTE
  if (success)
    success = (i2cPutbyte(subaddress & 0xFF) == 'K');       // SUB ADDRESS LOW BYTE
  if (success)
    STOP();                                                 // STOP
  if (success)
    success = (START() == 'S');                             // START
  if (success)
    success = (i2cPutbyte((addr7b * 2)|READ) == 'K');       // READ ADDRESS
  if (success)
    results = i2cGetbyte(LAST);                             // PULL THE BYTE W/NACK
  if (success)
    STOP();                                                 // STOP
  _delay_us(5);                                             // Give it a few us
  if ((TWCR & _BV(TWSTO)) || !success) {reset_i2c();}       // Not sure why on some registers
                                                            // this bit doesn't clear
                                                            // and SDA hangs low
  return results;
}

// I2C16_Write_Byte foe use with the VL6180 TOF sensor
uint8_t I2C16_Write_Byte( uint8_t addr7b, uint16_t subaddress, uint8_t data )
{
  uint8_t success = 1;
  uint8_t results = 0xFF;
  success = (START() == 'S');                               // START
  if (success)
    success = (i2cPutbyte(addr7b * 2) == 'K');              // READ ADDRESS
  if (success)
    success = (i2cPutbyte(subaddress >> 8) == 'K');         // SUB ADDRESS HIGH
  if (success)
    success = (i2cPutbyte(subaddress & 0xFF) == 'K');       // SUB ADDRESS LOW
  if (success)
    success = (i2cPutbyte(data) == 'K');                    // READ ADDRESS
  if (success)
    STOP();                                                 // STOP
    _delay_us(5);
  if ((TWCR & _BV(TWSTO)) || !success) {reset_i2c();}       // Not sure why on some registers
                                                            // this bit doesn't clear
                                                            // and SDA hangs low
  return results;
}

uint16_t I2C16_Read_Word( uint8_t addr7b, uint16_t subaddress )
{
  uint8_t success = 1;
  uint8_t hibyte = 0xFF;
  uint8_t lobyte = 0xFF;
  success = (START() == 'S');                               // START
  if (success)
    success = (i2cPutbyte(addr7b * 2) == 'K');              // WRITE ADDRESS
  if (success)
    success = (i2cPutbyte(subaddress >> 8) == 'K');         // SUB ADDRESS HIGH BYTE
  if (success)
    success = (i2cPutbyte(subaddress & 0xFF) == 'K');       // SUB ADDRESS LOW BYTE
  if (success)
    STOP();                                                 // STOP
  if (success)
    success = (START() == 'S');                             // START
  if (success)
    success = (i2cPutbyte((addr7b * 2)|READ) == 'K');       // READ ADDRESS
  if (success)
    hibyte = i2cGetbyte(!LAST);                             // PULL THE BYTE W/ACK
  if (success)
    lobyte = i2cGetbyte(LAST);                              // PULL THE BYTE W/NACK
  if (success)
    STOP();                                                 // STOP
  _delay_us(5);                                             // Give it a few us
  if ((TWCR & _BV(TWSTO)) || !success) {reset_i2c();}       // Not sure why on some registers
                                                            // this bit doesn't clear
                                                            // and SDA hangs low
  return (hibyte << 8) | lobyte;
}

// SMBUS Read BYTE with PEC
uint16_t SMBUS_Read_Byte_PEC ( uint8_t address, uint8_t subaddress )
{
  uint8_t success = 1;
  uint8_t results = 0xFF;
  uint8_t pec_byte = 0xFF;
  success = (START() == 'S'); // START
  if (success)    // WRITE ADDRESS
    success = (i2cPutbyte(address) == 'K');
  if (success)    // SUB ADDRESS
    success = (i2cPutbyte(subaddress) == 'K');
  if (success)    // RESTART
    success = (START() == 'S');
  if (success)    // READ ADDRESS
    success = (i2cPutbyte(address|READ) == 'K');
  if (success)
    results = i2cGetbyte(!LAST); // PULL THE BYTE
  if (success)
    pec_byte = i2cGetbyte(LAST); // PULL THE PEC BYTE W/NACK
  if (success)
    STOP(); // STOP
  else
    clear_bus_err(); // clear error, reset bus

  // return 16 bit ASSEMBLED RESULTS = { PEC_BYTE, RESULTS }
  return (((uint16_t)pec_byte << 8) | (uint16_t)results);
}

// SMBUS Write BYTE
uint8_t SMBUS_Write_Byte ( uint8_t address, uint8_t subaddress, uint8_t data )
{
  uint8_t success = 1;
  success = (START() == 'S'); // START
  if (success) // WRITE ADDRESS
    success = (i2cPutbyte(address) == 'K');
  if (success) // SUB ADDRESS
    success = (i2cPutbyte(subaddress) == 'K');
  if (success) // PUSH THE BYTE
    success = (i2cPutbyte(data) == 'K');
  if (success)
    STOP(); // STOP
  else
    clear_bus_err(); // clear error, reset bus
  return success;
}

// SMBUS Write BYTE with PEC
uint8_t SMBUS_Write_Byte_PEC ( uint8_t address, uint8_t subaddress, uint8_t data, uint8_t pec_byte )
{
  uint8_t success = 1;
  success = (START() == 'S'); // START
  if (success) // WRITE ADDRESS
    success = (i2cPutbyte(address) == 'K');
  if (success) // SUB ADDRESS
    success = (i2cPutbyte(subaddress) == 'K');
  if (success) // PUSH THE BYTE
    success = (i2cPutbyte(data) == 'K');
  if (success) // PUSH THE PEC BYTE
    success = (i2cPutbyte(pec_byte) == 'K');
  if (success)
    STOP(); // STOP
  else
    clear_bus_err(); // clear error, reset bus
  return success;
}

// SMBUS Read WORD
uint16_t SMBUS_Read_Word ( uint8_t address, uint8_t subaddress )
{
  uint8_t success = 1;
  uint8_t hi_byte = 0xFF;
  uint8_t lo_byte = 0xFF;
  success = (START() == 'S'); // START
  if (success)    // WRITE ADDRESS
    success = (i2cPutbyte(address) == 'K');
  if (success)    // SUB ADDRESS
    success = (i2cPutbyte(subaddress) == 'K');
  if (success)    // RESTART
    success = (START() == 'S');
  if (success)    // READ ADDRESS
    success = (i2cPutbyte(address|READ) == 'K');
  if (success) {
    // PULL THE LOW BYTE
    lo_byte = i2cGetbyte(!LAST);
    // PULL THE HIGH BYTE W/NACK
    hi_byte = i2cGetbyte(LAST);
  }

  if (success)
    STOP(); // STOP
  else
    clear_bus_err(); // clear error, reset bus

  // return ASSEMBLED WORD
  return ((uint16_t)hi_byte << 8 | (uint16_t)lo_byte);
}

// SMBUS Read WORD with PEC
uint32_t SMBUS_Read_Word_PEC ( uint8_t address, uint8_t subaddress )
{
  uint8_t success = 1;
  uint8_t hi_byte = 0xFF;
  uint8_t lo_byte = 0xFF;
  uint8_t pec_byte = 0xFF;

  success = (START() == 'S'); // START
  if (success)    // WRITE ADDRESS
    success = (i2cPutbyte(address) == 'K');
  if (success)    // SUB ADDRESS
    success = (i2cPutbyte(subaddress) == 'K');
  if (success)    // RESTART
    success = (START() == 'S');
  if (success)    // READ ADDRESS
    success = (i2cPutbyte(address|READ) == 'K');
  if (success) {
    // PULL THE LOW BYTE
    lo_byte = i2cGetbyte(!LAST);
    // PULL THE HIGH BYTE
    hi_byte = i2cGetbyte(!LAST);
    // PULL THE PEC BYTE W/NACK
    pec_byte = i2cGetbyte(LAST);
  }

  if (success)
    STOP(); // STOP
  else
    clear_bus_err(); // clear error, reset bus

  return ((uint32_t)pec_byte << 16 | (uint32_t)hi_byte << 8 | (uint32_t)lo_byte);
}

// SMBUS Write WORD
uint8_t SMBUS_Write_Word ( uint8_t address, uint8_t subaddress, uint8_t hi_byte, uint8_t lo_byte )
{
  uint8_t success = 1;
  success = (START() == 'S'); // START
  if (success)  // WRITE ADDRESS
    success = (i2cPutbyte(address) == 'K');
  if (success)  // SUB ADDRESS
    success = (i2cPutbyte(subaddress) == 'K');
  if (success)  // PUSH THE LOW BYTE
    success = (i2cPutbyte(lo_byte) == 'K');
  if (success)  // PUSH THE HIGH BYTE
    success = (i2cPutbyte(hi_byte) == 'K');
  if (success)
    STOP(); // STOP
  else
    clear_bus_err(); // clear error, reset bus

  return success;
}

// SMBUS Write WORD with PEC
uint8_t SMBUS_Write_Word_PEC ( uint8_t address, uint8_t subaddress, uint8_t hi_byte, uint8_t lo_byte, uint8_t pec_byte )
{
  uint8_t success = 1;
  success = (START() == 'S'); // START
  if (success)  // WRITE ADDRESS
    success = (i2cPutbyte(address) == 'K');
  if (success)  // SUB ADDRESS
    success = (i2cPutbyte(subaddress) == 'K');
  if (success)  // PUSH THE LOW BYTE
    success = (i2cPutbyte(lo_byte) == 'K');
  if (success)  // PUSH THE HIGH BYTE
    success = (i2cPutbyte(hi_byte) == 'K');
  if (success)  // PUSH THE PEC BYTE
    success = (i2cPutbyte(pec_byte) == 'K');
  if (success)
    STOP(); // STOP
  else
    clear_bus_err(); // clear error, reset bus

  return success;
}
