/*
 * H264VideoRTPSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "H264VideoRTPSource.h"
#include "Base64.h"
#include "strDup.h"

////////// H264BufferedPacket and H264BufferedPacketFactory //////////

class H264BufferedPacket: public BufferedPacket {
public:
	H264BufferedPacket(H264VideoRTPSource& ourSource);
	virtual ~H264BufferedPacket();

private:
	// redefined virtual functions
	virtual unsigned nextEnclosedFrameSize(DP_U8*& framePtr,
			unsigned dataSize);
private:
	H264VideoRTPSource& fOurSource;
};

class H264BufferedPacketFactory: public BufferedPacketFactory {
private:
	// redefined virtual functions
	virtual BufferedPacket* createNewPacket(MultiFramedRTPSource* ourSource);
};

///////// H264VideoRTPSource implementation ////////

H264VideoRTPSource*
H264VideoRTPSource::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
		unsigned rtpTimestampFrequency) {
	return new H264VideoRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency);
}

H264VideoRTPSource::H264VideoRTPSource(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
		unsigned rtpTimestampFrequency) :
		MultiFramedRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
				rtpTimestampFrequency, new H264BufferedPacketFactory), fCurPacketNALUnitType(
				0) {
}

H264VideoRTPSource::~H264VideoRTPSource() {
}

Boolean H264VideoRTPSource::processSpecialHeader(BufferedPacket* packet,
		unsigned& resultSpecialHeaderSize) {
	DP_U8* headerStart = packet->data();
	unsigned packetSize = packet->dataSize();
	unsigned numBytesToSkip;

	// Check the 'nal_unit_type' for special 'aggregation' or 'fragmentation' packets:
	if (packetSize < 1)
		return False;
	fCurPacketNALUnitType = (headerStart[0] & 0x1F);
	switch (fCurPacketNALUnitType) {
	case 24: { // STAP-A
		numBytesToSkip = 1; // discard the type byte
		break;
	}
	case 25:
	case 26:
	case 27: { // STAP-B, MTAP16, or MTAP24
		numBytesToSkip = 3; // discard the type byte, and the initial DON
		break;
	}
	case 28:
	case 29: { // // FU-A or FU-B
		// For these NALUs, the first two bytes are the FU indicator and the FU header.
		// If the start bit is set, we reconstruct the original NAL header into byte 1:
		if (packetSize < 2)
			return False;
		DP_U8 startBit = headerStart[1] & 0x80;
		DP_U8 endBit = headerStart[1] & 0x40;
		if (startBit) {
			fCurrentPacketBeginsFrame = True;

			headerStart[1] = (headerStart[0] & 0xE0) | (headerStart[1] & 0x1F);
			numBytesToSkip = 1;
		} else {
			// The start bit is not set, so we skip both the FU indicator and header:
			fCurrentPacketBeginsFrame = False;
			numBytesToSkip = 2;
		}
		fCurrentPacketCompletesFrame = (endBit != 0);
		break;
	}
	default: {
		// This packet contains one complete NAL unit:
		fCurrentPacketBeginsFrame = fCurrentPacketCompletesFrame = True;
		numBytesToSkip = 0;
		break;
	}
	}

	resultSpecialHeaderSize = numBytesToSkip;
	return True;
}

char const* H264VideoRTPSource::MIMEtype() const {
	return "video/H264";
}

SPropRecord* parseSPropParameterSets(char const* sPropParameterSetsStr,
// result parameter:
		unsigned& numSPropRecords) {
	// Make a copy of the input string, so we can replace the commas with '\0's:
	char* inStr = strDup(sPropParameterSetsStr);
	if (inStr == NULL) {
		numSPropRecords = 0;
		return NULL;
	}

	// Count the number of commas (and thus the number of parameter sets):
	numSPropRecords = 1;
	char* s;
	for (s = inStr; *s != '\0'; ++s) {
		if (*s == ',') {
			++numSPropRecords;
			*s = '\0';
		}
	}

	// Allocate and fill in the result array:
	SPropRecord* resultArray = new SPropRecord[numSPropRecords];
	s = inStr;
	for (unsigned i = 0; i < numSPropRecords; ++i) {
		resultArray[i].sPropBytes = base64Decode(s, resultArray[i].sPropLength);
		s += strlen(s) + 1;
	}

	delete[] inStr;
	return resultArray;
}

////////// H264BufferedPacket and H264BufferedPacketFactory implementation //////////

H264BufferedPacket::H264BufferedPacket(H264VideoRTPSource& ourSource) :
		fOurSource(ourSource) {
}

H264BufferedPacket::~H264BufferedPacket() {
}

unsigned H264BufferedPacket::nextEnclosedFrameSize(DP_U8*& framePtr,
		unsigned dataSize) {
	unsigned resultNALUSize = 0; // if an error occurs

	switch (fOurSource.fCurPacketNALUnitType) {
	case 24:
	case 25: { // STAP-A or STAP-B
		// The first two bytes are NALU size:
		if (dataSize < 2)
			break;
		resultNALUSize = (framePtr[0] << 8) | framePtr[1];
		framePtr += 2;
		break;
	}
	case 26: { // MTAP16
		// The first two bytes are NALU size.  The next three are the DOND and TS offset:
		if (dataSize < 5)
			break;
		resultNALUSize = (framePtr[0] << 8) | framePtr[1];
		framePtr += 5;
		break;
	}
	case 27: { // MTAP24
		// The first two bytes are NALU size.  The next four are the DOND and TS offset:
		if (dataSize < 6)
			break;
		resultNALUSize = (framePtr[0] << 8) | framePtr[1];
		framePtr += 6;
		break;
	}
	default: {
		// Common case: We use the entire packet data:
		return dataSize;
	}
	}

	return (resultNALUSize <= dataSize) ? resultNALUSize : dataSize;
}

BufferedPacket* H264BufferedPacketFactory::createNewPacket(
		MultiFramedRTPSource* ourSource) {
	return new H264BufferedPacket((H264VideoRTPSource&) (*ourSource));
}

