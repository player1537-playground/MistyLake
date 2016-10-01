/*
 * MessageWriter.cpp
 *
 * Created: 7/31/2016 9:55:48 PM
 *  Author: Andrew_2
 */

#define MESSAGE_BUFFER_SIZE 254
#define WRITE_BUFFER_SIZE 256
#define  SPEC_ID 1
#define  DATA_ID 2

#define SHORT_TO_BYTES(shortVal, byteArray, index) \
  do { \
    byteArray[index] = (shortVal >> 8) & 0xFF; \
    byteArray[index + 1] = shortVal & 0xFF; \
  } while(0)

#include "MessageWriter.h"
#include "Controller.h"
#include "Controller_Arduino.h"

unsigned char outMessageBuffer [MESSAGE_BUFFER_SIZE];
unsigned char writeBuffer [WRITE_BUFFER_SIZE];

int request;

void stuffData(const unsigned char *ptr, unsigned char length, unsigned char *dst);

void setupWriter() {
  request = 0;
}

void writeData() {

  for(int i = 0; i < 16; i++) {
    outMessageBuffer[i] = 0;
  }

  unsigned char len = 0;

  if(request == 1) {

    len = 16;
    outMessageBuffer[0] = len;
    outMessageBuffer[1] = SPEC_ID;

    outMessageBuffer[5] = HARDWARE_VERSION;
    outMessageBuffer[9] = SOFTWARE_VERSION;
    outMessageBuffer[13] = NUM_BUTTONS;

    request = 0;


  } else {
    len = 16;
    outMessageBuffer[0] = len;
    outMessageBuffer[1] = DATA_ID;

    Trackpad t1 = getTrack1();
    SHORT_TO_BYTES(t1.x, outMessageBuffer, 2);
    SHORT_TO_BYTES(t1.y, outMessageBuffer, 4);
    SHORT_TO_BYTES(t1.strength, outMessageBuffer, 6);

    Trackpad t2 = getTrack1();
    SHORT_TO_BYTES(t2.x, outMessageBuffer, 8);
    SHORT_TO_BYTES(t2.y, outMessageBuffer, 10);
    SHORT_TO_BYTES(t2.strength, outMessageBuffer, 12);

    SHORT_TO_BYTES(getButtons(), outMessageBuffer, 14);

  }

  stuffData(outMessageBuffer, len, writeBuffer);

  for(int i = 0; i < len + 2; i++) {
    serial.write(writeBuffer[i]);
  }

}

#define FinishBlock(X) (*code_ptr = (X), code_ptr = dst++, code = 0x01)

void stuffData(const unsigned char *ptr, unsigned char length, unsigned char *dst) {
  const unsigned char *end = ptr + length;
  unsigned char *code_ptr = dst++;
  unsigned char code = 0x01;

  while (ptr < end) {
    if (*ptr == 0) {
      FinishBlock(code);
    } else {
      *dst++ = *ptr;
      code++;
    }
    ptr++;
  }

  FinishBlock(code);
  *dst++ = 0;
}


void requestWriteSpecWriter() {
  request = 1;
}
