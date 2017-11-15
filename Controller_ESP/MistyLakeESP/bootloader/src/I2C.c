#include "I2C.h"
#include "IQS5xx.h"
#include "defs.h"
#include "os_type.h"
#include "osapi.h"
#include "gpio.h"

uint16 	ui16Timeout = 10000;  // Timeout on i2c in us
uint8 	ui8Success;
uint32 	ui32StartTime;
//
// Function prototypes
//
uint8 I2C_W16(uint8 ui8SlaveAdr, uint16 ui16RegisterAddress, uint8 *pData, uint8 ui8NoOfBytes);
uint8 I2C_R16(uint8 ui8SlaveAdr, uint16 ui16RegisterAddress, uint8 *pData, uint8 ui8NoOfBytes);
uint8 I2C_W8(uint8 ui8SlaveAdr, uint8 ui8RegisterAddress, uint8 *pData, uint8 ui8NoOfBytes);
uint8 I2C_R8(uint8 ui8SlaveAdr, uint8 ui8RegisterAddress, uint8 *pData, uint8 ui8NoOfBytes);
uint8 BL_BR_16(uint8 ui8SlaveAdr, uint16 ui16RegisterAddress, uint8 *pData, uint8 ui8NoOfBytes);

//#############################################################################
//! Public functions
//#############################################################################


//*****************************************************************************
//
//! Setup I2C
//!
//! Enable the I2c module on the arduino board (including the pull-ups, 
//! enabling of the ACK, and setting the clock frequency)
//!
//! \param None
//!
//! \return None
//
//*****************************************************************************
void ICACHE_FLASH_ATTR I2C_Setup(void)
{
	i2c_master_gpio_init();
}
 
//*****************************************************************************
//
//! I2C write function (16 bit data address)
//!
//! This function writes the provided data to the 16bit address specified.  If  
//! anything in the write process is not successful, then it will be repeated
//! up till four more times.  If still not successful, it will write an error
//! message on the serial terminal.
//!
//! \param ui8SlaveAdr is the 7-bit device slave address
//! \param ui16RegisterAddress is the 16-bit memory map address on the IQS5xx
//! \param pData is a pointer to the first byte of a block of data to write
//! \param ui8NoOfBytes indicates how many bytes of data to write
//!
//! \return Boolean indicating success/fail of write attempt
//
//*****************************************************************************
uint8 ICACHE_FLASH_ATTR I2C_Write_16(uint8 ui8SlaveAdr, uint16 ui16RegisterAddress, uint8 *pData, uint8 ui8NoOfBytes)
{
	uint8 ui8Retry = 4;
	
	ui8Success = I2C_W16(ui8SlaveAdr, ui16RegisterAddress, pData, ui8NoOfBytes);
	//
	// If comms was not successful, retry 4 more times
	//
	while((!ui8Success) && (ui8Retry != 0))
	{
		ui8Success = I2C_W16(ui8SlaveAdr, ui16RegisterAddress, pData, ui8NoOfBytes);
		ui8Retry--;
	}
	
	if(ui8Success)
	{
		return true;
	}
	else
	{
		os_printf("Comms write error\n");
		return false;
	}
}

//*****************************************************************************
//
//! I2C write function (8 bit data address)
//!
//! This function writes the provided data to the 8bit address specified.  If  
//! anything in the write process is not successful, then it will be repeated
//! up till four more times.  If still not successful, it will write an error
//! message on the serial terminal.
//!
//! \param ui8SlaveAdr is the 7-bit device slave address
//! \param ui8RegisterAddress is the 8-bit address
//! \param pData is a pointer to the first byte of a block of data to write
//! \param ui8NoOfBytes indicates how many bytes of data to write
//!
//! \return Boolean indicating success/fail of write attempt
//
//*****************************************************************************
uint8 ICACHE_FLASH_ATTR I2C_Write_8(uint8 ui8SlaveAdr, uint8 ui8RegisterAddress, uint8 *pData, uint8 ui8NoOfBytes)
{
	uint8 ui8Retry = 4;
	
	ui8Success = I2C_W8(ui8SlaveAdr, ui8RegisterAddress, pData, ui8NoOfBytes);
	//
	// If comms was not successful, retry 4 more times
	//
	while((!ui8Success) && (ui8Retry != 0))
	{
		ui8Success = I2C_W8(ui8SlaveAdr, ui8RegisterAddress, pData, ui8NoOfBytes);
		ui8Retry--;
	}
	
	if(ui8Success)
	{
		return true;
	}
	else
	{
		os_printf("Comms write error\n");
		return false;
	}
}

//*****************************************************************************
//
//! I2C read function (16 bit data address)
//!
//! This function reads data from the 16bit address specified and stores this data
//! in the area provided by the pointer.  If anything in the read process is 
//! not successful, then it will be repeated up till four more times.  
//! If still not successful, it will write an error message on the serial 
//! terminal.
//!
//! \param ui8SlaveAdr is the 7-bit device slave address
//! \param ui16RegisterAddress is the 16-bit memory map address on the IQS5xx
//! \param pData is a pointer to where the read data must be stored
//! \param ui8NoOfBytes indicates how many bytes of data to read
//!
//! \return Boolean indicating success/fail of read attempt
//
//*****************************************************************************
uint8 ICACHE_FLASH_ATTR I2C_Read_16(uint8 ui8SlaveAdr, uint16 ui16RegisterAddress, uint8 *pData, uint8 ui8NoOfBytes)
{
	uint8 ui8Retry = 4;
	
	ui8Success = I2C_R16(ui8SlaveAdr, ui16RegisterAddress, pData, ui8NoOfBytes);
	//
	// If comms was not successful, retry 4 more times
	//
	while((!ui8Success) && (ui8Retry != 0))
	{
		ui8Success = I2C_R16(ui8SlaveAdr, ui16RegisterAddress, pData, ui8NoOfBytes);
		ui8Retry--;
	}
	
	if(ui8Success)
	{
		return true;
	}
	else
	{
		os_printf("Comms read error\n");
		return false;
	}
}

//*****************************************************************************
//
//! I2C read function (8 bit data address)
//!
//! This function reads data from the 8bit address specified and stores this data
//! in the area provided by the pointer.  If anything in the read process is 
//! not successful, then it will be repeated up till four more times.  
//! If still not successful, it will write an error message on the serial 
//! terminal.
//!
//! \param ui8SlaveAdr is the 7-bit device slave address
//! \param ui8RegisterAddress is the 8-bit address
//! \param pData is a pointer to where the read data must be stored
//! \param ui8NoOfBytes indicates how many bytes of data to read
//!
//! \return Boolean indicating success/fail of read attempt
//
//*****************************************************************************
uint8 ICACHE_FLASH_ATTR I2C_Read_8(uint8 ui8SlaveAdr, uint8 ui8RegisterAddress, uint8 *pData, uint8 ui8NoOfBytes)
{
	uint8 ui8Retry = 20;
	
	ui8Success = I2C_R8(ui8SlaveAdr, ui8RegisterAddress, pData, ui8NoOfBytes);
	//
	// If comms was not successful, retry 4 more times
	//
	while((!ui8Success) && (ui8Retry != 0))
	{
		ui8Success = I2C_R8(ui8SlaveAdr, ui8RegisterAddress, pData, ui8NoOfBytes);
		ui8Retry--;
	}
	
	if(ui8Success)
	{
		return true;
	}
	else
	{
		os_printf("Comms read error\n");
		return false;
	}
}

//*****************************************************************************
//
//! Bootloader I2C block read function
//!
//! This function reads performs a block read (normally 64 bytes) using the 
//! required bootloader protocol.  This means that the command 0x01 must first
//! be sent before the 16bit address, and then a read must be performed.  This  
//! is not standard i2c sequence, so a different function was written for this.
//!
//! \param ui8SlaveAdr is the 7-bit device slave address
//! \param ui16RegisterAddress is the 16-bit address
//! \param pData is a pointer to where the read data must be stored
//! \param ui8NoOfBytes indicates how many bytes of data to read
//!
//! \return Boolean indicating success/fail of read attempt
//
//*****************************************************************************
uint8 ICACHE_FLASH_ATTR BL_Block_Read_16(uint8 ui8SlaveAdr, uint16 ui16RegisterAddress, uint8 *pData, uint8 ui8NoOfBytes)
{
	uint8 ui8Retry = 4;
	
	ui8Success = BL_BR_16(ui8SlaveAdr, ui16RegisterAddress, pData, ui8NoOfBytes);
	//
	// If comms was not successful, retry 4 more times
	//
	while((!ui8Success) && (ui8Retry != 0))
	{
		ui8Success = I2C_R16(ui8SlaveAdr, ui16RegisterAddress, pData, ui8NoOfBytes);
		ui8Retry--;
	}
	
	if(ui8Success)
	{
		return true;
	}
	else
	{
		os_printf("Comms read error\n");
		return false;
	}
}

//*****************************************************************************
//
//! Wait for a communication window
//!                            
//! This function waits for the RDY window to be available. RDY I/O indicates 
//! when the communication window is available.  
//!			                         
//! \param None
//!                                             
//! \return None
//                                                      
//*****************************************************************************
void ICACHE_FLASH_ATTR RDY_wait() 
{
	while(!GPIO_INPUT_GET(RDY_PIN))
	{
		//__asm__("nop\n\t\n");
	}
}

//*****************************************************************************
//
//! Terminate communication window
//!                            
//! The IQS5xx B000 does not close the communication window on the reception of 
//!	the STOP bit; therefore this function sends the END COMMUNICATION WINDOW 
//!	COMMAND (Please see datasheet for more information). RDY will go low after 
//!	receiving any write to 0xEEEE followed by a STOP. 
//!			                         
//! \param None
//!                                           
//! \return None
//                                                      
//*****************************************************************************
void ICACHE_FLASH_ATTR Close_Comms() 
{
  uint8 ui8DataBuffer[1];
  
  I2C_Write_16(IQS5xx_ADDR, END_WINDOW, &ui8DataBuffer[0], 1);
}


//#############################################################################
//! Private Functions
//#############################################################################

void ICACHE_FLASH_ATTR ResetTimeout(void)
{
	// Mark the current time, to start the i2c timeout counter from
	//
	ui32StartTime = system_get_time();
}

uint8 ICACHE_FLASH_ATTR I2CTimeout(void)
{
	return((system_get_time() - ui32StartTime) >= ui16Timeout);
}

uint8 ICACHE_FLASH_ATTR I2C_start(void)
{
	i2c_master_start();
	return true;
}

void ICACHE_FLASH_ATTR I2C_Reset(void)
{
	os_printf("Reset Error\n");
	i2c_master_stop();
}

uint8 ICACHE_FLASH_ATTR I2C_stop(void)
{
	i2c_master_stop();
	return true;
}

uint8 ICACHE_FLASH_ATTR I2C_sendByte(uint8 ui8Data)
{
	ResetTimeout();
	do
	{
		if(I2CTimeout())
		{
			I2C_Reset();
			return false;
		}
		i2c_master_writeByte(ui8Data);
	} while(!i2c_master_checkAck());
	
	return true;
}
uint8 ICACHE_FLASH_ATTR I2C_sendAddress(uint8 ui8Address)
{
	return I2C_sendByte(ui8Address);
}

uint8 ICACHE_FLASH_ATTR I2C_receiveByte(uint8 ack, uint8 *dest)
{

	*dest = i2c_master_readByte();
	if(!ack) {
		i2c_master_send_nack();
	} else {
		i2c_master_send_ack();
	}
	return true;
}

uint8 ICACHE_FLASH_ATTR I2C_W16(uint8 ui8SlaveAdr, uint16 ui16RegisterAddress, uint8 *pData, uint8 ui8NoOfBytes)
{
	uint8	i;
	
	if(I2C_start() == false)
	{
		return false;
	}

	if(I2C_sendAddress(ui8SlaveAdr<<1) == false)
	{
		return false;
	}
	
	if(I2C_sendByte((uint8)(ui16RegisterAddress>>8)) == false)
	{
		return false;
	}
	
	if(I2C_sendByte((uint8)ui16RegisterAddress) == false)
	{
		return false;
	}
	
	for(i = 0; i < ui8NoOfBytes; i++)
	{
		if(I2C_sendByte(*pData) == false)
		{
			return false;
		}
		pData++;
	}

	if(I2C_stop() == false)
	{
		return false;
	}
	return true;
}

uint8 ICACHE_FLASH_ATTR I2C_R16(uint8 ui8SlaveAdr, uint16 ui16RegisterAddress, uint8 *pData, uint8 ui8NoOfBytes)
{
	uint8	i;
		
	if(ui8NoOfBytes == 0)
	{
		ui8NoOfBytes++;
	}

	if(I2C_start() == false)
	{
		return false;
	}

	if(I2C_sendAddress(ui8SlaveAdr<<1) == false)
	{
		return false;
	}

	if(I2C_sendByte((uint8)(ui16RegisterAddress>>8)) == false)
	{
		return false;
	}

	if(I2C_sendByte((uint8)ui16RegisterAddress) == false)
	{
		return false;
	}

	if(I2C_start() == false)
	{
		return false;
	}

	if(I2C_sendAddress((ui8SlaveAdr<<1) + 0x01) == false)
	{
		return false;
	}
	
	for(i = 0; i < ui8NoOfBytes; i++)
	{
		uint8 dat = 0;
		if(i == (ui8NoOfBytes - 1))
		{
			if(I2C_receiveByte(0, &dat) == false)
			{
				return false;
			}
		}
		else
		{
			if(I2C_receiveByte(1, &dat) == false)
			{
				return false;
			}
		}
		*pData = dat;
		pData++;
	}
	if(I2C_stop() == false)
	{
		return false;
	}
	return true;
}

uint8 ICACHE_FLASH_ATTR I2C_R8(uint8 ui8SlaveAdr, uint8 ui8RegisterAddress, uint8 *pData, uint8 ui8NoOfBytes)
{
	uint8	i;
		
	if(ui8NoOfBytes == 0)
	{
		ui8NoOfBytes++;
	}

	if(I2C_start() == false)
	{
		return false;
	}

	if(I2C_sendAddress(ui8SlaveAdr<<1) == false)
	{
		return false;
	}

	if(I2C_sendByte(ui8RegisterAddress) == false)
	{
		return false;
	}

	if(I2C_start() == false)
	{
		return false;
	}

	if(I2C_sendAddress((ui8SlaveAdr<<1) + 0x01) == false)
	{
		return false;
	}
	
	for(i = 0; i < ui8NoOfBytes; i++)
	{
		uint8 dat = 0;
		if(i == (ui8NoOfBytes - 1))
		{
			if(I2C_receiveByte(0, &dat) == false)
			{
				return false;
			}
		}
		else
		{
			if(I2C_receiveByte(1, &dat) == false)
			{
				return false;
			}
		}
		*pData = dat;
		pData++;
	}
	if(I2C_stop() == false)
	{
		return false;
	}
	return true;
}

uint8 ICACHE_FLASH_ATTR I2C_W8(uint8 ui8SlaveAdr, uint8 ui8RegisterAddress, uint8 *pData, uint8 ui8NoOfBytes)
{
	uint8	i;
	
	if(I2C_start() == false)
	{
		return false;
	}

	if(I2C_sendAddress(ui8SlaveAdr<<1) == false)
	{
		return false;
	}
	
	if(I2C_sendByte(ui8RegisterAddress) == false)
	{
		return false;
	}
	
	for(i = 0; i < ui8NoOfBytes; i++)
	{
		if(I2C_sendByte(*pData) == false)
		{
			return false;
		}
		pData++;
	}

	if(I2C_stop() == false)
	{
		return false;
	}
	return true;
}

uint8 ICACHE_FLASH_ATTR BL_BR_16(uint8 ui8SlaveAdr, uint16 ui16RegisterAddress, uint8 *pData, uint8 ui8NoOfBytes)
{
	uint8	i;
		
	if(ui8NoOfBytes == 0)
	{
		ui8NoOfBytes++;
	}

	if(I2C_start() == false)
	{
		return false;
	}

	if(I2C_sendAddress(ui8SlaveAdr<<1) == false)
	{
		return false;
	}
	//
	// Send the block read command
	//
	if(I2C_sendByte(BL_CMD_READ) == false)
	{
		return false;
	}

	if(I2C_sendByte((uint8)(ui16RegisterAddress>>8)) == false)
	{
		return false;
	}

	if(I2C_sendByte((uint8)ui16RegisterAddress) == false)
	{
		return false;
	}

	if(I2C_start() == false)
	{
		return false;
	}

	if(I2C_sendAddress((ui8SlaveAdr<<1) + 0x01) == false)
	{
		return false;
	}
	
	for(i = 0; i < ui8NoOfBytes; i++)
	{
		uint8 dat = 0;
		if(i == (ui8NoOfBytes - 1))
		{
			if(I2C_receiveByte(0, &dat) == false)
			{
				return false;
			}
		}
		else
		{
			if(I2C_receiveByte(1, &dat) == false)
			{
				return false;
			}
		}
		*pData = dat;
		pData++;
	}
	if(I2C_stop() == false)
	{
		return false;
	}
	return true;	
}
