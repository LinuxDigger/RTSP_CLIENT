/*
 * AMRAudioFileSink.h
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_AMRAUDIOFILESINK_H_
#define INCLUDE_AMRAUDIOFILESINK_H_

#include "FileSink.h"

class AMRAudioFileSink: public FileSink {
public:
	static AMRAudioFileSink* createNew(UsageEnvironment& env,
			char const* fileName, unsigned bufferSize = 10000,
			Boolean oneFilePerFrame = False, CommonPlay *cpObj=NULL);
	// (See "FileSink.hh" for a description of these parameters.)

protected:
	AMRAudioFileSink(UsageEnvironment& env, FILE* fid, unsigned bufferSize,
			char const* perFrameFileNamePrefix, CommonPlay *cpObj);
	// called only by createNew()
	virtual ~AMRAudioFileSink();

protected:
	// redefined virtual functions:
	virtual Boolean sourceIsCompatibleWithUs(MediaSource& source);
	virtual void afterGettingFrame(unsigned frameSize,
			unsigned numTruncatedBytes, struct timeval presentationTime);

protected:
	Boolean fHaveWrittenHeader;
};

#endif /* INCLUDE_AMRAUDIOFILESINK_H_ */
