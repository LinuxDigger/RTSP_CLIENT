/*
 * MPEG4GenericRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "MPEG4GenericRTPSink.h"
#include "Locale.h"
#include <ctype.h> // needed on some systems to define "tolower()"
#include "strDup.h"

MPEG4GenericRTPSink::MPEG4GenericRTPSink(UsageEnvironment& env,
		CommonPlay *cpObj, Groupsock* RTPgs, u_int8_t rtpPayloadFormat,
		u_int32_t rtpTimestampFrequency, char const* sdpMediaTypeString,
		char const* mpeg4Mode, char const* configString, unsigned numChannels) :
		MultiFramedRTPSink(env, cpObj, RTPgs, rtpPayloadFormat,
				rtpTimestampFrequency, "MPEG4-GENERIC", numChannels), fSDPMediaTypeString(
				strDup(sdpMediaTypeString)), fMPEG4Mode(strDup(mpeg4Mode)), fConfigString(
				strDup(configString)) {
	// Check whether "mpeg4Mode" is one that we handle:
	if (mpeg4Mode == NULL) {
		env << "MPEG4GenericRTPSink error: NULL \"mpeg4Mode\" parameter\n";
	} else {
		// To ease comparison, convert "mpeg4Mode" to lower case:
		size_t const len = strlen(mpeg4Mode) + 1;
		char* m = new char[len];

		Locale l("POSIX");
		for (size_t i = 0; i < len; ++i)
			m[i] = tolower(mpeg4Mode[i]);

		if (strcmp(m, "aac-hbr") != 0) {
			env
					<< "MPEG4GenericRTPSink error: Unknown \"mpeg4Mode\" parameter: \""
					<< mpeg4Mode << "\"\n";
		}
		delete[] m;
	}

	// Set up the "a=fmtp:" SDP line for this stream:
	char const* fmtpFmt = "a=fmtp:%d "
			"streamtype=%d;profile-level-id=1;"
			"mode=%s;sizelength=13;indexlength=3;indexdeltalength=3;"
			"config=%s\r\n";
	unsigned fmtpFmtSize = strlen(fmtpFmt) + 3 /* max char len */
	+ 3 /* max char len */
	+ strlen(fMPEG4Mode) + strlen(fConfigString);
	char* fmtp = new char[fmtpFmtSize];
	sprintf(fmtp, fmtpFmt, rtpPayloadType(),
			strcmp(fSDPMediaTypeString, "video") == 0 ? 4 : 5, fMPEG4Mode,
			fConfigString);
	fFmtpSDPLine = strDup(fmtp);
	delete[] fmtp;
}

MPEG4GenericRTPSink::~MPEG4GenericRTPSink() {
	delete[] fFmtpSDPLine;
	delete[] (char*) fConfigString;
	delete[] (char*) fMPEG4Mode;
	delete[] (char*) fSDPMediaTypeString;
}

MPEG4GenericRTPSink*
MPEG4GenericRTPSink::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, u_int8_t rtpPayloadFormat,
		u_int32_t rtpTimestampFrequency, char const* sdpMediaTypeString,
		char const* mpeg4Mode, char const* configString, unsigned numChannels) {
	return new MPEG4GenericRTPSink(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency, sdpMediaTypeString, mpeg4Mode, configString,
			numChannels);
}

Boolean MPEG4GenericRTPSink::frameCanAppearAfterPacketStart(
		DP_U8 const* /*frameStart*/,
		unsigned /*numBytesInFrame*/) const {
	// (For now) allow at most 1 frame in a single packet:
	return False;
}

void MPEG4GenericRTPSink::doSpecialFrameHandling(unsigned fragmentationOffset,
		DP_U8* frameStart, unsigned numBytesInFrame,
		struct timeval framePresentationTime, unsigned numRemainingBytes) {
	// Set the "AU Header Section".  This is 4 bytes: 2 bytes for the
	// initial "AU-headers-length" field, and 2 bytes for the first
	// (and only) "AU Header":
	unsigned fullFrameSize = fragmentationOffset + numBytesInFrame
			+ numRemainingBytes;
	DP_U8 headers[4];
	headers[0] = 0;
	headers[1] = 16 /* bits */; // AU-headers-length
	headers[2] = fullFrameSize >> 5;
	headers[3] = (fullFrameSize & 0x1F) << 3;

	setSpecialHeaderBytes(headers, sizeof headers);

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

unsigned MPEG4GenericRTPSink::specialHeaderSize() const {
	return 2 + 2;
}

char const* MPEG4GenericRTPSink::sdpMediaType() const {
	return fSDPMediaTypeString;
}

char const* MPEG4GenericRTPSink::auxSDPLine() {
	return fFmtpSDPLine;
}

