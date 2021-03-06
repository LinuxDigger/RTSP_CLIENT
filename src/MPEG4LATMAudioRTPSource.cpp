/*
 * MPEG4LATMAudioRTPSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "MPEG4LATMAudioRTPSource.h"

////////// LATMBufferedPacket and LATMBufferedPacketFactory //////////

class LATMBufferedPacket: public BufferedPacket {
public:
	LATMBufferedPacket(Boolean includeLATMDataLengthField);
	virtual ~LATMBufferedPacket();

private:
	// redefined virtual functions
	virtual unsigned nextEnclosedFrameSize(DP_U8*& framePtr,
			unsigned dataSize);

private:
	Boolean fIncludeLATMDataLengthField;
};

class LATMBufferedPacketFactory: public BufferedPacketFactory {
private:
	// redefined virtual functions
	virtual BufferedPacket* createNewPacket(MultiFramedRTPSource* ourSource);
};

///////// MPEG4LATMAudioRTPSource implementation ////////

MPEG4LATMAudioRTPSource*
MPEG4LATMAudioRTPSource::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
		unsigned rtpTimestampFrequency) {
	return new MPEG4LATMAudioRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency);
}

MPEG4LATMAudioRTPSource::MPEG4LATMAudioRTPSource(UsageEnvironment& env,
		CommonPlay *cpObj, Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
		unsigned rtpTimestampFrequency) :
		MultiFramedRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
				rtpTimestampFrequency, new LATMBufferedPacketFactory), fIncludeLATMDataLengthField(
				True) {
}

MPEG4LATMAudioRTPSource::~MPEG4LATMAudioRTPSource() {
}

void MPEG4LATMAudioRTPSource::omitLATMDataLengthField() {
	fIncludeLATMDataLengthField = False;
}

Boolean MPEG4LATMAudioRTPSource::processSpecialHeader(BufferedPacket* packet,
		unsigned& resultSpecialHeaderSize) {
	fCurrentPacketBeginsFrame = fCurrentPacketCompletesFrame;
	// whether the *previous* packet ended a frame

	// The RTP "M" (marker) bit indicates the last fragment of a frame:
	fCurrentPacketCompletesFrame = packet->rtpMarkerBit();

	// There is no special header
	resultSpecialHeaderSize = 0;
	return True;
}

char const* MPEG4LATMAudioRTPSource::MIMEtype() const {
	return "audio/MP4A-LATM";
}

////////// LATMBufferedPacket and LATMBufferedPacketFactory implementation

LATMBufferedPacket::LATMBufferedPacket(Boolean includeLATMDataLengthField) :
		fIncludeLATMDataLengthField(includeLATMDataLengthField) {
}

LATMBufferedPacket::~LATMBufferedPacket() {
}

unsigned LATMBufferedPacket::nextEnclosedFrameSize(DP_U8*& framePtr,
		unsigned dataSize) {
	// Look at the LATM data length byte(s), to determine the size
	// of the LATM payload.
	unsigned resultFrameSize = 0;
	unsigned i;
	for (i = 0; i < dataSize; ++i) {
		resultFrameSize += framePtr[i];
		if (framePtr[i] != 0xFF)
			break;
	}
	++i;
	if (fIncludeLATMDataLengthField) {
		resultFrameSize += i;
	} else {
		framePtr += i;
		dataSize -= i;
	}

	return (resultFrameSize <= dataSize) ? resultFrameSize : dataSize;
}

BufferedPacket* LATMBufferedPacketFactory::createNewPacket(
		MultiFramedRTPSource* ourSource) {
	MPEG4LATMAudioRTPSource* source = (MPEG4LATMAudioRTPSource*) ourSource;
	return new LATMBufferedPacket(
			source->returnedFrameIncludesLATMDataLengthField());
}

////////// parseStreamMuxConfigStr() implementation //////////

static Boolean getNibble(char const*& configStr, DP_U8& resultNibble) {
	char c = configStr[0];
	if (c == '\0')
		return False; // we've reached the end

	if (c >= '0' && c <= '9') {
		resultNibble = c - '0';
	} else if (c >= 'A' && c <= 'F') {
		resultNibble = 10 + c - 'A';
	} else if (c >= 'a' && c <= 'f') {
		resultNibble = 10 + c - 'a';
	} else {
		return False;
	}

	++configStr; // move to the next nibble
	return True;
}

static Boolean getByte(char const*& configStr, DP_U8& resultByte) {
	resultByte = 0; // by default, in case parsing fails

	DP_U8 firstNibble;
	if (!getNibble(configStr, firstNibble))
		return False;
	resultByte = firstNibble << 4;

	DP_U8 secondNibble = 0;
	if (!getNibble(configStr, secondNibble) && configStr[0] != '\0') {
		// There's a second nibble, but it's malformed
		return False;
	}
	resultByte |= secondNibble;

	return True;
}

Boolean parseStreamMuxConfigStr(char const* configStr,
		// result parameters:
		Boolean& audioMuxVersion, Boolean& allStreamsSameTimeFraming,
		DP_U8& numSubFrames, DP_U8& numProgram,
		DP_U8& numLayer, DP_U8*& audioSpecificConfig,
		unsigned& audioSpecificConfigSize) {
	// Set default versions of the result parameters:
	audioMuxVersion = False;
	allStreamsSameTimeFraming = True;
	numSubFrames = numProgram = numLayer = 0;
	audioSpecificConfig = NULL;
	audioSpecificConfigSize = 0;

	do {
		if (configStr == NULL)
			break;

		DP_U8 nextByte;

		if (!getByte(configStr, nextByte))
			break;
		audioMuxVersion = (nextByte & 0x80) != 0;
		if (audioMuxVersion)
			break;

		allStreamsSameTimeFraming = ((nextByte & 0x40) >> 6) != 0;
		numSubFrames = (nextByte & 0x3F);

		if (!getByte(configStr, nextByte))
			break;
		numProgram = (nextByte & 0xF0) >> 4;

		numLayer = (nextByte & 0x0E) >> 1;

		// The one remaining bit, and the rest of the string,
		// are used for "audioSpecificConfig":
		DP_U8 remainingBit = nextByte & 1;

		unsigned ascSize = (strlen(configStr) + 1) / 2 + 1;
		audioSpecificConfig = new DP_U8[ascSize];

		Boolean parseSuccess;
		unsigned i = 0;
		do {
			nextByte = 0;
			parseSuccess = getByte(configStr, nextByte);
			audioSpecificConfig[i++] = (remainingBit << 7)
					| ((nextByte & 0xFE) >> 1);
			remainingBit = nextByte & 1;
		} while (parseSuccess);
		if (i != ascSize)
			break; // part of the remaining string was bad

		audioSpecificConfigSize = ascSize;
		return True; // parsing succeeded
	} while (0);

	delete[] audioSpecificConfig;
	return False; // parsing failed
}

DP_U8* parseStreamMuxConfigStr(char const* configStr,
// result parameter:
		unsigned& audioSpecificConfigSize) {
	Boolean audioMuxVersion, allStreamsSameTimeFraming;
	DP_U8 numSubFrames, numProgram, numLayer;
	DP_U8* audioSpecificConfig;

	if (!parseStreamMuxConfigStr(configStr, audioMuxVersion,
			allStreamsSameTimeFraming, numSubFrames, numProgram, numLayer,
			audioSpecificConfig, audioSpecificConfigSize)) {
		audioSpecificConfigSize = 0;
		return NULL;
	}

	return audioSpecificConfig;
}

DP_U8* parseGeneralConfigStr(char const* configStr,
// result parameter:
		unsigned& configSize) {
	DP_U8* config = NULL;
	do {
		if (configStr == NULL)
			break;
		configSize = (strlen(configStr) + 1) / 2;

		config = new DP_U8[configSize];
		if (config == NULL)
			break;

		unsigned i;
		for (i = 0; i < configSize; ++i) {
			if (!getByte(configStr, config[i]))
				break;
		}
		if (i != configSize)
			break; // part of the string was bad

		return config;
	} while (0);

	configSize = 0;
	delete[] config;
	return NULL;
}

