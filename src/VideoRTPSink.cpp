/*
 * VideoRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "VideoRTPSink.h"

VideoRTPSink::VideoRTPSink(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* rtpgs, DP_U8 rtpPayloadType,
		unsigned rtpTimestampFrequency, char const* rtpPayloadFormatName) :
		MultiFramedRTPSink(env, cpObj, rtpgs, rtpPayloadType,
				rtpTimestampFrequency, rtpPayloadFormatName) {
}

VideoRTPSink::~VideoRTPSink() {
}

char const* VideoRTPSink::sdpMediaType() const {
	return "video";
}

