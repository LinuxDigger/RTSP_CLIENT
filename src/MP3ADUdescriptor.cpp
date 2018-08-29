/*
 * MP3ADUdescriptor.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */



#include "MP3ADUdescriptor.h"

////////// ADUdescriptor //////////

//##### NOTE: For now, ignore fragmentation.  Fix this later! #####

#define TWO_BYTE_DESCR_FLAG 0x40

unsigned ADUdescriptor::generateDescriptor(unsigned char*& toPtr,
					   unsigned remainingFrameSize) {
  unsigned descriptorSize = ADUdescriptor::computeSize(remainingFrameSize);
  switch (descriptorSize) {
  case 1: {
    *toPtr++ = (unsigned char)remainingFrameSize;
    break;
  }
  case 2: {
    generateTwoByteDescriptor(toPtr, remainingFrameSize);
    break;
  }
  }

  return descriptorSize;
}

void ADUdescriptor::generateTwoByteDescriptor(unsigned char*& toPtr,
					      unsigned remainingFrameSize) {
  *toPtr++ = (TWO_BYTE_DESCR_FLAG|(unsigned char)(remainingFrameSize>>8));
  *toPtr++ = (unsigned char)(remainingFrameSize&0xFF);
}

unsigned ADUdescriptor::getRemainingFrameSize(unsigned char*& fromPtr) {
  unsigned char firstByte = *fromPtr++;

  if (firstByte&TWO_BYTE_DESCR_FLAG) {
    // This is a 2-byte descriptor
    unsigned char secondByte = *fromPtr++;

    return ((firstByte&0x3F)<<8) | secondByte;
  } else {
    // This is a 1-byte descriptor
    return (firstByte&0x3F);
  }
}

