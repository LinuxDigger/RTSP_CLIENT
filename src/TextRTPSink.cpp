/*
 * TextRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "TextRTPSink.h"

TextRTPSink::TextRTPSink(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* rtpgs, DP_U8 rtpPayloadType,
		unsigned rtpTimestampFrequency, char const* rtpPayloadFormatName) :
		MultiFramedRTPSink(env, cpObj, rtpgs, rtpPayloadType,
				rtpTimestampFrequency, rtpPayloadFormatName) {
}

TextRTPSink::~TextRTPSink() {
}

char const* TextRTPSink::sdpMediaType() const {
	return "text";
}

