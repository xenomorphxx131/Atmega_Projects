/********************************************************************************
 *                                                                              *
 *   Hardware I²C Driver                                                        *
 *                                                                              *
 ********************************************************************************/

#include "i2c_twi.h"

/********************************************************************************
 *   Configure Port Speed                                                       *
 ********************************************************************************/
void i2cSetBusSpeed(i2c_twi_port_t* i2c_p)
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
            break;
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
    TWCR |= _BV(TWEN);       //enable TWI
}
/********************************************************************************
 *  Initialize Port                                                             *
 ********************************************************************************/
void i2cTwiInit(i2c_twi_port_t *i2c_port)
{
    *i2c_port->sdaddr  &= ~i2c_port->sda;   // Initialze the pin direction to input
    *i2c_port->sclddr  &= ~i2c_port->scl;   // Initialze the pin direction to input
    *i2c_port->sdaport |= i2c_port->sda;    // Pin value to High with direction low enables pull up
    *i2c_port->sclport |= i2c_port->scl;    // Pin value to High with direction low enables pull up
    i2cSetBusSpeed(i2c_port);
}
/********************************************************************************
 *  Enable Port                                                                 *
 ********************************************************************************/
void enableTwi()
{
    STOP();
}
/********************************************************************************
 *  Disable Port                                                                *
 ********************************************************************************/
void disableTwi()
{
    TWCR &= ~_BV(TWEN);
}
/********************************************************************************
*  Clear Bus Error                                                              *
*********************************************************************************/
void clear_bus_err()
{
    if (TWSR != 0)
    {
        START();
        STOP();
    }
    STOP();
}
/********************************************************************************
*  Start                                                                        *
*********************************************************************************/
char START()
{
    char results = 0;
    int timout = 0;

    for(int n = 0; n < MAX_ITER; n++)
    {
        if (TWSR == 0)
            clear_bus_err();                         // Check for error condition TWSR==0
        TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);             // Send the actual Start
        while (timout < MAX_ITER && !(TWCR & _BV(TWINT)))       // Wait for status Bits
        {
            _delay_us(50);
            timout++;
        }
        if (timout==MAX_ITER)
        {
            results = '?';                                      // Timout Error
            n = MAX_ITER;
        }
        else 
        {
            switch (TW_STATUS)
            {
                case TW_REP_START:
                    results = 'S';  // good
                    n = MAX_ITER;
                    break;
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
/********************************************************************************
*  Stop                                                                         *
*********************************************************************************/
char STOP()
{
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
    return 'P';
}
/********************************************************************************
*  Put Byte                                                                     *
*********************************************************************************/
char i2cPutbyte(uint8_t the_byte)
{
    int timout=0;
    char results = 0;
    
    TWDR = the_byte;
    TWCR = _BV(TWINT) | _BV(TWEN); //Clear interrupt to start transmission
    while (timout < MAX_ITER && !(TWCR & _BV(TWINT))) // Wait for status bits
    {
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