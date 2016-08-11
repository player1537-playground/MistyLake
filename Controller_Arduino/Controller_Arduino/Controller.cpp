/*
 * Controller.cpp
 *
 * Created: 7/31/2016 9:59:13 PM
 *  Author: Andrew_2
 */ 

#include "Arduino.h"
#include "Controller.h"
#include "MessageWriter.h"
#include "Controller_Arduino.h"
#include "Wire.h"

#define T1_ADDRESS 0b1110100 //116

const unsigned char EVENT_ADDRESS_1 = 0x05;
const unsigned char EVENT_ADDRESS_2 = 0x8F;
const unsigned char EVENT_DATA = 0x00;

const unsigned char DATA_ADDRESS_1 = 0x00;
const unsigned char DATA_ADDRESS_2 = 0x16;

const unsigned char END_ADDRESS_1 = 0xEE;
const unsigned char END_ADDRESS_2 = 0xEE;
const unsigned char END_DATA = 0xFF;


const unsigned char numButtons = 3;
const unsigned char buttonPins[] = {4,5,6};
const int bytesToRead = 6;

Trackpad t1,t2;
unsigned short buttons;

char t1Buffer [6];

void readTrackpads();
void readButtons();

Trackpad getTrack1() {
	return t1;
}
Trackpad getTrack2() {
	return t2;
}
unsigned short getButtons() {
	return buttons;
}

void setupController() {
	
	for(int i = 0; i < numButtons; i++) {
		pinMode(buttonPins[i], INPUT);	
	}
	buttons = 0;
	
	t1.x = t1.y = t1.strength = 0;
	t2.x = t2.y = t2.strength = 0;
	
	
	Wire.begin();
	
	//Disable events on trackpad
	/*Wire.beginTransmission(T1_ADDRESS);
	Wire.write(EVENT_ADDRESS_1);
	Wire.write(EVENT_ADDRESS_2);
	Wire.write(EVENT_DATA);
	Wire.endTransmission();*/
	
	
}

void updateController() {
	readTrackpads();
	readButtons();
	
}

//read data from trackpads via i2c
void readTrackpads() {

	//select appropriate address in device memory map
	Wire.beginTransmission(T1_ADDRESS);
	Wire.write(DATA_ADDRESS_1);
	Wire.write(DATA_ADDRESS_2);
	unsigned short tmp = (unsigned short)Wire.endTransmission();
	
	unsigned char bytesRead = Wire.requestFrom(T1_ADDRESS, bytesToRead);
	
	
	if(bytesRead == bytesToRead) {
		Wire.readBytes(t1Buffer, bytesToRead);
		
		t1.x = (t1Buffer[0] << 8) | (t1Buffer[1]);
		t1.y = (t1Buffer[2] << 8) | (t1Buffer[3]);
		t1.strength = (t1Buffer[4] << 8) | (t1Buffer[5]);
		
	}
	
	
	//End i2c communication with sensor
	//This is necessary to allow the chip to process new inputs
	Wire.beginTransmission(T1_ADDRESS);
	Wire.write(END_ADDRESS_1);
	Wire.write(END_ADDRESS_2);
	Wire.write(END_DATA);
	Wire.endTransmission();
	
	
}

void readButtons() {
	buttons = 0;
	for(int i = 0; i < numButtons; i++) {
		if(digitalRead(buttonPins[i]) == 1) {
			buttons |=  1 << (15 - i);
		}
	}
}

void requestWriteSpec() {
	requestWriteSpecWriter();
}