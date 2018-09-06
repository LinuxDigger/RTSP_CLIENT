/*
 * DVVideoRTPSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "DVVideoRTPSource.h"

DVVideoRTPSource*
DVVideoRTPSource::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
		unsigned rtpTimestampFrequency) {
	return new DVVideoRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency);
}

DVVideoRTPSource::DVVideoRTPSource(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* rtpGS, DP_U8 rtpPayloadFormat,
		unsigned rtpTimestampFrequency) :
		MultiFramedRTPSource(env, cpObj, rtpGS, rtpPayloadFormat,
				rtpTimestampFrequency) {
}

DVVideoRTPSource::~DVVideoRTPSource() {
}

#define DV_DIF_BLOCK_SIZE 80
#define DV_SECTION_HEADER 0x1F

Boolean DVVideoRTPSource::processSpecialHeader(BufferedPacket* packet,
		unsigned& resultSpecialHeaderSize) {
	unsigned const packetSize = packet->dataSize();
	if (packetSize < DV_DIF_BLOCK_SIZE)
		return False; // TARFU!

	u_int8_t const* data = packet->data();
	fCurrentPacketBeginsFrame = data[0] == DV_SECTION_HEADER
			&& (data[1] & 0xf8) == 0 && data[2] == 0; // thanks to Ben Hutchings

			// The RTP "M" (marker) bit indicates the last fragment of a frame:
	fCurrentPacketCompletesFrame = packet->rtpMarkerBit();

	// There is no special header
	resultSpecialHeaderSize = 0;
	return True;
}

char const* DVVideoRTPSource::MIMEtype() const {
	return "video/DV";
}

