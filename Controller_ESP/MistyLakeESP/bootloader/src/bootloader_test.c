#include "IQS5xx.h"
#include "defs.h"
#include "I2C.h"

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "gpio.h"
#include "user_interface.h" 
#include "espconn.h"


LOCAL os_timer_t test_timer;
LOCAL uint16 blockNumCur, blockNumInc, blockNumMax;

void ICACHE_FLASH_ATTR user_read_memory(void) {
	if(blockNumCur < blockNumMax) {
		uint16 blockNumStop = blockNumCur + blockNumInc;
		if(blockNumStop > blockNumMax) {
			blockNumStop = blockNumMax;
		}
		for(; blockNumCur < blockNumStop; blockNumCur++) {
			PrintMemory((uint8)blockNumCur);
		}
	} else {
		os_timer_disarm(&test_timer);
	}
}

void ICACHE_FLASH_ATTR user_read_check(void) {
	PrintChecksum();
}

void user_init(void) {
  // init gpio subsytem
  gpio_init();
  
  uart_div_modify(0, UART_CLK_FREQ / 115200);
  
  os_printf("SDK version:%s\n", system_get_sdk_version());

  wifi_set_opmode(STATIONAP_MODE);
  
  //init NRST pin gpio12 for output
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
  PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTDI_U);
  gpio_output_set(0, 0, 0, BIT12);
  
  //init RDY pin gpio13 for input
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
  PIN_PULLUP_DIS(PERIPHS_IO_MUX_MTCK_U);
  gpio_output_set(0, 0, 0, BIT13);
  
  blockNumCur = 239;
  blockNumInc = 1;
  blockNumMax = 240;
  
  I2C_Setup();
  
  EnterBootloaderIQS5xx();
  
  /*os_timer_disarm(&test_timer);
  os_timer_setfn(&test_timer, (os_timer_func_t *)user_read_check, NULL);
  os_timer_arm(&test_timer, 1000, 1);*/
  
  os_printf("Initial Read\n");
  user_read_memory();
  os_printf("Write\n");
  ProgramLastBlock();
  os_printf("Followup Read\n");
  blockNumCur = 239;
  user_read_memory();
  
  
  //ProgramIQS5xx();
  
  //IQS5xx_AcknowledgeReset();
  
  //IQS5xx_CheckVersion();
  
  //Close_Comms();

}