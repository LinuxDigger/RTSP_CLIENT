/*
 * H265VideoStreamDiscreteFramer.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "H265VideoStreamDiscreteFramer.h"

H265VideoStreamDiscreteFramer*
H265VideoStreamDiscreteFramer::createNew(UsageEnvironment& env,
		CommonPlay *cpObj, FramedSource* inputSource) {
	return new H265VideoStreamDiscreteFramer(env, cpObj, inputSource);
}

H265VideoStreamDiscreteFramer::H265VideoStreamDiscreteFramer(
		UsageEnvironment& env, CommonPlay *cpObj, FramedSource* inputSource) :
		H264or5VideoStreamDiscreteFramer(265, env, cpObj, inputSource) {
}

H265VideoStreamDiscreteFramer::~H265VideoStreamDiscreteFramer() {
}

Boolean H265VideoStreamDiscreteFramer::isH265VideoStreamFramer() const {
	return True;
}

