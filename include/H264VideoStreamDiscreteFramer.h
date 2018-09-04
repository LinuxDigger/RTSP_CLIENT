/*
 * H264VideoStreamDiscreteFramer.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H264VIDEOSTREAMDISCRETEFRAMER_H_
#define INCLUDE_H264VIDEOSTREAMDISCRETEFRAMER_H_

#include "H264or5VideoStreamDiscreteFramer.h"

class H264VideoStreamDiscreteFramer: public H264or5VideoStreamDiscreteFramer {
public:
	static H264VideoStreamDiscreteFramer*
	createNew(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource);

protected:
	H264VideoStreamDiscreteFramer(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource);
	// called only by createNew()
	virtual ~H264VideoStreamDiscreteFramer();

private:
	// redefined virtual functions:
	virtual Boolean isH264VideoStreamFramer() const;
};

#endif /* INCLUDE_H264VIDEOSTREAMDISCRETEFRAMER_H_ */
