/*
 * H264or5VideoStreamDiscreteFramer.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H264OR5VIDEOSTREAMDISCRETEFRAMER_H_
#define INCLUDE_H264OR5VIDEOSTREAMDISCRETEFRAMER_H_

#include "H264or5VideoStreamFramer.h"

class H264or5VideoStreamDiscreteFramer: public H264or5VideoStreamFramer {
protected:
	H264or5VideoStreamDiscreteFramer(int hNumber, UsageEnvironment& env,
			FramedSource* inputSource);
	// we're an abstract base class
	virtual ~H264or5VideoStreamDiscreteFramer();

protected:
	// redefined virtual functions:
	virtual void doGetNextFrame();

protected:
	static void afterGettingFrame(void* clientData, unsigned frameSize,
			unsigned numTruncatedBytes, struct timeval presentationTime,
			unsigned durationInMicroseconds);
	void afterGettingFrame1(unsigned frameSize, unsigned numTruncatedBytes,
			struct timeval presentationTime, unsigned durationInMicroseconds);

	virtual Boolean nalUnitEndsAccessUnit(u_int8_t nal_unit_type);
};

#endif /* INCLUDE_H264OR5VIDEOSTREAMDISCRETEFRAMER_H_ */
