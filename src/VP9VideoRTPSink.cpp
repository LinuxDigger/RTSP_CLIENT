/*
 * VP9VideoRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "VP9VideoRTPSink.h"

VP9VideoRTPSink::VP9VideoRTPSink(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat) :
		VideoRTPSink(env, cpObj, RTPgs, rtpPayloadFormat, 90000, "VP9") {
}

VP9VideoRTPSink::~VP9VideoRTPSink() {
}

VP9VideoRTPSink*
VP9VideoRTPSink::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat) {
	return new VP9VideoRTPSink(env, cpObj, RTPgs, rtpPayloadFormat);
}

Boolean VP9VideoRTPSink::frameCanAppearAfterPacketStart(
		DP_U8 const* /*frameStart*/,
		unsigned /*numBytesInFrame*/) const {
	// A packet can contain only one frame
	return False;
}

void VP9VideoRTPSink::doSpecialFrameHandling(unsigned fragmentationOffset,
		DP_U8* /*frameStart*/, unsigned /*numBytesInFrame*/,
		struct timeval framePresentationTime, unsigned numRemainingBytes) {
	// Set the "VP9 Payload Descriptor" (just the minimal required 1-byte version):
	u_int8_t vp9PayloadDescriptor = fragmentationOffset == 0 ? 0x10 : 0x00;
	// I = L = F = V = U = 0; S = 1 iff this is the first (or only) fragment of the frame

	if (numRemainingBytes == 0) {
		// This packet contains the last (or only) fragment of the frame.
		// Set the E bit:
		vp9PayloadDescriptor |= 0x08;
		// Also set the RTP 'M' ('marker') bit:
		setMarkerBit();
	}

	setSpecialHeaderBytes(&vp9PayloadDescriptor, 1);

	// Also set the RTP timestamp:
	setTimestamp(framePresentationTime);
}

unsigned VP9VideoRTPSink::specialHeaderSize() const {
	// We include only the required 1-byte form of the "VP9 Payload Descriptor":
	return 1;
}

