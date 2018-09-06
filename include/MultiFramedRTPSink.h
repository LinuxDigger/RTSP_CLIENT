/*
 * MultiFramedRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MULTIFRAMEDRTPSINK_H_
#define INCLUDE_MULTIFRAMEDRTPSINK_H_

#include "RTPSink.h"

class MultiFramedRTPSink: public RTPSink {
public:
	void setPacketSizes(unsigned preferredPacketSize, unsigned maxPacketSize);

	typedef void (onSendErrorFunc)(void* clientData);
	void setOnSendErrorFunc(onSendErrorFunc* onSendErrorFunc,
			void* onSendErrorFuncData) {
		// Can be used to set a callback function to be called if there's an error sending RTP packets on our socket.
		fOnSendErrorFunc = onSendErrorFunc;
		fOnSendErrorData = onSendErrorFuncData;
	}

protected:
	MultiFramedRTPSink(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* rtpgs, DP_U8 rtpPayloadType,
			unsigned rtpTimestampFrequency, char const* rtpPayloadFormatName,
			unsigned numChannels = 1);
	// we're a virtual base class

	virtual ~MultiFramedRTPSink();

	virtual void doSpecialFrameHandling(unsigned fragmentationOffset,
			DP_U8* frameStart, unsigned numBytesInFrame,
			struct timeval framePresentationTime, unsigned numRemainingBytes);
	// perform any processing specific to the particular payload format
	virtual Boolean allowFragmentationAfterStart() const;
	// whether a frame can be fragmented if other frame(s) appear earlier
	// in the packet (by default: False)
	virtual Boolean allowOtherFramesAfterLastFragment() const;
	// whether other frames can be packed into a packet following the
	// final fragment of a previous, fragmented frame (by default: False)
	virtual Boolean frameCanAppearAfterPacketStart(
			DP_U8 const* frameStart, unsigned numBytesInFrame) const;
	// whether this frame can appear in position >1 in a pkt (default: True)
	virtual unsigned specialHeaderSize() const;
	// returns the size of any special header used (following the RTP header) (default: 0)
	virtual unsigned frameSpecificHeaderSize() const;
	// returns the size of any frame-specific header used (before each frame
	// within the packet) (default: 0)
	virtual unsigned computeOverflowForNewFrame(unsigned newFrameSize) const;
	// returns the number of overflow bytes that would be produced by adding a new
	// frame of size "newFrameSize" to the current RTP packet.
	// (By default, this just calls "numOverflowBytes()", but subclasses can redefine
	// this to (e.g.) impose a granularity upon RTP payload fragments.)

	// Functions that might be called by doSpecialFrameHandling(), or other subclass virtual functions:
	Boolean isFirstPacket() const {
		return fIsFirstPacket;
	}
	Boolean isFirstFrameInPacket() const {
		return fNumFramesUsedSoFar == 0;
	}
	unsigned curFragmentationOffset() const {
		return fCurFragmentationOffset;
	}
	void setMarkerBit();
	void setTimestamp(struct timeval framePresentationTime);
	void setSpecialHeaderWord(unsigned word, /* 32 bits, in host order */
	unsigned wordPosition = 0);
	void setSpecialHeaderBytes(DP_U8 const* bytes, unsigned numBytes,
			unsigned bytePosition = 0);
	void setFrameSpecificHeaderWord(unsigned word, /* 32 bits, in host order */
	unsigned wordPosition = 0);
	void setFrameSpecificHeaderBytes(DP_U8 const* bytes,
			unsigned numBytes, unsigned bytePosition = 0);
	void setFramePadding(unsigned numPaddingBytes);
	unsigned numFramesUsedSoFar() const {
		return fNumFramesUsedSoFar;
	}
	unsigned ourMaxPacketSize() const {
		return fOurMaxPacketSize;
	}

public:
	// redefined virtual functions:
	virtual void stopPlaying();

protected:
	// redefined virtual functions:
	virtual Boolean continuePlaying();

private:
	void buildAndSendPacket(Boolean isFirstPacket);
	void packFrame();
	void sendPacketIfNecessary();
	static void sendNext(void* firstArg);
	friend void sendNext(void*);

	static void afterGettingFrame(void* clientData, unsigned numBytesRead,
			unsigned numTruncatedBytes, struct timeval presentationTime,
			unsigned durationInMicroseconds);
	void afterGettingFrame1(unsigned numBytesRead, unsigned numTruncatedBytes,
			struct timeval presentationTime, unsigned durationInMicroseconds);
	Boolean isTooBigForAPacket(unsigned numBytes) const;

	static void ourHandleClosure(void* clientData);

private:
	OutPacketBuffer* fOutBuf;

	Boolean fNoFramesLeft;
	unsigned fNumFramesUsedSoFar;
	unsigned fCurFragmentationOffset;
	Boolean fPreviousFrameEndedFragmentation;

	Boolean fIsFirstPacket;
	struct timeval fNextSendTime;
	unsigned fTimestampPosition;
	unsigned fSpecialHeaderPosition;
	unsigned fSpecialHeaderSize; // size in bytes of any special header used
	unsigned fCurFrameSpecificHeaderPosition;
	unsigned fCurFrameSpecificHeaderSize; // size in bytes of cur frame-specific header
	unsigned fTotalFrameSpecificHeaderSizes; // size of all frame-specific hdrs in pkt
	unsigned fOurMaxPacketSize;

	onSendErrorFunc* fOnSendErrorFunc;
	void* fOnSendErrorData;
};

#endif /* INCLUDE_MULTIFRAMEDRTPSINK_H_ */
