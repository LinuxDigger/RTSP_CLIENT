/*
 * AC3AudioRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "AC3AudioRTPSink.h"

AC3AudioRTPSink::AC3AudioRTPSink(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, u_int8_t rtpPayloadFormat,
		u_int32_t rtpTimestampFrequency) :
		AudioRTPSink(env, cpObj, RTPgs, rtpPayloadFormat, rtpTimestampFrequency,
				"AC3"), fTotNumFragmentsUsed(0) {
}

AC3AudioRTPSink::~AC3AudioRTPSink() {
}

AC3AudioRTPSink*
AC3AudioRTPSink::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, u_int8_t rtpPayloadFormat,
		u_int32_t rtpTimestampFrequency) {
	return new AC3AudioRTPSink(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency);
}

Boolean AC3AudioRTPSink::frameCanAppearAfterPacketStart(
		DP_U8 const* /*frameStart*/,
		unsigned /*numBytesInFrame*/) const {
	// (For now) allow at most 1 frame in a single packet:
	return False;
}

void AC3AudioRTPSink::doSpecialFrameHandling(unsigned fragmentationOffset,
		DP_U8* frameStart, unsigned numBytesInFrame,
		struct timeval framePresentationTime, unsigned numRemainingBytes) {
	// Set the 2-byte "payload header", as defined in RFC 4184.
	DP_U8 headers[2];

	Boolean isFragment = numRemainingBytes > 0 || fragmentationOffset > 0;
	if (!isFragment) {
		headers[0] = 0; // One or more complete frames
		headers[1] = 1; // because we (for now) allow at most 1 frame per packet
	} else {
		if (fragmentationOffset > 0) {
			headers[0] = 3; // Fragment of frame other than initial fragment
		} else {
			// An initial fragment of the frame
			unsigned const totalFrameSize = fragmentationOffset
					+ numBytesInFrame + numRemainingBytes;
			unsigned const fiveEighthsPoint = totalFrameSize / 2
					+ totalFrameSize / 8;
			headers[0] = numBytesInFrame >= fiveEighthsPoint ? 1 : 2;

			// Because this outgoing packet will be full (because it's an initial fragment), we can compute how many total
			// fragments (and thus packets) will make up the complete AC-3 frame:
			fTotNumFragmentsUsed = (totalFrameSize + (numBytesInFrame - 1))
					/ numBytesInFrame;
		}

		headers[1] = fTotNumFragmentsUsed;
	}

	setSpecialHeaderBytes(headers, sizeof headers);

	if (numRemainingBytes == 0) {
		// This packet contains the last (or only) fragment of the frame.
		// Set the RTP 'M' ('marker') bit:
		setMarkerBit();
	}

	// Important: Also call our base class's doSpecialFrameHandling(),
	// to set the packet's timestamp:
	MultiFramedRTPSink::doSpecialFrameHandling(fragmentationOffset, frameStart,
			numBytesInFrame, framePresentationTime, numRemainingBytes);
}

unsigned AC3AudioRTPSink::specialHeaderSize() const {
	return 2;
}

