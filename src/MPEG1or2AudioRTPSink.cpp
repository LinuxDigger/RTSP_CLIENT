/*
 * MPEG1or2AudioRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "MPEG1or2AudioRTPSink.h"

MPEG1or2AudioRTPSink::MPEG1or2AudioRTPSink(UsageEnvironment& env,
		CommonPlay *cpObj, Groupsock* RTPgs) :
		AudioRTPSink(env, cpObj, RTPgs, 14, 90000, "MPA") {
}

MPEG1or2AudioRTPSink::~MPEG1or2AudioRTPSink() {
}

MPEG1or2AudioRTPSink*
MPEG1or2AudioRTPSink::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs) {
	return new MPEG1or2AudioRTPSink(env, cpObj, RTPgs);
}

void MPEG1or2AudioRTPSink::doSpecialFrameHandling(unsigned fragmentationOffset,
		DP_U8* frameStart, unsigned numBytesInFrame,
		struct timeval framePresentationTime, unsigned numRemainingBytes) {
	// If this is the 1st frame in the 1st packet, set the RTP 'M' (marker)
	// bit (because this is considered the start of a talk spurt):
	if (isFirstPacket() && isFirstFrameInPacket()) {
		setMarkerBit();
	}

	// If this is the first frame in the packet, set the lower half of the
	// audio-specific header (to the "fragmentationOffset"):
	if (isFirstFrameInPacket()) {
		setSpecialHeaderWord(fragmentationOffset & 0xFFFF);
	}

	// Important: Also call our base class's doSpecialFrameHandling(),
	// to set the packet's timestamp:
	MultiFramedRTPSink::doSpecialFrameHandling(fragmentationOffset, frameStart,
			numBytesInFrame, framePresentationTime, numRemainingBytes);
}

unsigned MPEG1or2AudioRTPSink::specialHeaderSize() const {
	// There's a 4 byte special audio header:
	return 4;
}

