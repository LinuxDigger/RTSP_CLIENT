/*
 * MediaSink.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MEDIASINK_H_
#define INCLUDE_MEDIASINK_H_

#include "FramedSource.h"
//#include "CommonPlay.h"
class CommonPlay;
class MediaSink: public Medium {
public:
	static Boolean lookupByName(UsageEnvironment& env, char const* sinkName,
			MediaSink*& resultSink);

	typedef void (afterPlayingFunc)(void* clientData, CommonPlay *cpObj);
	Boolean startPlaying(MediaSource& source, afterPlayingFunc* afterFunc,
			void* afterClientData, CommonPlay *cpObj);
	virtual void stopPlaying();

	// Test for specific types of sink:
	virtual Boolean isRTPSink() const;

	FramedSource* source() const {
		return fSource;
	}

protected:
	MediaSink(UsageEnvironment& env, CommonPlay *cpObj); // abstract base class
	virtual ~MediaSink();

	virtual Boolean sourceIsCompatibleWithUs(MediaSource& source);
	// called by startPlaying()
	virtual Boolean continuePlaying() = 0;
	// called by startPlaying()

	static void onSourceClosure(void* clientData); // can be used in "getNextFrame()" calls
	void onSourceClosure();
	// should be called (on ourselves) by continuePlaying() when it
	// discovers that the source we're playing from has closed.

	FramedSource* fSource;

private:
	// redefined virtual functions:
	virtual Boolean isSink() const;

private:
	// The following fields are used when we're being played:
	afterPlayingFunc* fAfterFunc;
	void* fAfterClientData;
protected:
	CommonPlay *fcpObj;
};

// A data structure that a sink may use for an output packet:
class OutPacketBuffer {
public:
	OutPacketBuffer(unsigned preferredPacketSize, unsigned maxPacketSize,
			unsigned maxBufferSize = 0);
	// if "maxBufferSize" is >0, use it - instead of "maxSize" to compute the buffer size
	~OutPacketBuffer();

	static unsigned maxSize;
	static void increaseMaxSizeTo(unsigned newMaxSize) {
		if (newMaxSize > OutPacketBuffer::maxSize)
			OutPacketBuffer::maxSize = newMaxSize;
	}

	DP_U8* curPtr() const {
		return &fBuf[fPacketStart + fCurOffset];
	}
	unsigned totalBytesAvailable() const {
		return fLimit - (fPacketStart + fCurOffset);
	}
	unsigned totalBufferSize() const {
		return fLimit;
	}
	DP_U8* packet() const {
		return &fBuf[fPacketStart];
	}
	unsigned curPacketSize() const {
		return fCurOffset;
	}

	void increment(unsigned numBytes) {
		fCurOffset += numBytes;
	}

	void enqueue(DP_U8 const* from, unsigned numBytes);
	void enqueueWord(u_int32_t word);
	void insert(DP_U8 const* from, unsigned numBytes,
			unsigned toPosition);
	void insertWord(u_int32_t word, unsigned toPosition);
	void extract(DP_U8* to, unsigned numBytes, unsigned fromPosition);
	u_int32_t extractWord(unsigned fromPosition);

	void skipBytes(unsigned numBytes);

	Boolean isPreferredSize() const {
		return fCurOffset >= fPreferred;
	}
	Boolean wouldOverflow(unsigned numBytes) const {
		return (fCurOffset + numBytes) > fMax;
	}
	unsigned numOverflowBytes(unsigned numBytes) const {
		return (fCurOffset + numBytes) - fMax;
	}
	Boolean isTooBigForAPacket(unsigned numBytes) const {
		return numBytes > fMax;
	}

	void setOverflowData(unsigned overflowDataOffset, unsigned overflowDataSize,
			struct timeval const& presentationTime,
			unsigned durationInMicroseconds);
	unsigned overflowDataSize() const {
		return fOverflowDataSize;
	}
	struct timeval overflowPresentationTime() const {
		return fOverflowPresentationTime;
	}
	unsigned overflowDurationInMicroseconds() const {
		return fOverflowDurationInMicroseconds;
	}
	Boolean haveOverflowData() const {
		return fOverflowDataSize > 0;
	}
	void useOverflowData();

	void adjustPacketStart(unsigned numBytes);
	void resetPacketStart();
	void resetOffset() {
		fCurOffset = 0;
	}
	void resetOverflowData() {
		fOverflowDataOffset = fOverflowDataSize = 0;
	}

private:
	unsigned fPacketStart, fCurOffset, fPreferred, fMax, fLimit;
	DP_U8* fBuf;

	unsigned fOverflowDataOffset, fOverflowDataSize;
	struct timeval fOverflowPresentationTime;
	unsigned fOverflowDurationInMicroseconds;
};

#endif /* INCLUDE_MEDIASINK_H_ */
