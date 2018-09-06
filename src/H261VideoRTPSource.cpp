/*
 * H261VideoRTPSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "H261VideoRTPSource.h"

H261VideoRTPSource*
H261VideoRTPSource::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
		unsigned rtpTimestampFrequency) {
	return new H261VideoRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency);
}

H261VideoRTPSource::H261VideoRTPSource(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
		unsigned rtpTimestampFrequency) :
		MultiFramedRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
				rtpTimestampFrequency), fLastSpecialHeader(0) {
}

H261VideoRTPSource::~H261VideoRTPSource() {
}

Boolean H261VideoRTPSource::processSpecialHeader(BufferedPacket* packet,
		unsigned& resultSpecialHeaderSize) {
	// There's a 4-byte video-specific header
	if (packet->dataSize() < 4)
		return False;

	DP_U8* headerStart = packet->data();
	fLastSpecialHeader = (headerStart[0] << 24) | (headerStart[1] << 16)
			| (headerStart[2] << 8) | headerStart[3];

#ifdef DELIVER_COMPLETE_FRAMES
	fCurrentPacketBeginsFrame = fCurrentPacketCompletesFrame;
	// whether the *previous* packet ended a frame

	// The RTP "M" (marker) bit indicates the last fragment of a frame:
	fCurrentPacketCompletesFrame = packet->rtpMarkerBit();
#endif

	resultSpecialHeaderSize = 4;
	return True;
}

char const* H261VideoRTPSource::MIMEtype() const {
	return "video/H261";
}

