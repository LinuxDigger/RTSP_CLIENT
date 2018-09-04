/*
 * AC3AudioRTPSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "AC3AudioRTPSource.h"

AC3AudioRTPSource*
AC3AudioRTPSource::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, unsigned char rtpPayloadFormat,
		unsigned rtpTimestampFrequency) {
	return new AC3AudioRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency);
}

AC3AudioRTPSource::AC3AudioRTPSource(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* rtpGS, unsigned char rtpPayloadFormat,
		unsigned rtpTimestampFrequency) :
		MultiFramedRTPSource(env, cpObj, rtpGS, rtpPayloadFormat,
				rtpTimestampFrequency) {
}

AC3AudioRTPSource::~AC3AudioRTPSource() {
}

Boolean AC3AudioRTPSource::processSpecialHeader(BufferedPacket* packet,
		unsigned& resultSpecialHeaderSize) {
	unsigned char* headerStart = packet->data();
	unsigned packetSize = packet->dataSize();

	// There's a 2-byte payload header at the beginning:
	if (packetSize < 2)
		return False;
	resultSpecialHeaderSize = 2;

	unsigned char FT = headerStart[0] & 0x03;
	fCurrentPacketBeginsFrame = FT != 3;

	// The RTP "M" (marker) bit indicates the last fragment of a frame.
	// In case the sender did not set the "M" bit correctly, we also test for FT == 0:
	fCurrentPacketCompletesFrame = packet->rtpMarkerBit() || FT == 0;

	return True;
}

char const* AC3AudioRTPSource::MIMEtype() const {
	return "audio/AC3";
}

