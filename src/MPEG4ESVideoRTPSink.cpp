/*
 * MPEG4ESVideoRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "MPEG4ESVideoRTPSink.h"
#include "MPEG4VideoStreamFramer.h"
#include "MPEG4LATMAudioRTPSource.h" // for "parseGeneralConfigStr()"
#include "strDup.h"

MPEG4ESVideoRTPSink::MPEG4ESVideoRTPSink(UsageEnvironment& env,
		CommonPlay *cpObj, Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
		u_int32_t rtpTimestampFrequency, u_int8_t profileAndLevelIndication,
		char const* configStr) :
		VideoRTPSink(env, cpObj, RTPgs, rtpPayloadFormat, rtpTimestampFrequency,
				"MP4V-ES"), fVOPIsPresent(False), fProfileAndLevelIndication(
				profileAndLevelIndication), fFmtpSDPLine(NULL) {
	fConfigBytes = parseGeneralConfigStr(configStr, fNumConfigBytes);
}

MPEG4ESVideoRTPSink::~MPEG4ESVideoRTPSink() {
	delete[] fFmtpSDPLine;
	delete[] fConfigBytes;
}

MPEG4ESVideoRTPSink*
MPEG4ESVideoRTPSink::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
		u_int32_t rtpTimestampFrequency) {
	return new MPEG4ESVideoRTPSink(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency);
}

MPEG4ESVideoRTPSink*
MPEG4ESVideoRTPSink::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
		u_int32_t rtpTimestampFrequency, u_int8_t profileAndLevelIndication,
		char const* configStr) {
	return new MPEG4ESVideoRTPSink(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency, profileAndLevelIndication, configStr);
}

Boolean MPEG4ESVideoRTPSink::sourceIsCompatibleWithUs(MediaSource& source) {
	// Our source must be an appropriate framer:
	return source.isMPEG4VideoStreamFramer();
}

#define VOP_START_CODE                    0x000001B6

void MPEG4ESVideoRTPSink::doSpecialFrameHandling(unsigned fragmentationOffset,
		DP_U8* frameStart, unsigned numBytesInFrame,
		struct timeval framePresentationTime, unsigned numRemainingBytes) {
	if (fragmentationOffset == 0) {
		// Begin by inspecting the 4-byte code at the start of the frame:
		if (numBytesInFrame < 4)
			return; // shouldn't happen
		u_int32_t startCode = (frameStart[0] << 24) | (frameStart[1] << 16)
				| (frameStart[2] << 8) | frameStart[3];

		fVOPIsPresent = startCode == VOP_START_CODE;
	}

	// Set the RTP 'M' (marker) bit iff this frame ends a VOP
	// (and there are no fragments remaining).
	// This relies on the source being a "MPEG4VideoStreamFramer".
	MPEG4VideoStreamFramer* framerSource = (MPEG4VideoStreamFramer*) fSource;
	if (framerSource != NULL && framerSource->pictureEndMarker()
			&& numRemainingBytes == 0) {
		setMarkerBit();
		framerSource->pictureEndMarker() = False;
	}

	// Also set the RTP timestamp.  (We do this for each frame
	// in the packet, to ensure that the timestamp of the VOP (if present)
	// gets used.)
	setTimestamp(framePresentationTime);
}

Boolean MPEG4ESVideoRTPSink::allowFragmentationAfterStart() const {
	return True;
}

Boolean MPEG4ESVideoRTPSink::frameCanAppearAfterPacketStart(
		DP_U8 const* /*frameStart*/,
		unsigned /*numBytesInFrame*/) const {
	// Once we've packed a VOP into the packet, then no other
	// frame can be packed into it:
	return !fVOPIsPresent;
}

char const* MPEG4ESVideoRTPSink::auxSDPLine() {
	// Generate a new "a=fmtp:" line each time, using our own 'configuration' information (if we have it),
	// otherwise parameters from our framer source (in case they've changed since the last time that
	// we were called):
	unsigned configLength = fNumConfigBytes;
	DP_U8* config = fConfigBytes;
	if (fProfileAndLevelIndication == 0 || config == NULL) {
		// We need to get this information from our framer source:
		MPEG4VideoStreamFramer* framerSource = (MPEG4VideoStreamFramer*) fSource;
		if (framerSource == NULL)
			return NULL; // we don't yet have a source

		fProfileAndLevelIndication =
				framerSource->profile_and_level_indication();
		if (fProfileAndLevelIndication == 0)
			return NULL; // our source isn't ready

		config = framerSource->getConfigBytes(configLength);
		if (config == NULL)
			return NULL; // our source isn't ready
	}

	char const* fmtpFmt = "a=fmtp:%d "
			"profile-level-id=%d;"
			"config=";
	unsigned fmtpFmtSize = strlen(fmtpFmt) + 3 /* max char len */
	+ 3 /* max char len */
	+ 2 * configLength /* 2*, because each byte prints as 2 chars */
	+ 2 /* trailing \r\n */;
	char* fmtp = new char[fmtpFmtSize];
	sprintf(fmtp, fmtpFmt, rtpPayloadType(), fProfileAndLevelIndication);
	char* endPtr = &fmtp[strlen(fmtp)];
	for (unsigned i = 0; i < configLength; ++i) {
		sprintf(endPtr, "%02X", config[i]);
		endPtr += 2;
	}
	sprintf(endPtr, "\r\n");

	delete[] fFmtpSDPLine;
	fFmtpSDPLine = strDup(fmtp);
	delete[] fmtp;
	return fFmtpSDPLine;
}

