
#include "defs.h"
#include "I2C.h"
#include "IQS5xx.h"
#include "osapi.h"
#include "os_type.h"
#include "gpio.h"
//
// Local function prototypes
//
void Print_signed(int num);
void Print_unsigned(uint16 num);

extern uint8 	Data_Buff[44];
extern uint16	ui16SnapStatus[15], ui16PrevSnap[15];

//*****************************************************************************
//
//! Acknowledge the reset flag
//!
//! This function simply sets the ACK_RESET bit found in the SYSTEM_CONTROL_0 
//! register.  By setting this bit the SHOW_RESET flag is cleared in the 
//! SYSTEM_INFO_0 register.  During normal operation, the SHOW_RESET bit can be 
//! monitored and if it becomes set, then an unexpected reset has occurred.  
//! If any device configuration is needed, it must then be repeated.
//!
//! \param None
//!
//! \return None
//
//*****************************************************************************
void ICACHE_FLASH_ATTR IQS5xx_AcknowledgeReset(void) 
{
    uint8 System_ctrl_0 = ACK_RESET;  

	I2C_Write_16(IQS5xx_ADDR, SystemControl0_adr, &System_ctrl_0, 1);
}


//*****************************************************************************
//
//! Reset the IQS5xx with the NRST line
//!
//! This function relies on the fact that the IQS5xx NRST pin is connected to
//! the Arduino digital I/O 6.  Note this is not connected on the IQS572 shield 
//! PCB, and needs to be manually wired to gain access to the NRST net. 
//! The I/O is simply set to output, guaranteed to go LOW, and then changed 
//! to an input so that the internal pull-up on the IQS5xx releases it from 
//! reset.
//!
//! \param None
//!
//! \return None
//
//*****************************************************************************
void ICACHE_FLASH_ATTR IQS5xx_Reset(void) 
{
	gpio_output_set(0, NRST_BIT, NRST_BIT, 0);
	os_delay_us(10);
	gpio_output_set(0, 0, 0, NRST_BIT);
}


void ICACHE_FLASH_ATTR EnterBootloaderIQS5xx(void) 
{
	uint8 ui8DataBuffer[64];
	
	IQS5xx_Reset();
	os_delay_us(1000);
	// 
	// Change i2c slave to the bootloader address, which is defined as 0x40
	// XOR'ed with the slave address
	//
	//
	// Now poll on that address to enter bootloader
	//
	while(!I2C_Read_8(BL_ADDR, BL_CMD_VERSION, &ui8DataBuffer[0] ,2))
	{
		os_printf("Polling to enter bootloader...");
	};
	if((ui8DataBuffer[0] == 0x02) && (ui8DataBuffer[1] == 0x00))
	{
		os_printf("Bootloader version correct: 2.\n");
	}
	else
	{
		os_printf("Incorrect bootloader version: %u.%u\n", ui8DataBuffer[0], ui8DataBuffer[1]);	
		
	};
}

void ICACHE_FLASH_ATTR PrintChecksum() {
	uint8 ui8DataBuffer[64];
	
	BL_Block_Read_16(BL_ADDR, 0x83C0, &ui8DataBuffer[0], 64);
	uint8 ind;
	os_printf("Checksum: ");
	for(ind = 0; ind < 64; ind++) {
		os_printf(" %u", ui8DataBuffer[ind]);
	}
	os_printf("\n");
}

void ICACHE_FLASH_ATTR PrintMemory(uint8 blockNo) {
	
	uint8 ui8DataBuffer[64];
	
	BL_Block_Read_16(BL_ADDR, 0x8400 + (blockNo*64), &ui8DataBuffer[0], 64);
	uint8 ind;
	os_printf("Block %u: ", blockNo);
	for(ind = 0; ind < 64; ind++) {
		os_printf(" %u", ui8DataBuffer[ind]);
	}
	os_printf("\n");
	
}

//uint8 lastBlock[64] = {3, 255, 3, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 63, 7, 0, 100, 0, 25, 1, 44, 0, 150, 1, 44, 0, 0, 7, 208, 23, 0, 50, 37, 0, 50, 0, 25, 0, 0, 0, 1, 1, 0};
uint8 lastBlock[64] = {3, 255, 3, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 63, 7, 0, 100, 0, 25, 1, 44, 0, 150, 1, 44, 0, 0, 7, 208, 23, 0, 50, 37, 0, 50, 0, 25, 0, 0, 0, 1, 1, 0x75};

void ICACHE_FLASH_ATTR ProgramLastBlock() {
	uint8 ui8BlockNo = 239;
	I2C_Write_16(BL_ADDR, 0x8400 + (ui8BlockNo*64), &lastBlock[0], 64);
	os_delay_us(7000);
}

//*****************************************************************************
//
//! Program the IQS5xx through the i2c bootloader
//!
//! This function resets the IQS5xx device, by means of toggling the digital
//! pin 7 (which possibly needs to be manually wired to the NRST on the IQS5xx)
//! Immediately after reset the bootloader is polled on the i2c, and once this
//! is acknowledged, then bootloader is entered.  The version is then confirmed,
//! and then the device is programmed.  A CRC check is done, and also a read
//! back of the section excluded in the CRC to confirm successful programming.
//! Then the IC is reset again, or bootloader exit command is given.
//!
//! \param None
//!
//! \return None
//
//*****************************************************************************
void ICACHE_FLASH_ATTR ProgramIQS5xx(void) 
{
	uint8 ui8DataBuffer[64], ui8BlockNo, ui8ByteNo, ui8Temp;
	bool bNVFail;
	
	// Reset the IQS5xx so that the window opportunity after reset can be used 
	// to enter the bootloader mode.
	//
	IQS5xx_Reset();
	os_delay_us(1000);
	// 
	// Change i2c slave to the bootloader address, which is defined as 0x40
	// XOR'ed with the slave address
	//
	//
	// Now poll on that address to enter bootloader
	//
	while(!I2C_Read_8(BL_ADDR, BL_CMD_VERSION, &ui8DataBuffer[0] ,2))
	{
		os_printf("Polling to enter bootloader...");
	};
	if((ui8DataBuffer[0] == 0x02) && (ui8DataBuffer[1] == 0x00))
	{
		os_printf("Bootloader version correct: 2.\n");
	}
	else
	{
		os_printf("Incorrect bootloader version: %u.%u\n", ui8DataBuffer[0], ui8DataBuffer[1]);	
		
	};
	
	uint8 blockNo;
	for(blockNo = 220; blockNo < 240; blockNo++) {
	BL_Block_Read_16(BL_ADDR, 0x8400 + (blockNo*64), &ui8DataBuffer[0], 64);
	uint8 ind;
	for(ind = 0; ind < 64; ind++) {
		os_printf(" %u", ui8DataBuffer[ind]);
	}
	os_printf("\n");
	}
	
	//
	// Now write the program area, which is address 0x8400 to 0xBFFF.
	// This is written in blocks of 64 bytes, and gives a total of 240
	// such blocks required to write.
	//
	/*for (ui8BlockNo = 0; ui8BlockNo < 240; ui8BlockNo++)
    {
		for (ui8ByteNo = 0; ui8ByteNo < 64; ui8ByteNo++)
		{
			ui8DataBuffer[ui8ByteNo] = pgm_read_byte_near(hex_array + ((ui8BlockNo*64)+ui8ByteNo));
		}
		I2C_Write_16(BL_ADDR, 0x8400 + (ui8BlockNo*64), &ui8DataBuffer[0], 64);
		//
		// This delay is necessary to give the bootloader time to write 
        // the 64 byte buffer into memory
        // This delay is only necessary after each block write
        // 
		os_delay_us(7000);
    }
	//
	// Now program the checksum descriptor, so that the CRC check can be run
	//
	I2C_Write_16(BL_ADDR, 0x83C0, &crc_array[0], 64);
	delay(7000);
	//
	// Perform CRC check, a '0x00' returned is pass, '0x01' is fail
	//
	I2C_Read_8(BL_ADDR, BL_CMD_CRCCHECK, &ui8DataBuffer[0], 1);
	
	if(ui8DataBuffer[0] == 0x00)
	{
		os_printf("CRC Success");
	}
	else
	{
		os_printf("CRC Fail");
	}
	//
	// The IQS5xx-B000 non-volatile memory does not form part of the CRC 
	// checksum, so a read back and compare is needed to verify this section, 
	// which is located 0xBE00 to 0xBFFF (512 bytes, or 8x64 blocks)
	//
	bNVFail = FALSE;
	for (ui8BlockNo = 0; ui8BlockNo < 8; ui8BlockNo++)
    {
		BL_Block_Read_16(BL_ADDR, 0xBE00 + (ui8BlockNo*64), &ui8DataBuffer[0], 64);
		//
		// Now compare each byte read back to that in the array, to make sure 
        // each byte is correct.
        //
		for (ui8ByteNo = 0; ui8ByteNo < 64; ui8ByteNo++)
		{
			// this read method is needed because the data is stored in program memory
			//
			ui8Temp = pgm_read_byte_near(hex_array + (0xBE00-0x8400) + (ui8BlockNo*64) + ui8ByteNo);
			if(ui8DataBuffer[ui8ByteNo] != ui8Temp)
			{
				bNVFail = TRUE;
			}
		}
    }
	if(bNVFail)
	{
		os_printf("Non-volatile read back: FAIL");
	}
	else
	{
		os_printf("Non-volatile read back: PASS");
	}
	//
	// Here you can either reset the IQS5xx, and then go to the application, or
	// you can send the 0x02 command to jump to the application code.
	//
	//
	
	// Reset to exit bootloader and go into application
	//
	// IQS5xx_Reset();
	
	*/
	//
	// Give command to enter application code, thus exiting bootloader
	//
	I2C_Write_8(BL_ADDR, BL_CMD_EXIT, &ui8DataBuffer[0], 1);
	//
	// Delay to allow IQS5xx to power up and initialise
	//
	os_delay_us(3000);	
}

//*****************************************************************************
//
//! Read and display version
//!
//! This function reads the version details from the IQS5xx and sends this to 
//! the display port.
//!
//! \param None
//!
//! \return None
//
//*****************************************************************************
void ICACHE_FLASH_ATTR IQS5xx_CheckVersion(void) 
{
	uint8 ui8DataBuffer[6];
	//
	// Dont wait for RDY here, since the device could be in EventMode, and then
	// there will be no communication window to complete this.  Rather do a 
	// forced communication, where clock stretching will be done on the IQS5xx
	// until an appropriate time to complete the i2c.
	//
	I2C_Read_16(IQS5xx_ADDR, ProductNumber_adr, &ui8DataBuffer[0] ,6);
  
	os_printf("Product "); 
	os_printf((ui8DataBuffer[0]<<8) + ui8DataBuffer[1]);
	os_printf("  Project ");
	os_printf((ui8DataBuffer[2]<<8) + ui8DataBuffer[3]);
	os_printf("  Version "); 
	os_printf(ui8DataBuffer[4]); os_printf("."); 
	os_printf(ui8DataBuffer[5]);
}


//*****************************************************************************
//
//! Print a signed value on serial display
//!                            
//! Print the signed integer on the serial port with adjusted tabs 
//! on serial port for easy column reading. 
//!			                         
//! \param None
//!                                           
//! \return None
//                                                      
//*****************************************************************************
void ICACHE_FLASH_ATTR Print_signed(int i16Num)
{
	if(i16Num < (-99))
	{
		os_printf(" ");
	}
	else if(i16Num < (-9))
	{
		os_printf("  ");
	}
	else if(i16Num < 0)
	{
		os_printf("   ");
	}
	else if(i16Num < 10)
	{
		os_printf("    ");
	}
	else if(i16Num < 100)
	{
		os_printf("   ");
	}
	else if(i16Num < 1000)
	{
		os_printf("  ");
	}
	os_printf("%i", i16Num);
}

//*****************************************************************************
//
//! Print an unsigned value on serial display
//!                            
//! Print the unsigned integer on the serial port with adjusted tabs 
//! on serial port for easy column reading. 
//!			                         
//! \param None
//!                                           
//! \return None
//                                                      
//*****************************************************************************
void ICACHE_FLASH_ATTR Print_unsigned(uint16 ui16Num)
{
	if(ui16Num < 10)
	{
		os_printf("    ");
	}
	else if(ui16Num < 100)
	{
		os_printf("   ");
	}
	else if(ui16Num < 1000)
	{
		os_printf("  ");
	}
	else if(ui16Num < 10000)
	{
		os_printf(" ");
	}

	if(ui16Num > 10000)
	{
		os_printf("  xxx");
	}
	else
	{
		os_printf("%u", ui16Num);
	}
}


