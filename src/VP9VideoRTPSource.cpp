/*
 * VP9VideoRTPSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */



#include "VP9VideoRTPSource.h"

VP9VideoRTPSource*
VP9VideoRTPSource::createNew(UsageEnvironment& env, Groupsock* RTPgs,
			      unsigned char rtpPayloadFormat,
			      unsigned rtpTimestampFrequency) {
  return new VP9VideoRTPSource(env, RTPgs, rtpPayloadFormat,
			       rtpTimestampFrequency);
}

VP9VideoRTPSource
::VP9VideoRTPSource(UsageEnvironment& env, Groupsock* RTPgs,
		     unsigned char rtpPayloadFormat,
		     unsigned rtpTimestampFrequency)
  : MultiFramedRTPSource(env, RTPgs, rtpPayloadFormat, rtpTimestampFrequency) {
}

VP9VideoRTPSource::~VP9VideoRTPSource() {
}

#define incrHeader do { ++resultSpecialHeaderSize; ++headerStart; if (--packetSize == 0) return False; } while (0)

Boolean VP9VideoRTPSource
::processSpecialHeader(BufferedPacket* packet,
                       unsigned& resultSpecialHeaderSize) {
  unsigned char* headerStart = packet->data();
  unsigned packetSize = packet->dataSize();

  // Figure out the size of the special header.
  if (packetSize == 0) return False; // error
  resultSpecialHeaderSize = 1; // unless we learn otherwise

  u_int8_t const byte1 = *headerStart;
  Boolean const I = (byte1&0x80) != 0;
  Boolean const L = (byte1&0x40) != 0;
  Boolean const F = (byte1&0x20) != 0;
  Boolean const B = (byte1&0x10) != 0;
  Boolean const E = (byte1&0x08) != 0;
  Boolean const V = (byte1&0x04) != 0;
  Boolean const U = (byte1&0x02) != 0;

  fCurrentPacketBeginsFrame = B;
  fCurrentPacketCompletesFrame = E;
      // use this instead of the RTP header's 'M' bit (which might not be accurate)

  if (I) { // PictureID present
    incrHeader;
    Boolean const M = ((*headerStart)&0x80) != 0;
    if (M) incrHeader;
  }

  if (L) { // Layer indices present
    incrHeader;
    if (F) { // Reference indices present
      incrHeader;
      unsigned R = (*headerStart)&0x03;
      while (R-- > 0) {
	incrHeader;
	Boolean const X = ((*headerStart)&0x10) != 0;
	if (X) incrHeader;
      }
    }
  }

  if (V) { // Scalability Structure (SS) present
    incrHeader;
    unsigned patternLength = *headerStart;
    while (patternLength-- > 0) {
      incrHeader;
      unsigned R = (*headerStart)&0x03;
      while (R-- > 0) {
	incrHeader;
	Boolean const X = ((*headerStart)&0x10) != 0;
	if (X) incrHeader;
      }
    }
  }

  if (U) { // Scalability Structure Update (SU) present
    return False; // This structure isn't yet defined in the VP9 payload format I-D
  }

  return True;
}

char const* VP9VideoRTPSource::MIMEtype() const {
  return "video/VP9";
}


