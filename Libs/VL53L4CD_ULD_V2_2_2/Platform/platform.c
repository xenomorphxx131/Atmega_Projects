/**
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "platform.h"

uint8_t VL53L4CD_RdDWord(Dev_t dev, uint16_t RegisterAdress, uint32_t *value)
{
    I2C_16BITSUB_Read_DWord(dev, RegisterAdress, value);
    return 0;
}

uint8_t VL53L4CD_RdWord(Dev_t dev, uint16_t RegisterAdress, uint16_t *value)
{
    I2C_16BITSUB_Read_Word(dev, RegisterAdress, value);
    return 0;
}

uint8_t VL53L4CD_RdByte(Dev_t dev, uint16_t RegisterAdress, uint8_t *value)
{
    I2C_16BITSUB_Read_Byte(dev, RegisterAdress, value);
    return 0;
}

uint8_t VL53L4CD_WrByte(Dev_t dev, uint16_t RegisterAdress, uint8_t value)
{
    I2C_16BITSUB_Write_Byte(dev, RegisterAdress, value);
    return 0;
}

uint8_t VL53L4CD_WrWord(Dev_t dev, uint16_t RegisterAdress, uint16_t value)
{
    I2C_16BITSUB_Write_Word(dev, RegisterAdress, value);
    return 0;
}

uint8_t VL53L4CD_WrDWord(Dev_t dev, uint16_t RegisterAdress, uint32_t value)
{
    I2C_16BITSUB_Write_DWord(dev, RegisterAdress, value);
    return 0;
}

uint8_t VL53L4CD_WaitMs(Dev_t dev, uint32_t TimeMs)
{
    _delay_ms(TimeMs);
    return 0;
}
