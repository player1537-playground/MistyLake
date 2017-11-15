#include "i2c_util.h"
#include "osapi.h"
#include "i2c_master.h"


uint8 ICACHE_FLASH_ATTR i2c_master_writeBytes(uint8 address, const uint8 *values, uint8 length) {
	i2c_master_start();

	i2c_master_writeByte(address);
	if (!i2c_master_checkAck())
	{
		i2c_master_stop();
		os_printf("Device not ACKed on address\n" );
		return false;
	}

	uint8 i;
	for(i = 0; i < length; i++){

		i2c_master_writeByte(values[i]);
		if (!i2c_master_checkAck())
		{
			os_printf( "Device not ACKed on write\n" );
			i2c_master_stop();
			return false;
		}
	}

	i2c_master_stop();
	return true;
}

uint8 ICACHE_FLASH_ATTR i2c_master_readBytes(uint8 address, uint8 *values, uint8 length) {
	
	uint8 timeout = 100;
	do{
		i2c_master_start();
		i2c_master_writeByte(address | 0x1);
		if(!i2c_master_checkAck()) {
			i2c_master_stop();
			i2c_master_wait(1000);
			continue;
		}
		break;
	} while(--timeout>0);

	if(timeout == 0){
		os_printf("TIMEOUT\n");
		return false;
	}
	
	os_printf("Read: ");
	uint8 numRead = 0;
	while(numRead < length){
		uint8 byte = i2c_master_readByte();
		values[numRead++] = byte;
		if(numRead == length) {
			i2c_master_send_nack();
		} else {
			i2c_master_send_ack();
		}
		os_printf("%d ", byte);
	}
	
	i2c_master_stop();
	os_printf("\n");
	return true;
}
