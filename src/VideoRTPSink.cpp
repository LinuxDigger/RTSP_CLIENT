/*
 * VideoRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "VideoRTPSink.h"

VideoRTPSink::VideoRTPSink(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* rtpgs, unsigned char rtpPayloadType,
		unsigned rtpTimestampFrequency, char const* rtpPayloadFormatName) :
		MultiFramedRTPSink(env, cpObj, rtpgs, rtpPayloadType,
				rtpTimestampFrequency, rtpPayloadFormatName) {
}

VideoRTPSink::~VideoRTPSink() {
}

char const* VideoRTPSink::sdpMediaType() const {
	return "video";
}

