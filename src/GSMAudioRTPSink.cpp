/*
 * GSMAudioRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "GSMAudioRTPSink.h"

GSMAudioRTPSink::GSMAudioRTPSink(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs) :
		AudioRTPSink(env, cpObj, RTPgs, 3, 8000, "GSM") {
}

GSMAudioRTPSink::~GSMAudioRTPSink() {
}

GSMAudioRTPSink*
GSMAudioRTPSink::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs) {
	return new GSMAudioRTPSink(env, cpObj, RTPgs);
}

Boolean GSMAudioRTPSink::frameCanAppearAfterPacketStart(
		DP_U8 const* /*frameStart*/,
		unsigned /*numBytesInFrame*/) const {
	// Allow at most 5 frames in a single packet:
	return numFramesUsedSoFar() < 5;
}

