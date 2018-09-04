/*
 * H265VideoStreamDiscreteFramer.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H265VIDEOSTREAMDISCRETEFRAMER_H_
#define INCLUDE_H265VIDEOSTREAMDISCRETEFRAMER_H_

#include "H264or5VideoStreamDiscreteFramer.h"

class H265VideoStreamDiscreteFramer: public H264or5VideoStreamDiscreteFramer {
public:
	static H265VideoStreamDiscreteFramer*
	createNew(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource);

protected:
	H265VideoStreamDiscreteFramer(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource);
	// called only by createNew()
	virtual ~H265VideoStreamDiscreteFramer();

private:
	// redefined virtual functions:
	virtual Boolean isH265VideoStreamFramer() const;
};

#endif /* INCLUDE_H265VIDEOSTREAMDISCRETEFRAMER_H_ */
