/*
 * GSMAudioRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "GSMAudioRTPSink.h"

GSMAudioRTPSink::GSMAudioRTPSink(UsageEnvironment& env, Groupsock* RTPgs) :
		AudioRTPSink(env, RTPgs, 3, 8000, "GSM") {
}

GSMAudioRTPSink::~GSMAudioRTPSink() {
}

GSMAudioRTPSink*
GSMAudioRTPSink::createNew(UsageEnvironment& env, Groupsock* RTPgs) {
	return new GSMAudioRTPSink(env, RTPgs);
}

Boolean GSMAudioRTPSink::frameCanAppearAfterPacketStart(
		unsigned char const* /*frameStart*/,
		unsigned /*numBytesInFrame*/) const {
	// Allow at most 5 frames in a single packet:
	return numFramesUsedSoFar() < 5;
}

