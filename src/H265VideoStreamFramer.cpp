/*
 * H265VideoStreamFramer.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "H265VideoStreamFramer.h"

H265VideoStreamFramer* H265VideoStreamFramer::createNew(UsageEnvironment& env,
		CommonPlay *cpObj, FramedSource* inputSource,
		Boolean includeStartCodeInOutput) {
	return new H265VideoStreamFramer(env, cpObj, inputSource, True,
			includeStartCodeInOutput);
}

H265VideoStreamFramer::H265VideoStreamFramer(UsageEnvironment& env,
		CommonPlay *cpObj, FramedSource* inputSource, Boolean createParser,
		Boolean includeStartCodeInOutput) :
		H264or5VideoStreamFramer(265, env, cpObj, inputSource, createParser,
				includeStartCodeInOutput) {
}

H265VideoStreamFramer::~H265VideoStreamFramer() {
}

Boolean H265VideoStreamFramer::isH265VideoStreamFramer() const {
	return True;
}

