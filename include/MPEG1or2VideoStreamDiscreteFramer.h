/*
 * MPEG1or2VideoStreamDiscreteFramer.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG1OR2VIDEOSTREAMDISCRETEFRAMER_H_
#define INCLUDE_MPEG1OR2VIDEOSTREAMDISCRETEFRAMER_H_

#include "MPEG1or2VideoStreamFramer.h"

#define VSH_MAX_SIZE 1000

class MPEG1or2VideoStreamDiscreteFramer: public MPEG1or2VideoStreamFramer {
public:
	static MPEG1or2VideoStreamDiscreteFramer*
	createNew(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource, Boolean iFramesOnly = False, // see MPEG1or2VideoStreamFramer.hh
			double vshPeriod = 5.0, // see MPEG1or2VideoStreamFramer.hh
			Boolean leavePresentationTimesUnmodified = False);

protected:
	MPEG1or2VideoStreamDiscreteFramer(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource, Boolean iFramesOnly, double vshPeriod,
			Boolean leavePresentationTimesUnmodified);
	// called only by createNew()
	virtual ~MPEG1or2VideoStreamDiscreteFramer();

protected:
	// redefined virtual functions:
	virtual void doGetNextFrame();

protected:
	static void afterGettingFrame(void* clientData, unsigned frameSize,
			unsigned numTruncatedBytes, struct timeval presentationTime,
			unsigned durationInMicroseconds);
	void afterGettingFrame1(unsigned frameSize, unsigned numTruncatedBytes,
			struct timeval presentationTime, unsigned durationInMicroseconds);

protected:
	Boolean fLeavePresentationTimesUnmodified;
	struct timeval fLastNonBFramePresentationTime;
	unsigned fLastNonBFrameTemporal_reference;

	// A saved copy of the most recently seen 'video_sequence_header',
	// in case we need to insert it into the stream periodically:
	DP_U8 fSavedVSHBuffer[VSH_MAX_SIZE];
	unsigned fSavedVSHSize;
	double fSavedVSHTimestamp;
	Boolean fIFramesOnly;
	double fVSHPeriod;
};

#endif /* INCLUDE_MPEG1OR2VIDEOSTREAMDISCRETEFRAMER_H_ */
