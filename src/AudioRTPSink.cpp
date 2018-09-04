/*
 * AudioRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "AudioRTPSink.h"

AudioRTPSink::AudioRTPSink(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* rtpgs, unsigned char rtpPayloadType,
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

