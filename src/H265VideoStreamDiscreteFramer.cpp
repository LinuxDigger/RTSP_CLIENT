/*
 * H265VideoStreamDiscreteFramer.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "H265VideoStreamDiscreteFramer.h"

H265VideoStreamDiscreteFramer*
H265VideoStreamDiscreteFramer::createNew(UsageEnvironment& env,
		FramedSource* inputSource) {
	return new H265VideoStreamDiscreteFramer(env, inputSource);
}

H265VideoStreamDiscreteFramer::H265VideoStreamDiscreteFramer(
		UsageEnvironment& env, FramedSource* inputSource) :
		H264or5VideoStreamDiscreteFramer(265, env, inputSource) {
}

H265VideoStreamDiscreteFramer::~H265VideoStreamDiscreteFramer() {
}

Boolean H265VideoStreamDiscreteFramer::isH265VideoStreamFramer() const {
	return True;
}

