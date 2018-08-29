/*
 * TextRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "TextRTPSink.h"

TextRTPSink::TextRTPSink(UsageEnvironment& env, Groupsock* rtpgs,
		unsigned char rtpPayloadType, unsigned rtpTimestampFrequency,
		char const* rtpPayloadFormatName) :
		MultiFramedRTPSink(env, rtpgs, rtpPayloadType, rtpTimestampFrequency,
				rtpPayloadFormatName) {
}

TextRTPSink::~TextRTPSink() {
}

char const* TextRTPSink::sdpMediaType() const {
	return "text";
}

