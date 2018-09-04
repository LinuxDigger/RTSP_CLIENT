/*
 * H264VideoStreamDiscreteFramer.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "H264VideoStreamDiscreteFramer.h"

H264VideoStreamDiscreteFramer*
H264VideoStreamDiscreteFramer::createNew(UsageEnvironment& env,
		CommonPlay *cpObj, FramedSource* inputSource) {
	return new H264VideoStreamDiscreteFramer(env, cpObj, inputSource);
}

H264VideoStreamDiscreteFramer::H264VideoStreamDiscreteFramer(
		UsageEnvironment& env, CommonPlay *cpObj, FramedSource* inputSource) :
		H264or5VideoStreamDiscreteFramer(264, env, cpObj, inputSource) {
}

H264VideoStreamDiscreteFramer::~H264VideoStreamDiscreteFramer() {
}

Boolean H264VideoStreamDiscreteFramer::isH264VideoStreamFramer() const {
	return True;
}

