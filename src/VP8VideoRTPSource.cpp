/*
 * VP8VideoRTPSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "VP8VideoRTPSource.h"

VP8VideoRTPSource*
VP8VideoRTPSource::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
		unsigned rtpTimestampFrequency) {
	return new VP8VideoRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency);
}

VP8VideoRTPSource::VP8VideoRTPSource(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
		unsigned rtpTimestampFrequency) :
		MultiFramedRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
				rtpTimestampFrequency) {
}

VP8VideoRTPSource::~VP8VideoRTPSource() {
}

#define incrHeader do { ++resultSpecialHeaderSize; ++headerStart; if (--packetSize == 0) return False; } while (0)

Boolean VP8VideoRTPSource::processSpecialHeader(BufferedPacket* packet,
		unsigned& resultSpecialHeaderSize) {
	DP_U8* headerStart = packet->data();
	unsigned packetSize = packet->dataSize();

	// The special header is from 1 to 6 bytes long.
	if (packetSize == 0)
		return False; // error
	resultSpecialHeaderSize = 1; // unless we learn otherwise

	u_int8_t const byte1 = *headerStart;
	Boolean const X = (byte1 & 0x80) != 0;
	Boolean const S = (byte1 & 0x10) != 0;
	u_int8_t const PartID = byte1 & 0x0F;

	fCurrentPacketBeginsFrame = S && PartID == 0;
	fCurrentPacketCompletesFrame = packet->rtpMarkerBit(); // RTP header's "M" bit

	if (X) {
		incrHeader
		;

		u_int8_t const byte2 = *headerStart;
		Boolean const I = (byte2 & 0x80) != 0;
		Boolean const L = (byte2 & 0x40) != 0;
		Boolean const T = (byte2 & 0x20) != 0;
		Boolean const K = (byte2 & 0x10) != 0;

		if (I) {
			incrHeader
			;
			if ((*headerStart) & 0x80) { // extension flag in the PictureID is set
				incrHeader
				;
			}
		}

		if (L)
			incrHeader
			;
		if (T || K)
			incrHeader
			;
	}

	return True;
}

char const* VP8VideoRTPSource::MIMEtype() const {
	return "video/VP8";
}

