/*
 * H264VideoStreamFramer.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H264VIDEOSTREAMFRAMER_H_
#define INCLUDE_H264VIDEOSTREAMFRAMER_H_

#include "H264or5VideoStreamFramer.h"

class H264VideoStreamFramer: public H264or5VideoStreamFramer {
public:
	static H264VideoStreamFramer* createNew(UsageEnvironment& env,
			CommonPlay *cpObj, FramedSource* inputSource,
			Boolean includeStartCodeInOutput = False);

protected:
	H264VideoStreamFramer(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource, Boolean createParser,
			Boolean includeStartCodeInOutput);
	// called only by "createNew()"
	virtual ~H264VideoStreamFramer();

	// redefined virtual functions:
	virtual Boolean isH264VideoStreamFramer() const;
};

#endif /* INCLUDE_H264VIDEOSTREAMFRAMER_H_ */
