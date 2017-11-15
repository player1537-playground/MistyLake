#ifndef defs_h
#define defs_h

#define IQS5xx_ADDR          	0x75
#define RDY_BIT              	BIT12
#define RDY_PIN              	12
#define	END_WINDOW				(uint16_t)0xEEEE

// #define I2C_SPEED				100000		// 100kHz i2c clock
#define I2C_SPEED				400000		// 400kHz i2c clock

#define BitIsSet(VAR,Index)		(VAR & (1<<Index)) != 0
#define BitIsNotSet(VAR,Index)	(VAR & (1<<Index)) == 0

#define SetBit(VAR,Index)		VAR |= (1<<Index)
#define ClearBit(VAR,Index)		VAR &= (uint8_t)(~(1<<Index))

// BOOTLOADER DEFINES
#define NRST_BIT				BIT13
#define NRST_PIN				13
#define	BL_ADDR					(IQS5xx_ADDR^0x40)
#define	BL_CMD_VERSION			0x00
#define BL_CMD_READ				0x01
#define	BL_CMD_EXIT				0x02
#define	BL_CMD_CRCCHECK			0x03

#include "os_type.h"

void IQS5xx_AcknowledgeReset(); 
void IQS5xx_CheckVersion();
void IQS5xx_Reset();
void ProgramIQS5xx();
void PrintMemory(uint8 block);
void PrintChecksum();
void ProgramLastBlock();
void EnterBootloaderIQS5xx();

#endif