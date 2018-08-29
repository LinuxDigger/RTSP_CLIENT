/*
 * MP3ADUdescriptor.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MP3ADUDESCRIPTOR_H_
#define INCLUDE_MP3ADUDESCRIPTOR_H_


// A class for handling the descriptor that begins each ADU frame:
//   (Note: We don't yet implement fragmentation)
class ADUdescriptor {
public:
  // Operations for generating a new descriptor
  static unsigned computeSize(unsigned remainingFrameSize) {
    return remainingFrameSize >= 64 ? 2 : 1;
  }
  static unsigned generateDescriptor(unsigned char*& toPtr, unsigned remainingFrameSize);
   // returns descriptor size; increments "toPtr" afterwards
  static void generateTwoByteDescriptor(unsigned char*& toPtr, unsigned remainingFrameSize);
   // always generates a 2-byte descriptor, even if "remainingFrameSize" is
   // small enough for a 1-byte descriptor

  // Operations for reading a descriptor
  static unsigned getRemainingFrameSize(unsigned char*& fromPtr);
   // increments "fromPtr" afterwards
};



#endif /* INCLUDE_MP3ADUDESCRIPTOR_H_ */
