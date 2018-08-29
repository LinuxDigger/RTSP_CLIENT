/*
 * FramedFilter.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "FramedFilter.h"

////////// FramedFilter //////////
#include <string.h>

void FramedFilter::detachInputSource() {
	if (fInputSource != NULL) {
		fInputSource->stopGettingFrames();
		reassignInputSource(NULL);
	}
}

FramedFilter::FramedFilter(UsageEnvironment& env, FramedSource* inputSource) :
		FramedSource(env), fInputSource(inputSource) {
}

FramedFilter::~FramedFilter() {
	Medium::close(fInputSource);
}

// Default implementations of needed virtual functions.  These merely
// call the same function in the input source - i.e., act like a 'null filter

char const* FramedFilter::MIMEtype() const {
	if (fInputSource == NULL)
		return "";

	return fInputSource->MIMEtype();
}

void FramedFilter::getAttributes() const {
	if (fInputSource != NULL)
		fInputSource->getAttributes();
}

void FramedFilter::doStopGettingFrames() {
	FramedSource::doStopGettingFrames();
	if (fInputSource != NULL)
		fInputSource->stopGettingFrames();
}

