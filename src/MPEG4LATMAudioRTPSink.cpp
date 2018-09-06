/*
 * MPEG4LATMAudioRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "strDup.h"
#include "MPEG4LATMAudioRTPSink.h"

MPEG4LATMAudioRTPSink::MPEG4LATMAudioRTPSink(UsageEnvironment& env,
		CommonPlay *cpObj, Groupsock* RTPgs, u_int8_t rtpPayloadFormat,
		u_int32_t rtpTimestampFrequency, char const* streamMuxConfigString,
		unsigned numChannels, Boolean allowMultipleFramesPerPacket) :
		AudioRTPSink(env, cpObj, RTPgs, rtpPayloadFormat, rtpTimestampFrequency,
				"MP4A-LATM", numChannels), fStreamMuxConfigString(
				strDup(streamMuxConfigString)), fAllowMultipleFramesPerPacket(
				allowMultipleFramesPerPacket) {
	// Set up the "a=fmtp:" SDP line for this stream:
	char const* fmtpFmt = "a=fmtp:%d "
			"cpresent=0;config=%s\r\n";
	unsigned fmtpFmtSize = strlen(fmtpFmt) + 3 /* max char len */
	+ strlen(fStreamMuxConfigString);
	char* fmtp = new char[fmtpFmtSize];
	sprintf(fmtp, fmtpFmt, rtpPayloadType(), fStreamMuxConfigString);
	fFmtpSDPLine = strDup(fmtp);
	delete[] fmtp;
}

MPEG4LATMAudioRTPSink::~MPEG4LATMAudioRTPSink() {
	delete[] fFmtpSDPLine;
	delete[] (char*) fStreamMuxConfigString;
}

MPEG4LATMAudioRTPSink*
MPEG4LATMAudioRTPSink::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, u_int8_t rtpPayloadFormat,
		u_int32_t rtpTimestampFrequency, char const* streamMuxConfigString,
		unsigned numChannels, Boolean allowMultipleFramesPerPacket) {
	return new MPEG4LATMAudioRTPSink(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency, streamMuxConfigString, numChannels,
			allowMultipleFramesPerPacket);
}

Boolean MPEG4LATMAudioRTPSink::frameCanAppearAfterPacketStart(
		DP_U8 const* /*frameStart*/,
		unsigned /*numBytesInFrame*/) const {
	return fAllowMultipleFramesPerPacket;
}

void MPEG4LATMAudioRTPSink::doSpecialFrameHandling(unsigned fragmentationOffset,
		DP_U8* frameStart, unsigned numBytesInFrame,
		struct timeval framePresentationTime, unsigned numRemainingBytes) {
	if (numRemainingBytes == 0) {
		// This packet contains the last (or only) fragment of the frame.
		// Set the RTP 'M' ('marker') bit:
		setMarkerBit();
	}

	// Important: Also call our base class's doSpecialFrameHandling(),
	// to set the packet's timestamp:
	MultiFramedRTPSink::doSpecialFrameHandling(fragmentationOffset, frameStart,
			numBytesInFrame, framePresentationTime, numRemainingBytes);
}

char const* MPEG4LATMAudioRTPSink::auxSDPLine() {
	return fFmtpSDPLine;
}

