/*
 * QCELPAudioRTPSource.cpp
 *
 *  Created on: Jun 12, 2018
 *      Author: jhb
 */

#include "QCELPAudioRTPSource.h"
#include "MultiFramedRTPSource.h"
#include "FramedFilter.h"
#include <string.h>
#include <stdlib.h>

// This source is implemented internally by two separate sources:
// (i) a RTP source for the raw (interleaved) QCELP frames, and
// (ii) a deinterleaving filter that reads from this.
// Define these two new classes here:

class RawQCELPRTPSource: public MultiFramedRTPSource {
public:
	static RawQCELPRTPSource* createNew(UsageEnvironment& env,
			CommonPlay *cpObj, Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
			unsigned rtpTimestampFrequency);

	DP_U8 interleaveL() const {
		return fInterleaveL;
	}
	DP_U8 interleaveN() const {
		return fInterleaveN;
	}
	DP_U8& frameIndex() {
		return fFrameIndex;
	} // index within pkt

private:
	RawQCELPRTPSource(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
			unsigned rtpTimestampFrequency);
	// called only by createNew()

	virtual ~RawQCELPRTPSource();

private:
	// redefined virtual functions:
	virtual Boolean processSpecialHeader(BufferedPacket* packet,
			unsigned& resultSpecialHeaderSize);
	virtual char const* MIMEtype() const;

	virtual Boolean hasBeenSynchronizedUsingRTCP();

private:
	DP_U8 fInterleaveL, fInterleaveN, fFrameIndex;
	unsigned fNumSuccessiveSyncedPackets;
};

class QCELPDeinterleaver: public FramedFilter {
public:
	static QCELPDeinterleaver* createNew(UsageEnvironment& env,
			CommonPlay *cpObj, RawQCELPRTPSource* inputSource);

private:
	QCELPDeinterleaver(UsageEnvironment& env, CommonPlay *cpObj,
			RawQCELPRTPSource* inputSource);
	// called only by "createNew()"

	virtual ~QCELPDeinterleaver();

	static void afterGettingFrame(void* clientData, unsigned frameSize,
			unsigned numTruncatedBytes, struct timeval presentationTime,
			unsigned durationInMicroseconds);
	void afterGettingFrame1(unsigned frameSize,
			struct timeval presentationTime);

private:
	// Redefined virtual functions:
	void doGetNextFrame();
	virtual void doStopGettingFrames();

private:
	class QCELPDeinterleavingBuffer* fDeinterleavingBuffer;
	Boolean fNeedAFrame;
};

////////// QCELPAudioRTPSource implementation //////////

FramedSource*
QCELPAudioRTPSource::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, RTPSource*& resultRTPSource,
		DP_U8 rtpPayloadFormat, unsigned rtpTimestampFrequency) {
	RawQCELPRTPSource* rawRTPSource;
	resultRTPSource = rawRTPSource = RawQCELPRTPSource::createNew(env, cpObj,
			RTPgs, rtpPayloadFormat, rtpTimestampFrequency);
	if (resultRTPSource == NULL)
		return NULL;

	QCELPDeinterleaver* deinterleaver = QCELPDeinterleaver::createNew(env,
			cpObj, rawRTPSource);
	if (deinterleaver == NULL) {
		Medium::close(resultRTPSource);
		resultRTPSource = NULL;
	}

	return deinterleaver;
}

////////// QCELPBufferedPacket and QCELPBufferedPacketFactory //////////

// A subclass of BufferedPacket, used to separate out QCELP frames.

class QCELPBufferedPacket: public BufferedPacket {
public:
	QCELPBufferedPacket(RawQCELPRTPSource& ourSource);
	virtual ~QCELPBufferedPacket();

private:
	// redefined virtual functions
	virtual unsigned nextEnclosedFrameSize(DP_U8*& framePtr,
			unsigned dataSize);
private:
	RawQCELPRTPSource& fOurSource;
};

class QCELPBufferedPacketFactory: public BufferedPacketFactory {
private:
	// redefined virtual functions
	virtual BufferedPacket* createNewPacket(MultiFramedRTPSource* ourSource);
};

///////// RawQCELPRTPSource implementation ////////

RawQCELPRTPSource*
RawQCELPRTPSource::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
		unsigned rtpTimestampFrequency) {
	return new RawQCELPRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
			rtpTimestampFrequency);
}

RawQCELPRTPSource::RawQCELPRTPSource(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
		unsigned rtpTimestampFrequency) :
		MultiFramedRTPSource(env, cpObj, RTPgs, rtpPayloadFormat,
				rtpTimestampFrequency, new QCELPBufferedPacketFactory), fInterleaveL(
				0), fInterleaveN(0), fFrameIndex(0), fNumSuccessiveSyncedPackets(
				0) {
}

RawQCELPRTPSource::~RawQCELPRTPSource() {
}

Boolean RawQCELPRTPSource::processSpecialHeader(BufferedPacket* packet,
		unsigned& resultSpecialHeaderSize) {
	DP_U8* headerStart = packet->data();
	unsigned packetSize = packet->dataSize();

	// First, check whether this packet's RTP timestamp is synchronized:
	if (RTPSource::hasBeenSynchronizedUsingRTCP()) {
		++fNumSuccessiveSyncedPackets;
	} else {
		fNumSuccessiveSyncedPackets = 0;
	}

	// There's a 1-byte header indicating the interleave parameters
	if (packetSize < 1)
		return False;

	// Get the interleaving parameters from the 1-byte header,
	// and check them for validity:
	DP_U8 const firstByte = headerStart[0];
	DP_U8 const interleaveL = (firstByte & 0x38) >> 3;
	DP_U8 const interleaveN = firstByte & 0x07;
#ifdef DEBUG
	fprintf(stderr, "packetSize: %d, interleaveL: %d, interleaveN: %d\n", packetSize, interleaveL, interleaveN);
#endif
	if (interleaveL > 5 || interleaveN > interleaveL)
		return False; //invalid

	fInterleaveL = interleaveL;
	fInterleaveN = interleaveN;
	fFrameIndex = 0; // initially

	resultSpecialHeaderSize = 1;
	return True;
}

char const* RawQCELPRTPSource::MIMEtype() const {
	return "audio/QCELP";
}

Boolean RawQCELPRTPSource::hasBeenSynchronizedUsingRTCP() {
	// Don't report ourselves as being synchronized until we've received
	// at least a complete interleave cycle of synchronized packets.
	// This ensures that the receiver is currently getting a frame from
	// a packet that was synchronized.
	if (fNumSuccessiveSyncedPackets > (unsigned) (fInterleaveL + 1)) {
		fNumSuccessiveSyncedPackets = fInterleaveL + 2; // prevents overflow
		return True;
	}
	return False;
}

///// QCELPBufferedPacket and QCELPBufferedPacketFactory implementation

QCELPBufferedPacket::QCELPBufferedPacket(RawQCELPRTPSource& ourSource) :
		fOurSource(ourSource) {
}

QCELPBufferedPacket::~QCELPBufferedPacket() {
}

unsigned QCELPBufferedPacket::nextEnclosedFrameSize(DP_U8*& framePtr,
		unsigned dataSize) {
	// The size of the QCELP frame is determined by the first byte:
	if (dataSize == 0)
		return 0; // sanity check
	DP_U8 const firstByte = framePtr[0];

	unsigned frameSize;
	switch (firstByte) {
	case 0: {
		frameSize = 1;
		break;
	}
	case 1: {
		frameSize = 4;
		break;
	}
	case 2: {
		frameSize = 8;
		break;
	}
	case 3: {
		frameSize = 17;
		break;
	}
	case 4: {
		frameSize = 35;
		break;
	}
	default: {
		frameSize = 0;
		break;
	}
	}

#ifdef DEBUG
	fprintf(stderr, "QCELPBufferedPacket::nextEnclosedFrameSize(): frameSize: %d, dataSize: %d\n", frameSize, dataSize);
#endif
	if (dataSize < frameSize)
		return 0;

	++fOurSource.frameIndex();
	return frameSize;
}

BufferedPacket* QCELPBufferedPacketFactory::createNewPacket(
		MultiFramedRTPSource* ourSource) {
	return new QCELPBufferedPacket((RawQCELPRTPSource&) (*ourSource));
}

///////// QCELPDeinterleavingBuffer /////////
// (used to implement QCELPDeinterleaver)

#define QCELP_MAX_FRAME_SIZE 35
#define QCELP_MAX_INTERLEAVE_L 5
#define QCELP_MAX_FRAMES_PER_PACKET 10
#define QCELP_MAX_INTERLEAVE_GROUP_SIZE \
    ((QCELP_MAX_INTERLEAVE_L+1)*QCELP_MAX_FRAMES_PER_PACKET)

class QCELPDeinterleavingBuffer {
public:
	QCELPDeinterleavingBuffer();
	virtual ~QCELPDeinterleavingBuffer();

	void deliverIncomingFrame(unsigned frameSize, DP_U8 interleaveL,
			DP_U8 interleaveN, DP_U8 frameIndex,
			unsigned short packetSeqNum, struct timeval presentationTime);
	Boolean retrieveFrame(DP_U8* to, unsigned maxSize,
			unsigned& resultFrameSize, unsigned& resultNumTruncatedBytes,
			struct timeval& resultPresentationTime);

	DP_U8* inputBuffer() {
		return fInputBuffer;
	}
	unsigned inputBufferSize() const {
		return QCELP_MAX_FRAME_SIZE;
	}

private:
	class FrameDescriptor {
	public:
		FrameDescriptor();
		virtual ~FrameDescriptor();

		unsigned frameSize;
		DP_U8* frameData;
		struct timeval presentationTime;
	};

	// Use two banks of descriptors - one for incoming, one for outgoing
	FrameDescriptor fFrames[QCELP_MAX_INTERLEAVE_GROUP_SIZE][2];
	DP_U8 fIncomingBankId; // toggles between 0 and 1
	DP_U8 fIncomingBinMax; // in the incoming bank
	DP_U8 fOutgoingBinMax; // in the outgoing bank
	DP_U8 fNextOutgoingBin;
	Boolean fHaveSeenPackets;
	u_int16_t fLastPacketSeqNumForGroup;
	DP_U8* fInputBuffer;
	struct timeval fLastRetrievedPresentationTime;
};

////////// QCELPDeinterleaver implementation /////////

QCELPDeinterleaver*
QCELPDeinterleaver::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		RawQCELPRTPSource* inputSource) {
	return new QCELPDeinterleaver(env, cpObj, inputSource);
}

QCELPDeinterleaver::QCELPDeinterleaver(UsageEnvironment& env, CommonPlay *cpObj,
		RawQCELPRTPSource* inputSource) :
		FramedFilter(env, cpObj, inputSource), fNeedAFrame(False) {
	fDeinterleavingBuffer = new QCELPDeinterleavingBuffer();
}

QCELPDeinterleaver::~QCELPDeinterleaver() {
	delete fDeinterleavingBuffer;
}

static unsigned const uSecsPerFrame = 20000; // 20 ms

void QCELPDeinterleaver::doGetNextFrame() {
	// First, try getting a frame from the deinterleaving buffer:
	if (fDeinterleavingBuffer->retrieveFrame(fTo, fMaxSize, fFrameSize,
			fNumTruncatedBytes, fPresentationTime)) {
		// Success!
		fNeedAFrame = False;

		fDurationInMicroseconds = uSecsPerFrame;

		// Call our own 'after getting' function.  Because we're not a 'leaf'
		// source, we can call this directly, without risking
		// infinite recursion
		afterGetting(this);
		return;
	}

	// No luck, so ask our source for help:
	fNeedAFrame = True;
	if (!fInputSource->isCurrentlyAwaitingData()) {
		fInputSource->getNextFrame(fDeinterleavingBuffer->inputBuffer(),
				fDeinterleavingBuffer->inputBufferSize(), afterGettingFrame,
				this, FramedSource::handleClosure, this);
	}
}

void QCELPDeinterleaver::doStopGettingFrames() {
	fNeedAFrame = False;
	fInputSource->stopGettingFrames();
}

void QCELPDeinterleaver::afterGettingFrame(void* clientData, unsigned frameSize,
		unsigned /*numTruncatedBytes*/, struct timeval presentationTime,
		unsigned /*durationInMicroseconds*/) {
	QCELPDeinterleaver* deinterleaver = (QCELPDeinterleaver*) clientData;
	deinterleaver->afterGettingFrame1(frameSize, presentationTime);
}

void QCELPDeinterleaver::afterGettingFrame1(unsigned frameSize,
		struct timeval presentationTime) {
	RawQCELPRTPSource* source = (RawQCELPRTPSource*) fInputSource;

	// First, put the frame into our deinterleaving buffer:
	fDeinterleavingBuffer->deliverIncomingFrame(frameSize,
			source->interleaveL(), source->interleaveN(), source->frameIndex(),
			source->curPacketRTPSeqNum(), presentationTime);

	// Then, try delivering a frame to the client (if he wants one):
	if (fNeedAFrame)
		doGetNextFrame();
}

////////// QCELPDeinterleavingBuffer implementation /////////

QCELPDeinterleavingBuffer::QCELPDeinterleavingBuffer() :
		fIncomingBankId(0), fIncomingBinMax(0), fOutgoingBinMax(0), fNextOutgoingBin(
				0), fHaveSeenPackets(False), fLastPacketSeqNumForGroup(0), fLastRetrievedPresentationTime() {
	fInputBuffer = new DP_U8[QCELP_MAX_FRAME_SIZE];
}

QCELPDeinterleavingBuffer::~QCELPDeinterleavingBuffer() {
	delete[] fInputBuffer;
}

void QCELPDeinterleavingBuffer::deliverIncomingFrame(unsigned frameSize,
		DP_U8 interleaveL, DP_U8 interleaveN,
		DP_U8 frameIndex, unsigned short packetSeqNum,
		struct timeval presentationTime) {
// First perform a sanity check on the parameters:
// (This is overkill, as the source should have already done this.)
	if (frameSize > QCELP_MAX_FRAME_SIZE || interleaveL > QCELP_MAX_INTERLEAVE_L
			|| interleaveN
					> interleaveL|| frameIndex == 0 || frameIndex > QCELP_MAX_FRAMES_PER_PACKET) {
#ifdef DEBUG
		fprintf(stderr, "QCELPDeinterleavingBuffer::deliverIncomingFrame() param sanity check failed (%d,%d,%d,%d)\n", frameSize, interleaveL, interleaveN, frameIndex);
#endif
		return;
	}

// The input "presentationTime" was that of the first frame in this
// packet.  Update it for the current frame:
	unsigned uSecIncrement = (frameIndex - 1) * (interleaveL + 1)
			* uSecsPerFrame;
	presentationTime.tv_usec += uSecIncrement;
	presentationTime.tv_sec += presentationTime.tv_usec / 1000000;
	presentationTime.tv_usec = presentationTime.tv_usec % 1000000;

// Next, check whether this packet is part of a new interleave group
	if (!fHaveSeenPackets
			|| seqNumLT(fLastPacketSeqNumForGroup, packetSeqNum)) {
		// We've moved to a new interleave group
		fHaveSeenPackets = True;
		fLastPacketSeqNumForGroup = packetSeqNum + interleaveL - interleaveN;

		// Switch the incoming and outgoing banks:
		fIncomingBankId ^= 1;
		DP_U8 tmp = fIncomingBinMax;
		fIncomingBinMax = fOutgoingBinMax;
		fOutgoingBinMax = tmp;
		fNextOutgoingBin = 0;
	}

// Now move the incoming frame into the appropriate bin:
	unsigned const binNumber = interleaveN
			+ (frameIndex - 1) * (interleaveL + 1);
	FrameDescriptor& inBin = fFrames[binNumber][fIncomingBankId];
	DP_U8* curBuffer = inBin.frameData;
	inBin.frameData = fInputBuffer;
	inBin.frameSize = frameSize;
	inBin.presentationTime = presentationTime;

	if (curBuffer == NULL)
		curBuffer = new DP_U8[QCELP_MAX_FRAME_SIZE];
	fInputBuffer = curBuffer;

	if (binNumber >= fIncomingBinMax) {
		fIncomingBinMax = binNumber + 1;
	}
}

Boolean QCELPDeinterleavingBuffer::retrieveFrame(DP_U8* to,
		unsigned maxSize, unsigned& resultFrameSize,
		unsigned& resultNumTruncatedBytes,
		struct timeval& resultPresentationTime) {
	if (fNextOutgoingBin >= fOutgoingBinMax)
		return False; // none left

	FrameDescriptor& outBin = fFrames[fNextOutgoingBin][fIncomingBankId ^ 1];
	DP_U8* fromPtr;
	DP_U8 fromSize = outBin.frameSize;
	outBin.frameSize = 0; // for the next time this bin is used

// Check whether this frame is missing; if so, return an 'erasure' frame:
	DP_U8 erasure = 14;
	if (fromSize == 0) {
		fromPtr = &erasure;
		fromSize = 1;

		// Compute this erasure frame's presentation time via extrapolation:
		resultPresentationTime = fLastRetrievedPresentationTime;
		resultPresentationTime.tv_usec += uSecsPerFrame;
		if (resultPresentationTime.tv_usec >= 1000000) {
			++resultPresentationTime.tv_sec;
			resultPresentationTime.tv_usec -= 1000000;
		}
	} else {
		// Normal case - a frame exists:
		fromPtr = outBin.frameData;
		resultPresentationTime = outBin.presentationTime;
	}

	fLastRetrievedPresentationTime = resultPresentationTime;

	if (fromSize > maxSize) {
		resultNumTruncatedBytes = fromSize - maxSize;
		resultFrameSize = maxSize;
	} else {
		resultNumTruncatedBytes = 0;
		resultFrameSize = fromSize;
	}
	memmove(to, fromPtr, resultFrameSize);

	++fNextOutgoingBin;
	return True;
}

QCELPDeinterleavingBuffer::FrameDescriptor::FrameDescriptor() :
		frameSize(0), frameData(NULL) {
}

QCELPDeinterleavingBuffer::FrameDescriptor::~FrameDescriptor() {
	delete[] frameData;
}

