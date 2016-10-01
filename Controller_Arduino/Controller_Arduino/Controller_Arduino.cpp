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

void setup();
void loop();

SoftwareSerial serial(0,1);

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
  setupController();
  setupWriter();
  setupReader();
}

void loop() {
  updateController();
  readData();
  writeData();
  delay(100);
}
