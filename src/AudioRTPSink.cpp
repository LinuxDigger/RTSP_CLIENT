/*
 * AudioRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "AudioRTPSink.h"

AudioRTPSink::AudioRTPSink(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* rtpgs, DP_U8 rtpPayloadType,
		unsigned rtpTimestampFrequency, char const* rtpPayloadFormatName,
		unsigned numChannels) :
		MultiFramedRTPSink(env, cpObj, rtpgs, rtpPayloadType,
				rtpTimestampFrequency, rtpPayloadFormatName, numChannels) {
}

AudioRTPSink::~AudioRTPSink() {
}

char const* AudioRTPSink::sdpMediaType() const {
	return "audio";
}

