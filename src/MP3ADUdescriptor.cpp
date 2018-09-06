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

unsigned ADUdescriptor::generateDescriptor(DP_U8*& toPtr,
					   unsigned remainingFrameSize) {
  unsigned descriptorSize = ADUdescriptor::computeSize(remainingFrameSize);
  switch (descriptorSize) {
  case 1: {
    *toPtr++ = (DP_U8)remainingFrameSize;
    break;
  }
  case 2: {
    generateTwoByteDescriptor(toPtr, remainingFrameSize);
    break;
  }
  }

  return descriptorSize;
}

void ADUdescriptor::generateTwoByteDescriptor(DP_U8*& toPtr,
					      unsigned remainingFrameSize) {
  *toPtr++ = (TWO_BYTE_DESCR_FLAG|(DP_U8)(remainingFrameSize>>8));
  *toPtr++ = (DP_U8)(remainingFrameSize&0xFF);
}

unsigned ADUdescriptor::getRemainingFrameSize(DP_U8*& fromPtr) {
  DP_U8 firstByte = *fromPtr++;

  if (firstByte&TWO_BYTE_DESCR_FLAG) {
    // This is a 2-byte descriptor
    DP_U8 secondByte = *fromPtr++;

    return ((firstByte&0x3F)<<8) | secondByte;
  } else {
    // This is a 1-byte descriptor
    return (firstByte&0x3F);
  }
}

