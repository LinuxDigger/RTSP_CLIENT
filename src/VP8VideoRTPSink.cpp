/*
 * VP8VideoRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "VP8VideoRTPSink.h"

VP8VideoRTPSink::VP8VideoRTPSink(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat) :
		VideoRTPSink(env, cpObj, RTPgs, rtpPayloadFormat, 90000, "VP8") {
}

VP8VideoRTPSink::~VP8VideoRTPSink() {
}

VP8VideoRTPSink*
VP8VideoRTPSink::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat) {
	return new VP8VideoRTPSink(env, cpObj, RTPgs, rtpPayloadFormat);
}

Boolean VP8VideoRTPSink::frameCanAppearAfterPacketStart(
		DP_U8 const* /*frameStart*/,
		unsigned /*numBytesInFrame*/) const {
	// A packet can contain only one frame
	return False;
}

void VP8VideoRTPSink::doSpecialFrameHandling(unsigned fragmentationOffset,
		DP_U8* /*frameStart*/, unsigned /*numBytesInFrame*/,
		struct timeval framePresentationTime, unsigned numRemainingBytes) {
	// Set the "VP8 Payload Descriptor" (just the minimal required 1-byte version):
	u_int8_t vp8PayloadDescriptor = fragmentationOffset == 0 ? 0x10 : 0x00;
	// X = R = N = 0; PartID = 0; S = 1 iff this is the first (or only) fragment of the frame
	setSpecialHeaderBytes(&vp8PayloadDescriptor, 1);

	if (numRemainingBytes == 0) {
		// This packet contains the last (or only) fragment of the frame.
		// Set the RTP 'M' ('marker') bit:
		setMarkerBit();
	}

	// Also set the RTP timestamp:
	setTimestamp(framePresentationTime);
}

unsigned VP8VideoRTPSink::specialHeaderSize() const {
	// We include only the required 1-byte form of the "VP8 Payload Descriptor":
	return 1;
}

