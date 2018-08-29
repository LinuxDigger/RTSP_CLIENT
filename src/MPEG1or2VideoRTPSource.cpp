/*
 * MPEG1or2VideoRTPSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */


#include "MPEG1or2VideoRTPSource.h"

MPEG1or2VideoRTPSource*
MPEG1or2VideoRTPSource::createNew(UsageEnvironment& env, Groupsock* RTPgs,
			      unsigned char rtpPayloadFormat,
			      unsigned rtpTimestampFrequency) {
  return new MPEG1or2VideoRTPSource(env, RTPgs, rtpPayloadFormat,
				rtpTimestampFrequency);
}

MPEG1or2VideoRTPSource::MPEG1or2VideoRTPSource(UsageEnvironment& env,
				       Groupsock* RTPgs,
				       unsigned char rtpPayloadFormat,
				       unsigned rtpTimestampFrequency)
  : MultiFramedRTPSource(env, RTPgs,
			 rtpPayloadFormat, rtpTimestampFrequency){
}

MPEG1or2VideoRTPSource::~MPEG1or2VideoRTPSource() {
}

Boolean MPEG1or2VideoRTPSource
::processSpecialHeader(BufferedPacket* packet,
		       unsigned& resultSpecialHeaderSize) {
  // There's a 4-byte video-specific header
  if (packet->dataSize() < 4) return False;

  u_int32_t header = ntohl(*(u_int32_t*)(packet->data()));

  u_int32_t sBit = header&0x00002000; // sequence-header-present
  u_int32_t bBit = header&0x00001000; // beginning-of-slice
  u_int32_t eBit = header&0x00000800; // end-of-slice

  fCurrentPacketBeginsFrame = (sBit|bBit) != 0;
  fCurrentPacketCompletesFrame = ((sBit != 0) && (bBit == 0)) || (eBit != 0);

  resultSpecialHeaderSize = 4;
  return True;
}

Boolean MPEG1or2VideoRTPSource
::packetIsUsableInJitterCalculation(unsigned char* packet,
				    unsigned packetSize) {
  // There's a 4-byte video-specific header
  if (packetSize < 4) return False;

  // Extract the "Picture-Type" field from this, to determine whether
  // this packet can be used in jitter calculations:
  unsigned header = ntohl(*(u_int32_t*)packet);

  unsigned short pictureType = (header>>8)&0x7;
  if (pictureType == 1) { // an I frame
    return True;
  } else { // a P, B, D, or other unknown frame type
    return False;
  }
}

char const* MPEG1or2VideoRTPSource::MIMEtype() const {
  return "video/MPEG";
}




