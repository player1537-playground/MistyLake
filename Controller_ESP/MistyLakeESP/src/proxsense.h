#ifndef PROXSENSE_H_
#define PROXSENSE_H_

#include "os_type.h"

struct proxsense {
	uint8 address;
	uint8 active;
	uint16 x, y, strength;
};


void initProxsense(void);
uint8 updateProxsenseDevice(struct proxsense *device);
void printProxsenseData(struct proxsense *device);

#endif

