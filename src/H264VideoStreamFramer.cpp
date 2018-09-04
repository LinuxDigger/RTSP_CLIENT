/*
 * H264VideoStreamFramer.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "H264VideoStreamFramer.h"

H264VideoStreamFramer* H264VideoStreamFramer::createNew(UsageEnvironment& env,
		CommonPlay *cpObj, FramedSource* inputSource,
		Boolean includeStartCodeInOutput) {
	return new H264VideoStreamFramer(env, cpObj, inputSource, True,
			includeStartCodeInOutput);
}

H264VideoStreamFramer::H264VideoStreamFramer(UsageEnvironment& env,
		CommonPlay *cpObj, FramedSource* inputSource, Boolean createParser,
		Boolean includeStartCodeInOutput) :
		H264or5VideoStreamFramer(264, env, cpObj, inputSource, createParser,
				includeStartCodeInOutput) {
}

H264VideoStreamFramer::~H264VideoStreamFramer() {
}

Boolean H264VideoStreamFramer::isH264VideoStreamFramer() const {
	return True;
}

