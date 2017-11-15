
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "gpio.h"
#include "user_interface.h" 
#include "espconn.h"
 
#include "proxsense.h"

LOCAL os_timer_t test_timer;
uint16 proxData[3];

void ICACHE_FLASH_ATTR printProxsense(void) {
	updateProxsense();
	
	getProxsenseData(0, proxData);
	os_printf("DEV 0\n");
	os_printf("X: %u\n", proxData[0]);
	os_printf("Y: %u\n", proxData[1]);
	os_printf("S: %u\n\n", proxData[2]);
	
	getProxsenseData(1, proxData);
	os_printf("DEV 1\n");
	os_printf("X: %u\n", proxData[0]);
	os_printf("Y: %u\n", proxData[1]);
	os_printf("S: %u\n\n", proxData[2]);
}


void user_init(void) {
  // init gpio subsytem
  gpio_init();
  
  uart_div_modify(0, UART_CLK_FREQ / 115200);
  
  os_printf("SDK version:%s\n", system_get_sdk_version());

  wifi_set_opmode(STATIONAP_MODE);
  
  initProxsense();
  
  os_timer_disarm(&test_timer);
  os_timer_setfn(&test_timer, (os_timer_func_t *)printProxsense, NULL);
  os_timer_arm(&test_timer, 500, 1);

}
