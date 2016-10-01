/*
 * Controller_Arduino.cpp
 *
 * Created: 7/31/2016 8:42:16 PM
 *  Author: Andrew_2
 */


#include "Arduino.h"
#include "Wire.h"
#include <avr/io.h>
#include "SoftwareSerial.h"
#include "Controller.h"
#include "MessageReader.h"
#include "MessageWriter.h"
#include "SoftwareWire.h"

void setup();
void loop();

SoftwareSerial serial(0,1);
SoftwareWire wire1(0, 0);
SoftwareWire wire2(0, 0);

int main(void) {
  init();

#ifdef USBCON
  USBDevice.attach();
#endif

  setup();
  while(1) {
    loop();
    if( serialEventRun ) serialEventRun();
  }
}

void setup() {
  serial.begin(9600);
  setupController(&wire1, &wire2);
  setupWriter();
  setupReader();
}

void loop() {
  updateController();
  readData();
  writeData();
  delay(100);
}
