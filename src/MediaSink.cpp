/*
 * MediaSink.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "MediaSink.h"
#include "GroupsockHelper.h"
#include <string.h>
#include "CommonPlay.h"

////////// MediaSink //////////

MediaSink::MediaSink(UsageEnvironment& env, CommonPlay *cpObj) :
		Medium(env, cpObj), fSource(NULL), fAfterFunc(), fAfterClientData(), fcpObj(
				cpObj) {
}

MediaSink::~MediaSink() {
	stopPlaying();
}

Boolean MediaSink::isSink() const {
	return True;
}

Boolean MediaSink::lookupByName(UsageEnvironment& env, char const* sinkName,
		MediaSink*& resultSink) {
	resultSink = NULL; // unless we succeed

	Medium* medium;
	if (!Medium::lookupByName(env, sinkName, medium))
		return False;

	if (!medium->isSink()) {
		env.setResultMsg(sinkName, " is not a media sink");
		return False;
	}

	resultSink = (MediaSink*) medium;
	return True;
}

Boolean MediaSink::sourceIsCompatibleWithUs(MediaSource& source) {
	// We currently support only framed sources.
	return source.isFramedSource();
}

Boolean MediaSink::startPlaying(MediaSource& source,
		afterPlayingFunc* afterFunc, void* afterClientData, CommonPlay *cpObj) {
	// Make sure we're not already being played:
	if (fSource != NULL) {
		envir().setResultMsg("This sink is already being played");
		return False;
	}

	// Make sure our source is compatible:
	if (!sourceIsCompatibleWithUs(source)) {
		envir().setResultMsg(
				"MediaSink::startPlaying(): source is not compatible!");
		return False;
	}
	fSource = (FramedSource*) &source;

	fAfterFunc = afterFunc;
	fAfterClientData = afterClientData;
	fcpObj = cpObj;
	return continuePlaying();
}

void MediaSink::stopPlaying() {
	// First, tell the source that we're no longer interested:
	if (fSource != NULL)
		fSource->stopGettingFrames();

	// Cancel any pending tasks:
	envir().taskScheduler(fcpObj->_fClientID / 100)->unscheduleDelayedTask(
			nextTask());

	fSource = NULL; // indicates that we can be played again
	fAfterFunc = NULL;
}

void MediaSink::onSourceClosure(void* clientData) {
	MediaSink* sink = (MediaSink*) clientData;
	sink->onSourceClosure();
}

void MediaSink::onSourceClosure() {
	// Cancel any pending tasks:
	envir().taskScheduler(fcpObj->_fClientID / 100)->unscheduleDelayedTask(
			nextTask());

	fSource = NULL; // indicates that we can be played again
	if (fAfterFunc != NULL) {
		(*fAfterFunc)(fAfterClientData, fcpObj);
	}
}

Boolean MediaSink::isRTPSink() const {
	return False; // default implementation
}

////////// OutPacketBuffer //////////

unsigned OutPacketBuffer::maxSize = 2000000; // by default

OutPacketBuffer::OutPacketBuffer(unsigned preferredPacketSize,
		unsigned maxPacketSize, unsigned maxBufferSize) :
		fPreferred(preferredPacketSize), fMax(maxPacketSize), fOverflowDataSize(
				0) {
	if (maxBufferSize == 0)
		maxBufferSize = maxSize;
	unsigned maxNumPackets = (maxBufferSize + (maxPacketSize - 1))
			/ maxPacketSize;
	fLimit = maxNumPackets * maxPacketSize;
	fBuf = new DP_U8[fLimit];
	resetPacketStart();
	resetOffset();
	resetOverflowData();
}

OutPacketBuffer::~OutPacketBuffer() {
	delete[] fBuf;
}

void OutPacketBuffer::enqueue(DP_U8 const* from, unsigned numBytes) {
	if (numBytes > totalBytesAvailable()) {
#ifdef DEBUG
		fprintf(stderr, "OutPacketBuffer::enqueue() warning: %d > %d\n", numBytes, totalBytesAvailable());
#endif
		numBytes = totalBytesAvailable();
	}

	if (curPtr() != from)
		memmove(curPtr(), from, numBytes);
	increment(numBytes);
}

void OutPacketBuffer::enqueueWord(u_int32_t word) {
	u_int32_t nWord = htonl(word);
	enqueue((DP_U8*) &nWord, 4);
}

void OutPacketBuffer::insert(DP_U8 const* from, unsigned numBytes,
		unsigned toPosition) {
	unsigned realToPosition = fPacketStart + toPosition;
	if (realToPosition + numBytes > fLimit) {
		if (realToPosition > fLimit)
			return; // we can't do this
		numBytes = fLimit - realToPosition;
	}

	memmove(&fBuf[realToPosition], from, numBytes);
	if (toPosition + numBytes > fCurOffset) {
		fCurOffset = toPosition + numBytes;
	}
}

void OutPacketBuffer::insertWord(u_int32_t word, unsigned toPosition) {
	u_int32_t nWord = htonl(word);
	insert((DP_U8*) &nWord, 4, toPosition);
}

void OutPacketBuffer::extract(DP_U8* to, unsigned numBytes,
		unsigned fromPosition) {
	unsigned realFromPosition = fPacketStart + fromPosition;
	if (realFromPosition + numBytes > fLimit) { // sanity check
		if (realFromPosition > fLimit)
			return; // we can't do this
		numBytes = fLimit - realFromPosition;
	}

	memmove(to, &fBuf[realFromPosition], numBytes);
}

u_int32_t OutPacketBuffer::extractWord(unsigned fromPosition) {
	u_int32_t nWord;
	extract((DP_U8*) &nWord, 4, fromPosition);
	return ntohl(nWord);
}

void OutPacketBuffer::skipBytes(unsigned numBytes) {
	if (numBytes > totalBytesAvailable()) {
		numBytes = totalBytesAvailable();
	}

	increment(numBytes);
}

void OutPacketBuffer::setOverflowData(unsigned overflowDataOffset,
		unsigned overflowDataSize, struct timeval const& presentationTime,
		unsigned durationInMicroseconds) {
	fOverflowDataOffset = overflowDataOffset;
	fOverflowDataSize = overflowDataSize;
	fOverflowPresentationTime = presentationTime;
	fOverflowDurationInMicroseconds = durationInMicroseconds;
}

void OutPacketBuffer::useOverflowData() {
	enqueue(&fBuf[fPacketStart + fOverflowDataOffset], fOverflowDataSize);
	fCurOffset -= fOverflowDataSize; // undoes increment performed by "enqueue"
	resetOverflowData();
}

void OutPacketBuffer::adjustPacketStart(unsigned numBytes) {
	fPacketStart += numBytes;
	if (fOverflowDataOffset >= numBytes) {
		fOverflowDataOffset -= numBytes;
	} else {
		fOverflowDataOffset = 0;
		fOverflowDataSize = 0; // an error otherwise
	}
}

void OutPacketBuffer::resetPacketStart() {
	if (fOverflowDataSize > 0) {
		fOverflowDataOffset += fPacketStart;
	}
	fPacketStart = 0;
}

