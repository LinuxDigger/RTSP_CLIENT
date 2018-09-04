/*
 * SimpleRTPSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "SimpleRTPSource.h"
#include <string.h>
#include "strDup.h"

SimpleRTPSource*
SimpleRTPSource::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, unsigned char rtpPayloadFormat,
		unsigned rtpTimestampFrequency, char const* mimeTypeString,
		unsigned offset, Boolean doNormalMBitRule) {
	return new SimpleRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency, mimeTypeString, offset, doNormalMBitRule);
}

SimpleRTPSource::SimpleRTPSource(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, unsigned char rtpPayloadFormat,
		unsigned rtpTimestampFrequency, char const* mimeTypeString,
		unsigned offset, Boolean doNormalMBitRule) :
		MultiFramedRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
				rtpTimestampFrequency), fMIMEtypeString(strDup(mimeTypeString)), fOffset(
				offset) {
	fUseMBitForFrameEnd = doNormalMBitRule
			&& strncmp(mimeTypeString, "audio/", 6) != 0;
}

SimpleRTPSource::~SimpleRTPSource() {
	delete[] (char*) fMIMEtypeString;
}

Boolean SimpleRTPSource::processSpecialHeader(BufferedPacket* packet,
		unsigned& resultSpecialHeaderSize) {
	fCurrentPacketCompletesFrame = !fUseMBitForFrameEnd
			|| packet->rtpMarkerBit();

	resultSpecialHeaderSize = fOffset;
	return True;
}

char const* SimpleRTPSource::MIMEtype() const {
	if (fMIMEtypeString == NULL)
		return MultiFramedRTPSource::MIMEtype();

	return fMIMEtypeString;
}

