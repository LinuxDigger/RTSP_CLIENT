/*
 * MP3ADURTPSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "MP3ADURTPSource.h"
#include "MP3ADUdescriptor.h"

////////// ADUBufferedPacket and ADUBufferedPacketFactory //////////

class ADUBufferedPacket: public BufferedPacket {
private:
	// redefined virtual functions
	virtual unsigned nextEnclosedFrameSize(unsigned char*& framePtr,
			unsigned dataSize);
};

class ADUBufferedPacketFactory: public BufferedPacketFactory {
private:
	// redefined virtual functions
	virtual BufferedPacket* createNewPacket(MultiFramedRTPSource* ourSource);
};

///////// MP3ADURTPSource implementation ////////

MP3ADURTPSource*
MP3ADURTPSource::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, unsigned char rtpPayloadFormat,
		unsigned rtpTimestampFrequency) {
	return new MP3ADURTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency);
}

MP3ADURTPSource::MP3ADURTPSource(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, unsigned char rtpPayloadFormat,
		unsigned rtpTimestampFrequency) :
		MultiFramedRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
				rtpTimestampFrequency, new ADUBufferedPacketFactory) {
}

MP3ADURTPSource::~MP3ADURTPSource() {
}

char const* MP3ADURTPSource::MIMEtype() const {
	return "audio/MPA-ROBUST";
}

////////// ADUBufferedPacket and ADUBufferredPacketFactory implementation

unsigned ADUBufferedPacket::nextEnclosedFrameSize(unsigned char*& framePtr,
		unsigned dataSize) {
	// Return the size of the next MP3 'ADU', on the assumption that
	// the input data is ADU-encoded MP3 frames.
	unsigned char* frameDataPtr = framePtr;
	unsigned remainingFrameSize = ADUdescriptor::getRemainingFrameSize(
			frameDataPtr);
	unsigned descriptorSize = (unsigned) (frameDataPtr - framePtr);
	unsigned fullADUSize = descriptorSize + remainingFrameSize;

	return (fullADUSize <= dataSize) ? fullADUSize : dataSize;
}

BufferedPacket* ADUBufferedPacketFactory::createNewPacket(
		MultiFramedRTPSource* /*ourSource*/) {
	return new ADUBufferedPacket;
}

