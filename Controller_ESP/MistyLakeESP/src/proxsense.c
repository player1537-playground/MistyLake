/*
 * Controller.cpp
 *
 * Created: 7/31/2016 9:59:13 PM
 *  Author: Andrew_2
 */
#include "proxsense.h"
#include "i2c_util.h"
#include "osapi.h"


const uint8 EVENT_MESSAGE[] = {0x05, 0x8F, 0x00};

const uint8 DATA_ADDRESS[] = {0x00, 0x16};

const uint8 END_MESSAGE[] = {0xEE, 0xEE, 0xFF};

const uint8 BYTES_TO_READ = 6;


void ICACHE_FLASH_ATTR initProxsense(void) {
	i2c_master_gpio_init();
}

uint8 ICACHE_FLASH_ATTR updateProxsenseDevice(struct proxsense *device) {
  uint8 buf[6];

  uint8 ack = i2c_master_writeBytes(device->address, DATA_ADDRESS, 2);
  if(!ack) {
	  os_printf("No response on write.\n");
	  return false;
  }
  ack = i2c_master_readBytes(device->address, buf, BYTES_TO_READ);
  
  device->x = (buf[0] << 8) | (buf[1]);
  device->y = (buf[2] << 8) | (buf[3]);
  device->strength = (buf[4] << 8) | (buf[5]);
  
  device->active = (buf[0] != 0xFF && buf[1] != 0xFF);
  
  ack = i2c_master_writeBytes(device->address, END_MESSAGE, 3);
  if(!ack) {
	  os_printf("No response on end.\n");
	  return false;
  }
  return true;
}

void ICACHE_FLASH_ATTR printProxsenseData(struct proxsense *device) {
	os_printf("Proxsense Dev: %u\n", device->address);
	os_printf("X: %u, Y: %u, S: %u\n", device->x, device->y, device->strength);
}
