/*
 * H263plusVideoRTPSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "H263plusVideoRTPSource.h"

H263plusVideoRTPSource*
H263plusVideoRTPSource::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, unsigned char rtpPayloadFormat,
		unsigned rtpTimestampFrequency) {
	return new H263plusVideoRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency);
}

H263plusVideoRTPSource::H263plusVideoRTPSource(UsageEnvironment& env,
		CommonPlay *cpObj, Groupsock* RTPgs, unsigned char rtpPayloadFormat,
		unsigned rtpTimestampFrequency) :
		MultiFramedRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
				rtpTimestampFrequency), fNumSpecialHeaders(0), fSpecialHeaderBytesLength(
				0) {
}

H263plusVideoRTPSource::~H263plusVideoRTPSource() {
}

Boolean H263plusVideoRTPSource::processSpecialHeader(BufferedPacket* packet,
		unsigned& resultSpecialHeaderSize) {
	unsigned char* headerStart = packet->data();
	unsigned packetSize = packet->dataSize();

	// The H.263+ payload header is at least 2 bytes in size.
	// Extract the known fields from the first 2 bytes:
	unsigned expectedHeaderSize = 2;
	if (packetSize < expectedHeaderSize)
		return False;

	//unsigned char RR = headerStart[0]>>3;
	Boolean P = (headerStart[0] & 0x4) != 0;
	Boolean V = (headerStart[0] & 0x2) != 0;
	unsigned char PLEN = ((headerStart[0] & 0x1) << 5) | (headerStart[1] >> 3);
	//unsigned char PEBIT = headerStart[1]&0x7;

	if (V) {
		// There's an extra VRC byte at the end of the header:
		++expectedHeaderSize;
		if (packetSize < expectedHeaderSize)
			return False;
	}

	if (PLEN > 0) {
		// There's an extra picture header at the end:
		expectedHeaderSize += PLEN;
		if (packetSize < expectedHeaderSize)
			return False;
	}

	fCurrentPacketBeginsFrame = P;
	if (fCurrentPacketBeginsFrame) {
		fNumSpecialHeaders = fSpecialHeaderBytesLength = 0;
	}

	// Make a copy of the special header bytes, in case a reader
	// can use them:
	unsigned bytesAvailable = SPECIAL_HEADER_BUFFER_SIZE
			- fSpecialHeaderBytesLength - 1;
	if (expectedHeaderSize <= bytesAvailable) {
		fSpecialHeaderBytes[fSpecialHeaderBytesLength++] = expectedHeaderSize;
		for (unsigned i = 0; i < expectedHeaderSize; ++i) {
			fSpecialHeaderBytes[fSpecialHeaderBytesLength++] = headerStart[i];
		}
		fPacketSizes[fNumSpecialHeaders++] = packetSize;
	}

	if (P) {
		// Prepend two zero bytes to the start of the payload proper.
		// Hack: Do this by shrinking this special header by 2 bytes:
		expectedHeaderSize -= 2;
		headerStart[expectedHeaderSize] = 0;
		headerStart[expectedHeaderSize + 1] = 0;
	}

	// The RTP "M" (marker) bit indicates the last fragment of a frame:
	fCurrentPacketCompletesFrame = packet->rtpMarkerBit();

	resultSpecialHeaderSize = expectedHeaderSize;
	return True;
}

char const* H263plusVideoRTPSource::MIMEtype() const {
	return "video/H263-1998";
}

