/*
 * MultiFramedRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MULTIFRAMEDRTPSOURCE_H_
#define INCLUDE_MULTIFRAMEDRTPSOURCE_H_

#include "RTPSource.h"
#include "Boolean.h"

class BufferedPacket;
// forward
class BufferedPacketFactory;
// forward

class MultiFramedRTPSource: public RTPSource {
protected:
	MultiFramedRTPSource(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
			unsigned rtpTimestampFrequency,
			BufferedPacketFactory* packetFactory = NULL);
	// virtual base class
	virtual ~MultiFramedRTPSource();

	virtual Boolean processSpecialHeader(BufferedPacket* packet,
			unsigned& resultSpecialHeaderSize);
	// Subclasses redefine this to handle any special, payload format
	// specific header that follows the RTP header.

	virtual Boolean packetIsUsableInJitterCalculation(DP_U8* packet,
			unsigned packetSize);
	// The default implementation returns True, but this can be redefined

protected:
	Boolean fCurrentPacketBeginsFrame;
	Boolean fCurrentPacketCompletesFrame;

protected:
	///
	// redefined virtual functions:
	virtual void doStopGettingFrames();

private:
	// redefined virtual functions:
	virtual void doGetNextFrame();
	virtual void setPacketReorderingThresholdTime(unsigned uSeconds);

private:
	void reset();
	void doGetNextFrame1();

	static void networkReadHandler(MultiFramedRTPSource* source, int /*mask*/);
	void networkReadHandler1();

	Boolean fAreDoingNetworkReads;
	BufferedPacket* fPacketReadInProgress;
	Boolean fNeedDelivery;
	Boolean fPacketLossInFragmentedFrame;
	DP_U8* fSavedTo;
	unsigned fSavedMaxSize;

	// A buffer to (optionally) hold incoming pkts that have been reorderered
	class ReorderingPacketBuffer* fReorderingBuffer;
};

// A 'packet data' class that's used to implement the above.
// Note that this can be subclassed - if desired - to redefine
// "nextEnclosedFrameSize()".

class BufferedPacket {
public:
	BufferedPacket();
	virtual ~BufferedPacket();

	Boolean hasUsableData() const {
		return fTail > fHead;
	}
	unsigned useCount() const {
		return fUseCount;
	}

	Boolean fillInData(RTPInterface& rtpInterface,
			struct sockaddr_in& fromAddress, Boolean& packetReadWasIncomplete);
	void assignMiscParams(unsigned short rtpSeqNo, unsigned rtpTimestamp,
			struct timeval presentationTime, Boolean hasBeenSyncedUsingRTCP,
			Boolean rtpMarkerBit, struct timeval timeReceived);
	void skip(unsigned numBytes); // used to skip over an initial header
	void removePadding(unsigned numBytes); // used to remove trailing bytes
	void appendData(DP_U8* newData, unsigned numBytes);
	void use(DP_U8* to, unsigned toSize, unsigned& bytesUsed,
			unsigned& bytesTruncated, unsigned short& rtpSeqNo,
			unsigned& rtpTimestamp, struct timeval& presentationTime,
			Boolean& hasBeenSyncedUsingRTCP, Boolean& rtpMarkerBit);

	BufferedPacket*& nextPacket() {
		return fNextPacket;
	}

	unsigned short rtpSeqNo() const {
		return fRTPSeqNo;
	}
	struct timeval const& timeReceived() const {
		return fTimeReceived;
	}

	DP_U8* data() const {
		return &fBuf[fHead];
	}
	unsigned dataSize() const {
		return fTail - fHead;
	}
	Boolean rtpMarkerBit() const {
		return fRTPMarkerBit;
	}
	Boolean& isFirstPacket() {
		return fIsFirstPacket;
	}
	unsigned bytesAvailable() const {
		return fPacketSize - fTail;
	}

protected:
	virtual void reset();
	virtual unsigned nextEnclosedFrameSize(DP_U8*& framePtr,
			unsigned dataSize);
	// The above function has been deprecated.  Instead, new subclasses should use:
	virtual void getNextEnclosedFrameParameters(DP_U8*& framePtr,
			unsigned dataSize, unsigned& frameSize,
			unsigned& frameDurationInMicroseconds);

	unsigned fPacketSize;
	DP_U8* fBuf;
	unsigned fHead;
	unsigned fTail;

private:
	BufferedPacket* fNextPacket; // used to link together packets

	unsigned fUseCount;
	unsigned short fRTPSeqNo;
	unsigned fRTPTimestamp;
	struct timeval fPresentationTime; // corresponding to "fRTPTimestamp"
	Boolean fHasBeenSyncedUsingRTCP;
	Boolean fRTPMarkerBit;
	Boolean fIsFirstPacket;
	struct timeval fTimeReceived;
};

// A 'factory' class for creating "BufferedPacket" objects.
// If you want to subclass "BufferedPacket", then you'll also
// want to subclass this, to redefine createNewPacket()

class BufferedPacketFactory {
public:
	BufferedPacketFactory();
	virtual ~BufferedPacketFactory();

	virtual BufferedPacket* createNewPacket(MultiFramedRTPSource* ourSource);
};

#endif /* INCLUDE_MULTIFRAMEDRTPSOURCE_H_ */
