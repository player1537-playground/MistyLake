
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "gpio.h"
#include "user_interface.h"
#include "espconn.h"

#include "proxsense.h"

LOCAL os_timer_t update_timer;
LOCAL struct espconn udp_espconn;
LOCAL uint8 sendInitialized = false;
LOCAL uint16 localPort = 1234;
uint16 remotePort = 1234;
uint8 remote_ip[4] = {192, 168, 4, 2};

const uint8 ADDRESS_RIGHT = 0b11101000;//0x74 << 1;
const uint8 ADDRESS_LEFT = 0b11101010;//0x75 << 1;
 
#define NUM_TOUCH_PADS 2
LOCAL struct proxsense touchpads[2];

void ICACHE_FLASH_ATTR sendUDP(void) {
	uint8 dataBuf[256];
	os_printf("Sending data\n");
	/*
	os_sprintf(dataBuf, "Controller Data\n Left: X: %u, Y: %u, S: %u\n Right: X: %u, Y: %u, S: %u\n", 
		touchpads[0].x, touchpads[0].y, touchpads[0].strength,
		touchpads[1].x, touchpads[1].y, touchpads[1].strength);
	*/
	
	dataBuf[0] = 2;
	dataBuf[1] = (touchpads[0].active);
	dataBuf[2] = (touchpads[0].x) >> 8;
	dataBuf[3] = (touchpads[0].x);
	dataBuf[4] = (touchpads[0].y) >> 8;
	dataBuf[5] = (touchpads[0].y);
	dataBuf[6] = (touchpads[0].strength) >> 8;
	dataBuf[7] = (touchpads[0].strength);
	dataBuf[8] = (touchpads[1].active);
	dataBuf[9] = (touchpads[1].x) >> 8;
	dataBuf[10] = (touchpads[1].x);
	dataBuf[11] = (touchpads[1].y) >> 8;
	dataBuf[12] = (touchpads[1].y);
	dataBuf[13] = (touchpads[1].strength) >> 8;
	dataBuf[14] = (touchpads[1].strength);
	uint8 i;
	for(i = 15; i < 32; i++) {
		dataBuf[i] = 0;
	}
	
	
	os_memcpy(udp_espconn.proto.udp->remote_ip, remote_ip, 4);
	udp_espconn.proto.udp->remote_port = remotePort;
	//espconn_sendto(&udp_espconn, dataBuf, os_strlen(dataBuf));
	espconn_sendto(&udp_espconn, dataBuf, 32);
}
 
void ICACHE_FLASH_ATTR updateController(void) {
	
	int i;
	for(i = 0; i < NUM_TOUCH_PADS; i++) {
		if(updateProxsenseDevice(&touchpads[i])) {
			printProxsenseData(&touchpads[i]);
		} else {
			os_printf("Error reading proxsense.\n");
		}
	}
	
	if(sendInitialized) {
		sendUDP();
	}
}
 
LOCAL void ICACHE_FLASH_ATTR
 user_udp_recv_cb(void *arg, char *pusrdata, unsigned short length)
 {
    os_printf("Received udp data: %s\n", pusrdata);

	/*
	int i = 0;
	for(; i < length; i++) {
		//uart_tx_one_char(pusrdata[i]);
	}*/

    struct espconn *pesp_conn = arg;
	remot_info *premot = NULL;
    if (espconn_get_connection_info(pesp_conn,&premot,0) == ESPCONN_OK){
		remotePort = premot->remote_port;
		os_memcpy(remote_ip, premot->remote_ip, 4);
		os_printf("Port: %u\n", remotePort);
		os_printf("IP: %u.%u.%u.%u\n", remote_ip[0], remote_ip[1], remote_ip[2], remote_ip[3]);
		sendInitialized = true;
	}
	 
 }

 
LOCAL void ICACHE_FLASH_ATTR udp_init(void) {
	
	udp_espconn.type = ESPCONN_UDP;
    udp_espconn.proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
	udp_espconn.proto.udp->local_port = localPort;
	udp_espconn.proto.udp->remote_port = remotePort;
    os_memcpy(udp_espconn.proto.udp->remote_ip, remote_ip, 4);
    espconn_regist_recvcb(&udp_espconn, user_udp_recv_cb); // register a udp packet receiving callback
    espconn_create(&udp_espconn);   // create udp
}

void user_init(void)
{
 
  // init gpio subsytem
  gpio_init();
  
  uart_div_modify(0, UART_CLK_FREQ / 115200);
  
  os_printf("SDK version:%s\n", system_get_sdk_version());

 
   //Set softAP + station mode 
   wifi_set_opmode(STATIONAP_MODE); 
 
   // Create udp listening.
   udp_init();
   
   touchpads[0].address = ADDRESS_LEFT;
   touchpads[1].address = ADDRESS_RIGHT;
  initProxsense();
  
  os_timer_disarm(&update_timer);
  os_timer_setfn(&update_timer, (os_timer_func_t *)updateController, NULL);
  os_timer_arm(&update_timer, 100, 1);
}