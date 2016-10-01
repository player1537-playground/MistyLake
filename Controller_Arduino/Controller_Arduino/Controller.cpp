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
#include "SoftwareWire.h"

#define ADDRESS 0b1110100 //116

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

Trackpad trackpads[2];
SoftwareWire *wires[2];
unsigned short buttons;

void readTrackpads();
void readButtons();

Trackpad getTrack1() {
  return trackpads[0];
}

Trackpad getTrack2() {
  return trackpads[1];
}

unsigned short getButtons() {
  return buttons;
}

void setupController(SoftwareWire *w1, SoftwareWire *w2) {
  int i;

  for(i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT);
  }
  buttons = 0;

  for (i=0; i<2; ++i) {
    trackpads[i].x = 0;
    trackpads[i].y = 0;
    trackpads[i].strength = 0;
  }

  wires[0] = w1;
  wires[1] = w2;

  //Disable events on trackpad
  /*Wire.beginTransmission(ADDRESS);
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
  int i;
  char buffer[6];

  //select appropriate address in device memory map
  for (i=0; i<2; ++i) {
    wires[i].beginTransmission(ADDRESS);
    wires[i].write(DATA_ADDRESS_1);
    wires[i].write(DATA_ADDRESS_2);
    unsigned short tmp = (unsigned short)wires[i].endTransmission();

    unsigned char bytesRead = wires[i].requestFrom(ADDRESS, bytesToRead);

    if(bytesRead == bytesToRead) {
      wires[i].readBytes(buffer, bytesToRead);

      trackpads[i].x = (buffer[0] << 8) | (buffer[1]);
      trackpads[i].y = (buffer[2] << 8) | (buffer[3]);
      trackpads[i].strength = (buffer[4] << 8) | (buffer[5]);
    }

    //End i2c communication with sensor
    //This is necessary to allow the chip to process new inputs
    wires[i].beginTransmission(ADDRESS);
    wires[i].write(END_ADDRESS_1);
    wires[i].write(END_ADDRESS_2);
    wires[i].write(END_DATA);
    wires[i].endTransmission();
  }
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
