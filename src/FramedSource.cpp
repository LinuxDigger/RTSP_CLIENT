/*
 * FramedSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "FramedSource.h"
#include <stdlib.h>
#include <iostream>
using namespace std;

////////// FramedSource //////////

FramedSource::FramedSource(UsageEnvironment& env, CommonPlay *cpObj) :
		MediaSource(env), fTo(0), fMaxSize(0), fFrameSize(0), fNumTruncatedBytes(
				0), fDurationInMicroseconds(0), fcpObj(cpObj), fAfterGettingFunc(
				NULL), fAfterGettingClientData(
		NULL), fOnCloseFunc(NULL), fOnCloseClientData(NULL), fIsCurrentlyAwaitingData(
				False) {
	fPresentationTime.tv_sec = fPresentationTime.tv_usec = 0; // initially
}

FramedSource::~FramedSource() {
}

Boolean FramedSource::isFramedSource() const {
	return True;
}

Boolean FramedSource::lookupByName(UsageEnvironment& env,
		char const* sourceName, FramedSource*& resultSource) {
	resultSource = NULL; // unless we succeed

	MediaSource* source;
	if (!MediaSource::lookupByName(env, sourceName, source))
		return False;

	if (!source->isFramedSource()) {
		env.setResultMsg(sourceName, " is not a framed source");
		return False;
	}

	resultSource = (FramedSource*) source;
	return True;
}

void FramedSource::getNextFrame(DP_U8* to, unsigned maxSize,
		afterGettingFunc* afterGettingFunc, void* afterGettingClientData,
		onCloseFunc* onCloseFunc, void* onCloseClientData) {
	// Make sure we're not already being read:
	if (fIsCurrentlyAwaitingData) {
		envir() << "FramedSource[" << this
				<< "]::getNextFrame(): attempting to read more than once at the same time!\n";
		envir().internalError();
	}

	fTo = to;
	fMaxSize = maxSize;
	fNumTruncatedBytes = 0; // by default; could be changed by doGetNextFrame()
	fDurationInMicroseconds = 0; // by default; could be changed by doGetNextFrame()
	fAfterGettingFunc = afterGettingFunc;
	fAfterGettingClientData = afterGettingClientData;
	fOnCloseFunc = onCloseFunc;
	fOnCloseClientData = onCloseClientData;
	fIsCurrentlyAwaitingData = True;

	doGetNextFrame();
}

//调用执行需要该帧数据的函数
void FramedSource::afterGetting(FramedSource* source) {
	source->nextTask() = NULL;
	source->fIsCurrentlyAwaitingData = False;
	// indicates that we can be read again
	// Note that this needs to be done here, in case the "fAfterFunc"
	// called below tries to read another frame (which it usually will)

	//xia面的fAfterGettingFunc为我们自己注册的函数，如果运行的是testProgs中的openRTSP实例，
	//则该函数指向下列代码中通过调用getNextFrame（）注册的afterGettingFrame（）函数
	if (source->fAfterGettingFunc != NULL) {
//		cout << "fAfterGettingFunc---------------------------------000000000"<<endl;
		(*(source->fAfterGettingFunc))(source->fAfterGettingClientData,
				source->fFrameSize, source->fNumTruncatedBytes,
				source->fPresentationTime, source->fDurationInMicroseconds);
	}
}

void FramedSource::handleClosure(void* clientData) {
	FramedSource* source = (FramedSource*) clientData;
	source->handleClosure();
}

void FramedSource::handleClosure() {
	fIsCurrentlyAwaitingData = False; // because we got a close instead
	if (fOnCloseFunc != NULL) {
		(*fOnCloseFunc)(fOnCloseClientData);
	}
}

void FramedSource::stopGettingFrames() {
	fIsCurrentlyAwaitingData = False; // indicates that we can be read again
	fAfterGettingFunc = NULL;
	fOnCloseFunc = NULL;

	// Perform any specialized action now:
	doStopGettingFrames();
}

void FramedSource::doStopGettingFrames() {
	// Default implementation: Do nothing except cancel any pending 'delivery' task:
	envir().taskScheduler().unscheduleDelayedTask(nextTask());
	// Subclasses may wish to redefine this function.
}

unsigned FramedSource::maxFrameSize() const {
	// By default, this source has no maximum frame size.
	return 0;
}

