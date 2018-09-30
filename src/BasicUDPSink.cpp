/*
 * BasicUDPSink.cpp
 *
 *  Created on: Aug 10, 2018
 *      Author: jhb
 */

#include "BasicUDPSink.h"
#include <GroupsockHelper.h>
#include "CommonPlay.h"

BasicUDPSink* BasicUDPSink::createNew(UsageEnvironment& env, Groupsock* gs,
		unsigned maxPayloadSize) {
	return new BasicUDPSink(env, gs, maxPayloadSize);
}

BasicUDPSink::BasicUDPSink(UsageEnvironment& env, Groupsock* gs,
		unsigned maxPayloadSize) :
		MediaSink(env, NULL), fGS(gs), fMaxPayloadSize(maxPayloadSize) {
	fOutputBuffer = new DP_U8[fMaxPayloadSize];
}

BasicUDPSink::~BasicUDPSink() {
	delete[] fOutputBuffer;
}

Boolean BasicUDPSink::continuePlaying() {
	// Record the fact that we're starting to play now:
	gettimeofday(&fNextSendTime, NULL);

	// Arrange to get and send the first payload.
	// (This will also schedule any future sends.)
	continuePlaying1();
	return True;
}

void BasicUDPSink::continuePlaying1() {
	nextTask() = NULL;
	if (fSource != NULL) {
		fSource->getNextFrame(fOutputBuffer, fMaxPayloadSize, afterGettingFrame,
				this, onSourceClosure, this);
	}
}

void BasicUDPSink::afterGettingFrame(void* clientData, unsigned frameSize,
		unsigned numTruncatedBytes, struct timeval /*presentationTime*/,
		unsigned durationInMicroseconds) {
	BasicUDPSink* sink = (BasicUDPSink*) clientData;
	sink->afterGettingFrame1(frameSize, numTruncatedBytes,
			durationInMicroseconds);
}

void BasicUDPSink::afterGettingFrame1(unsigned frameSize,
		unsigned numTruncatedBytes, unsigned durationInMicroseconds) {
	if (numTruncatedBytes > 0) {
		envir()
				<< "BasicUDPSink::afterGettingFrame1(): The input frame data was too large for our spcified maximum payload size ("
				<< fMaxPayloadSize << ").  " << numTruncatedBytes
				<< " bytes of trailing data was dropped!\n";
	}

	// Send the packet:
	fGS->output(envir(), fOutputBuffer, frameSize);

	// Figure out the time at which the next packet should be sent, based
	// on the duration of the payload that we just read:
	fNextSendTime.tv_usec += durationInMicroseconds;
	fNextSendTime.tv_sec += fNextSendTime.tv_usec / 1000000;
	fNextSendTime.tv_usec %= 1000000;

	struct timeval timeNow;
	gettimeofday(&timeNow, NULL);
	int secsDiff = fNextSendTime.tv_sec - timeNow.tv_sec;
	int64_t uSecondsToGo = secsDiff * 1000000
			+ (fNextSendTime.tv_usec - timeNow.tv_usec);
	if (uSecondsToGo < 0 || secsDiff < 0) { // sanity check: Make sure that the time-to-delay is non-negative:
		uSecondsToGo = 0;
	}

	// Delay this amount of time:
	nextTask() = envir().taskScheduler(fcpObj->_fClientID / 100)->scheduleDelayedTask(uSecondsToGo,
			(TaskFunc*) sendNext, this, fcpObj);
}

// The following is called after each delay between packet sends:
void BasicUDPSink::sendNext(void* firstArg) {
	BasicUDPSink* sink = (BasicUDPSink*) firstArg;
	sink->continuePlaying1();
}

