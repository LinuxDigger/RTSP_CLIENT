/*
 * H264or5VedioFileSink.h
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H264OR5VIDEOFILESINK_H_
#define INCLUDE_H264OR5VIDEOFILESINK_H_

#include "FileSink.h"

class H264or5VideoFileSink: public FileSink {
protected:
	H264or5VideoFileSink(UsageEnvironment& env, unsigned short clientID,
			FILE* fid, unsigned bufferSize, char const* perFrameFileNamePrefix,
			char const* sPropParameterSetsStr1,
			char const* sPropParameterSetsStr2 = NULL,
			char const* sPropParameterSetsStr3 = NULL,CommonPlay *cpObj=NULL);
	// we're an abstract base class
	virtual ~H264or5VideoFileSink();

protected:
	// redefined virtual functions:
	virtual void afterGettingFrame(unsigned frameSize,
			unsigned numTruncatedBytes, struct timeval presentationTime);

private:
	char const* fSPropParameterSetsStr[3];
	Boolean fHaveWrittenFirstFrame;
};

#endif /* INCLUDE_H264OR5VIDEOFILESINK_H_ */
