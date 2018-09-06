/*
 * H264VideoFileSink.h
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H264VIDEOFILESINK_H_
#define INCLUDE_H264VIDEOFILESINK_H_

#include "H264or5VideoFileSink.h"

class H264VideoFileSink: public H264or5VideoFileSink {
public:
	static H264VideoFileSink* createNew(UsageEnvironment& env,
			unsigned short cliID, char const* fileName,
			char const* sPropParameterSetsStr = NULL,
			// "sPropParameterSetsStr" is an optional 'SDP format' string
			// (comma-separated Base64-encoded) representing SPS and/or PPS NAL-units
			// to prepend to the output
			unsigned bufferSize = 100000, Boolean oneFilePerFrame = False);
	// See "FileSink.hh" for a description of these parameters.

protected:
	H264VideoFileSink(UsageEnvironment& env, unsigned short cliID, FILE* fid,
			char const* sPropParameterSetsStr, unsigned bufferSize,
			char const* perFrameFileNamePrefix);
	// called only by createNew()
	virtual ~H264VideoFileSink();
};

#endif /* INCLUDE_H264VIDEOFILESINK_H_ */
