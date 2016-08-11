/*
 * Controller.h
 *
 * Created: 7/31/2016 9:58:58 PM
 *  Author: Andrew_2
 */ 


#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#define HARDWARE_VERSION 1
#define SOFTWARE_VERSION 1
#define NUM_BUTTONS 3


struct Trackpad {
	unsigned short x, y, strength;
};

void setupController();
void updateController();

Trackpad getTrack1();
Trackpad getTrack2();

unsigned short getButtons();

void requestWriteSpec();


#endif /* CONTROLLER_H_ */