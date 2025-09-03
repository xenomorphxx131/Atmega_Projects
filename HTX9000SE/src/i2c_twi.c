//
//     Hardware I�C Driver

#include "i2c_twi.h"

/****************************************************************************
*                     Initialize Port                                       *
*****************************************************************************/
void i2cSetBusSpeed ( i2c_port_t* i2c_p )
{
  disableTwi();
  PRR0 &= ~(1<<PRTWI); //PRTWI bit in PRR0 must be written to zero to enable TWI
  TWBR = i2c_p->br ; // Bit rate reg
  switch (i2c_p->ps) // prescaler
    {
    case 0:
      TWSR &= ~(1<<TWPS0);
      TWSR &= ~(1<<TWPS1);
      break;
    case 1:
      TWSR |= (1<<TWPS0);
      TWSR &= ~(1<<TWPS1);
    case 2:
      TWSR &= ~(1<<TWPS0);
      TWSR |= (1<<TWPS1);
      break;
    case 3:
      TWSR |= (1<<TWPS0);
      TWSR |= (1<<TWPS1);
      break;
    default:
      TWSR |= (1<<TWPS0);
      TWSR &= ~(1<<TWPS1);
    }
  TWCR |= (1<<TWEN); //enable TWI
}
/**************************************************************************
*      TWI Init                                                           *
***************************************************************************/
void i2cTwiInit( i2c_port_t* i2c_p )
{
  *i2c_p->sdaddr  &= ~i2c_p->sda; // Initialze the pin direction to input for HIGH by resistor pull up
  *i2c_p->sclddr  &= ~i2c_p->scl; // Initialze the pin direction to input for HIGH by resistor pull up
  *i2c_p->sdaport &= ~i2c_p->sda; // Permanently set the pin state to low. Use the direction bit
  *i2c_p->sclport &= ~i2c_p->scl; // to set high Z. Requires a pull up resistor to make a "HIGH"
  i2cSetBusSpeed ( i2c_p );
}
/**************************************************************************
*      Disable TWI                                                        *
***************************************************************************/
void disableTwi ( ) 
{ TWCR &= ~(1<<TWEN); } //disable TWI
/**************************************************************************
*      Enable TWI                                                         *
***************************************************************************/
void enableTwi ( )
{ STOP(); } //enable TWI

// check TWCR
// int twcr () { return TWCR; }

// check TWSR
// int twsr () { return TWSR; }
/**************************************************************************
*      Clear Error                                                        *
***************************************************************************/
// clear error 
void clear_bus_err ( ) {
  if (TWSR != 0) {
    START();
    STOP();
  }
  STOP();
}
/**************************************************************************
*      Complete I²C Tokens                                                *
***************************************************************************/
/**************************************************************************
*      I²C Start                                                          *
***************************************************************************/
// Start
char START ( )
{

  char results = 0;
  int timout=0;

  for(int n=0;n < MAX_ITER; n++) {

    // Check for Error condition TWSR==0
    if (TWSR == 0) clear_bus_err();

    // Send Start
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

    // wait for Status Bits
    while ((timout<MAX_ITER) && (!(TWCR & (1<<TWINT)))) {
      //Delay_MS(1);
      _delay_us(50);
      timout++;
    }

    if (timout==MAX_ITER){
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
/**************************************************************************
*      I²C Stop                                                           *
***************************************************************************/
// Stop
char STOP ( )
{
  TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN); // send stop condition
  return 'P';
}
/**************************************************************************
*      I²C PutByte                                                        *
***************************************************************************/
// Put out a BYTE with Acknowledge
char i2cPutbyte ( uint8_t the_byte )
{
  int timout=0;
  char results = 0;
  TWDR = the_byte;
  //Clear interrupt to start transmission
  TWCR = (1<<TWINT) | (1<<TWEN);
  // Wait for status bits
  while ((timout<MAX_ITER) && (!(TWCR & (1<<TWINT)))) {
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
