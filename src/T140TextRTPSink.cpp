/*
 * T140TextRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "T140TextRTPSink.h"
#include <GroupsockHelper.h> // for "gettimeofday()"
#include "CommonPlay.h"

////////// T140TextRTPSink implementation //////////

T140TextRTPSink::T140TextRTPSink(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat) :
		TextRTPSink(env, cpObj, RTPgs, rtpPayloadFormat,
				1000/*mandatory RTP timestamp frequency for this payload format*/,
				"T140"), fOurIdleFilter(NULL), fAreInIdlePeriod(True) {
}

T140TextRTPSink::~T140TextRTPSink() {
	fSource = fOurIdleFilter; // hack: in case "fSource" had gotten set to NULL before we were called
	stopPlaying(); // call this now, because we won't have our 'idle filter' when the base class destructor calls it later.

	// Close our 'idle filter' as well:
	Medium::close(fOurIdleFilter);
	fSource = NULL; // for the base class destructor, which gets called next
}

T140TextRTPSink*
T140TextRTPSink::createNew(UsageEnvironment& env, CommonPlay *cpObj,
		Groupsock* RTPgs, DP_U8 rtpPayloadFormat) {
	return new T140TextRTPSink(env, cpObj, RTPgs, rtpPayloadFormat);
}

Boolean T140TextRTPSink::continuePlaying() {
	// First, check whether we have an 'idle filter' set up yet. If not, create it now, and insert it in front of our existing source:
	if (fOurIdleFilter == NULL) {
		fOurIdleFilter = new T140IdleFilter(envir(), fcpObj, fSource);
	} else {
		fOurIdleFilter->reassignInputSource(fSource);
	}
	fSource = fOurIdleFilter;

	// Then call the parent class's implementation:
	return MultiFramedRTPSink::continuePlaying();
}

void T140TextRTPSink::doSpecialFrameHandling(unsigned /*fragmentationOffset*/,
		DP_U8* /*frameStart*/, unsigned numBytesInFrame,
		struct timeval framePresentationTime, unsigned /*numRemainingBytes*/) {
	// Set the RTP 'M' (marker) bit if we have just ended an idle period - i.e., if we were in an idle period, but just got data:
	if (fAreInIdlePeriod && numBytesInFrame > 0)
		setMarkerBit();
	fAreInIdlePeriod = numBytesInFrame == 0;

	setTimestamp(framePresentationTime);
}

Boolean T140TextRTPSink::frameCanAppearAfterPacketStart(
		DP_U8 const* /*frameStart*/, unsigned /*numBytesInFrame*/) const {
	return False; // We don't concatenate input data; instead, send it out immediately
}

////////// T140IdleFilter implementation //////////

T140IdleFilter::T140IdleFilter(UsageEnvironment& env, CommonPlay *cpObj,
		FramedSource* inputSource) :
		FramedFilter(env, cpObj, inputSource), fIdleTimerTask(NULL), fBufferSize(
				OutPacketBuffer::maxSize), fNumBufferedBytes(0), fBufferedNumTruncatedBytes(
				0), fBufferedDataDurationInMicroseconds(0) {
	fBuffer = new char[fBufferSize];
}

T140IdleFilter::~T140IdleFilter() {
	envir().taskScheduler(fcpObj->_fClientID / 100)->unscheduleDelayedTask(
			fIdleTimerTask);

	delete[] fBuffer;
	detachInputSource(); // so that the subsequent ~FramedFilter() doesn't delete it
}

#define IDLE_TIMEOUT_MICROSECONDS 300000 /* 300 ms */

void T140IdleFilter::doGetNextFrame() {
	// First, see if we have buffered data that we can deliver:
	if (fNumBufferedBytes > 0) {
		deliverFromBuffer();
		return;
	}

	// We don't have any buffered data, so ask our input source for data (unless we've already done so).
	// But also set a timer to expire if this doesn't arrive promptly:
	fIdleTimerTask =
			envir().taskScheduler(fcpObj->_fClientID / 100)->scheduleDelayedTask(
			IDLE_TIMEOUT_MICROSECONDS, handleIdleTimeout, this, fcpObj);
	if (fInputSource != NULL && !fInputSource->isCurrentlyAwaitingData()) {
		fInputSource->getNextFrame((DP_U8*) fBuffer, fBufferSize,
				afterGettingFrame, this, onSourceClosure, this);
	}
}

void T140IdleFilter::afterGettingFrame(void* clientData, unsigned frameSize,
		unsigned numTruncatedBytes, struct timeval presentationTime,
		unsigned durationInMicroseconds) {
	((T140IdleFilter*) clientData)->afterGettingFrame(frameSize,
			numTruncatedBytes, presentationTime, durationInMicroseconds);
}

void T140IdleFilter::afterGettingFrame(unsigned frameSize,
		unsigned numTruncatedBytes, struct timeval presentationTime,
		unsigned durationInMicroseconds) {
	// First, cancel any pending idle timer:
	envir().taskScheduler(fcpObj->_fClientID / 100)->unscheduleDelayedTask(
			fIdleTimerTask);

	// Then note the new data that we have in our buffer:
	fNumBufferedBytes = frameSize;
	fBufferedNumTruncatedBytes = numTruncatedBytes;
	fBufferedDataPresentationTime = presentationTime;
	fBufferedDataDurationInMicroseconds = durationInMicroseconds;

	// Then, attempt to deliver this data.  (If we can't deliver it now, we'll do so the next time the reader asks for data.)
	if (isCurrentlyAwaitingData())
		(void) deliverFromBuffer();
}

void T140IdleFilter::doStopGettingFrames() {
	// Cancel any pending idle timer:
	envir().taskScheduler(fcpObj->_fClientID / 100)->unscheduleDelayedTask(
			fIdleTimerTask);

	// And call the parent's implementation of this virtual function:
	FramedFilter::doStopGettingFrames();
}

void T140IdleFilter::handleIdleTimeout(void* clientData, CommonPlay *cpObj) {
	((T140IdleFilter*) clientData)->handleIdleTimeout();
}

void T140IdleFilter::handleIdleTimeout() {
	// No data has arrived from the upstream source within our specified 'idle period' (after data was requested from downstream).
	// Send an empty 'idle' frame to our downstream "T140TextRTPSink".  (This will cause an empty RTP packet to get sent.)
	deliverEmptyFrame();
}

void T140IdleFilter::deliverFromBuffer() {
	if (fNumBufferedBytes <= fMaxSize) { // common case
		fNumTruncatedBytes = fBufferedNumTruncatedBytes;
		fFrameSize = fNumBufferedBytes;
	} else {
		fNumTruncatedBytes = fBufferedNumTruncatedBytes + fNumBufferedBytes
				- fMaxSize;
		fFrameSize = fMaxSize;
	}

	memmove(fTo, fBuffer, fFrameSize);
	fPresentationTime = fBufferedDataPresentationTime;
	fDurationInMicroseconds = fBufferedDataDurationInMicroseconds;

	fNumBufferedBytes = 0; // reset buffer

	FramedSource::afterGetting(this); // complete delivery
}

void T140IdleFilter::deliverEmptyFrame() {
	fFrameSize = fNumTruncatedBytes = 0;
	gettimeofday(&fPresentationTime, NULL);
	FramedSource::afterGetting(this); // complete delivery
}

void T140IdleFilter::onSourceClosure(void* clientData) {
	((T140IdleFilter*) clientData)->onSourceClosure();
}

void T140IdleFilter::onSourceClosure() {
	envir().taskScheduler(fcpObj->_fClientID / 100)->unscheduleDelayedTask(
			fIdleTimerTask);
	fIdleTimerTask = NULL;

	handleClosure();
}

