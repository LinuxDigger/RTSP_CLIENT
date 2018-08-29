/*
 * MediaSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "MediaSource.h"

////////// MediaSource //////////

MediaSource::MediaSource(UsageEnvironment& env) :
		Medium(env) {
}

MediaSource::~MediaSource() {
}

Boolean MediaSource::isSource() const {
	return True;
}

char const* MediaSource::MIMEtype() const {
	return "application/OCTET-STREAM"; // default type
}

Boolean MediaSource::isFramedSource() const {
	return False; // default implementation
}
Boolean MediaSource::isRTPSource() const {
	return False; // default implementation
}
Boolean MediaSource::isMPEG1or2VideoStreamFramer() const {
	return False; // default implementation
}
Boolean MediaSource::isMPEG4VideoStreamFramer() const {
	return False; // default implementation
}
Boolean MediaSource::isH264VideoStreamFramer() const {
	return False; // default implementation
}
Boolean MediaSource::isH265VideoStreamFramer() const {
	return False; // default implementation
}
Boolean MediaSource::isDVVideoStreamFramer() const {
	return False; // default implementation
}
Boolean MediaSource::isJPEGVideoSource() const {
	return False; // default implementation
}
Boolean MediaSource::isAMRAudioSource() const {
	return False; // default implementation
}

Boolean MediaSource::lookupByName(UsageEnvironment& env, char const* sourceName,
		MediaSource*& resultSource) {
	resultSource = NULL; // unless we succeed

	Medium* medium;
	if (!Medium::lookupByName(env, sourceName, medium))
		return False;

	if (!medium->isSource()) {
		env.setResultMsg(sourceName, " is not a media source");
		return False;
	}

	resultSource = (MediaSource*) medium;
	return True;
}

void MediaSource::getAttributes() const {
	// Default implementation
	envir().setResultMsg("");
}

