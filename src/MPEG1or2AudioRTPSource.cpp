/*
 * MPEG1or2AudioRTPSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "MPEG1or2AudioRTPSource.h"

MPEG1or2AudioRTPSource*
MPEG1or2AudioRTPSource::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
		unsigned rtpTimestampFrequency) {
	return new MPEG1or2AudioRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency);
}

MPEG1or2AudioRTPSource::MPEG1or2AudioRTPSource(UsageEnvironment& env,
		CommonPlay *cpObj, Groupsock* rtpGS, DP_U8 rtpPayloadFormat,
		unsigned rtpTimestampFrequency) :
		MultiFramedRTPSource(env, cpObj, rtpGS, rtpPayloadFormat,
				rtpTimestampFrequency) {
}

MPEG1or2AudioRTPSource::~MPEG1or2AudioRTPSource() {
}

Boolean MPEG1or2AudioRTPSource::processSpecialHeader(BufferedPacket* packet,
		unsigned& resultSpecialHeaderSize) {
	// There's a 4-byte header indicating fragmentation.
	if (packet->dataSize() < 4)
		return False;

	// Note: This fragmentation header is actually useless to us, because
	// it doesn't tell us whether or not this RTP packet *ends* a
	// fragmented frame.  Thus, we can't use it to properly set
	// "fCurrentPacketCompletesFrame".  Instead, we assume that even
	// a partial audio frame will be usable to clients.

	resultSpecialHeaderSize = 4;
	return True;
}

char const* MPEG1or2AudioRTPSource::MIMEtype() const {
	return "audio/MPEG";
}

