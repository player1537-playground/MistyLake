#ifndef I2C_UTIL_H_
#define I2C_UTIL_H_

#include "os_type.h"

uint8 i2c_master_writeBytes(uint8 address, const uint8 *values, uint8 length);
uint8 i2c_master_readBytes(uint8 address, uint8 *values, uint8 length);

#endif
