/*
 * TheoraVideoRTPSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "TheoraVideoRTPSource.h"

////////// TheoraBufferedPacket and TheoraBufferedPacketFactory //////////

class TheoraBufferedPacket: public BufferedPacket {
public:
	TheoraBufferedPacket();
	virtual ~TheoraBufferedPacket();

private:
	// redefined virtual functions
	virtual unsigned nextEnclosedFrameSize(unsigned char*& framePtr,
			unsigned dataSize);
};

class TheoraBufferedPacketFactory: public BufferedPacketFactory {
private:
	// redefined virtual functions
	virtual BufferedPacket* createNewPacket(MultiFramedRTPSource* ourSource);
};

///////// TheoraVideoRTPSource implementation ////////

TheoraVideoRTPSource*
TheoraVideoRTPSource::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, unsigned char rtpPayloadFormat) {
	return new TheoraVideoRTPSource(env, cpObj, RTPgs, rtpPayloadFormat);
}

TheoraVideoRTPSource::TheoraVideoRTPSource(UsageEnvironment& env,
		CommonPlay *cpObj, Groupsock* RTPgs, unsigned char rtpPayloadFormat) :
		MultiFramedRTPSource(env, cpObj, RTPgs, rtpPayloadFormat, 90000,
				new TheoraBufferedPacketFactory), fCurPacketIdent(0) {
}

TheoraVideoRTPSource::~TheoraVideoRTPSource() {
}

Boolean TheoraVideoRTPSource::processSpecialHeader(BufferedPacket* packet,
		unsigned& resultSpecialHeaderSize) {
	unsigned char* headerStart = packet->data();
	unsigned packetSize = packet->dataSize();

	resultSpecialHeaderSize = 4;
	if (packetSize < resultSpecialHeaderSize)
		return False; // packet was too small

	// The first 3 bytes of the header are the "Ident" field:
	fCurPacketIdent = (headerStart[0] << 16) | (headerStart[1] << 8)
			| headerStart[2];

	// The 4th byte is F|TDT|numPkts.
	// Reject any packet with TDT == 3:
	if ((headerStart[3] & 0x30) == 0x30)
		return False;

	u_int8_t F = headerStart[3] >> 6;
	fCurrentPacketBeginsFrame = F <= 1; // "Not Fragmented" or "Start Fragment"
	fCurrentPacketCompletesFrame = F == 0 || F == 3; // "Not Fragmented" or "End Fragment"

	return True;
}

char const* TheoraVideoRTPSource::MIMEtype() const {
	return "video/THEORA";
}

////////// TheoraBufferedPacket and TheoraBufferedPacketFactory implementation //////////

TheoraBufferedPacket::TheoraBufferedPacket() {
}

TheoraBufferedPacket::~TheoraBufferedPacket() {
}

unsigned TheoraBufferedPacket::nextEnclosedFrameSize(unsigned char*& framePtr,
		unsigned dataSize) {
	if (dataSize < 2) {
		// There's not enough space for a 2-byte header.  TARFU!  Just return the data that's left:
		return dataSize;
	}

	unsigned frameSize = (framePtr[0] << 8) | framePtr[1];
	framePtr += 2;
	if (frameSize > dataSize - 2)
		return dataSize - 2; // inconsistent frame size => just return all the data that's left

	return frameSize;
}

BufferedPacket* TheoraBufferedPacketFactory::createNewPacket(
		MultiFramedRTPSource* /*ourSource*/) {
	return new TheoraBufferedPacket();
}

