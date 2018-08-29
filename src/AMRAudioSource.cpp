/*
 * AMRAudioSource.cpp
 *
 *  Created on: Jun 12, 2018
 *      Author: jhb
 */


#include "AMRAudioSource.h"

AMRAudioSource::AMRAudioSource(UsageEnvironment& env,
			       Boolean isWideband, unsigned numChannels)
  : FramedSource(env),
    fIsWideband(isWideband), fNumChannels(numChannels), fLastFrameHeader(0) {
}

AMRAudioSource::~AMRAudioSource() {
}

char const* AMRAudioSource::MIMEtype() const {
  return "audio/AMR";
}

Boolean AMRAudioSource::isAMRAudioSource() const {
  return True;
}


