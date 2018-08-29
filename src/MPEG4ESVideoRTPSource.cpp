/*
 * MPEG4ESVideoRTPSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "MPEG4ESVideoRTPSource.h"

///////// MPEG4ESVideoRTPSource implementation ////////

//##### NOTE: INCOMPLETE!!! #####

MPEG4ESVideoRTPSource*
MPEG4ESVideoRTPSource::createNew(UsageEnvironment& env, Groupsock* RTPgs,
		unsigned char rtpPayloadFormat, unsigned rtpTimestampFrequency) {
	return new MPEG4ESVideoRTPSource(env, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency);
}

MPEG4ESVideoRTPSource::MPEG4ESVideoRTPSource(UsageEnvironment& env,
		Groupsock* RTPgs, unsigned char rtpPayloadFormat,
		unsigned rtpTimestampFrequency) :
		MultiFramedRTPSource(env, RTPgs, rtpPayloadFormat,
				rtpTimestampFrequency) {
}

MPEG4ESVideoRTPSource::~MPEG4ESVideoRTPSource() {
}

Boolean MPEG4ESVideoRTPSource::processSpecialHeader(BufferedPacket* packet,
		unsigned& resultSpecialHeaderSize) {
	// The packet begins a frame iff its data begins with a system code
	// (i.e., 0x000001??)
	fCurrentPacketBeginsFrame = packet->dataSize() >= 4
			&& (packet->data())[0] == 0 && (packet->data())[1] == 0
			&& (packet->data())[2] == 1;

	// The RTP "M" (marker) bit indicates the last fragment of a frame:
	fCurrentPacketCompletesFrame = packet->rtpMarkerBit();

	// There is no special header
	resultSpecialHeaderSize = 0;
	return True;
}

char const* MPEG4ESVideoRTPSource::MIMEtype() const {
	return "video/MP4V-ES";
}

