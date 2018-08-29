/*
 * H265VideoFileSink.h
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H265VIDEOFILESINK_H_
#define INCLUDE_H265VIDEOFILESINK_H_

#include "H264or5VideoFileSink.h"

class H265VideoFileSink: public H264or5VideoFileSink {
public:
	static H265VideoFileSink* createNew(UsageEnvironment& env,
			unsigned short clientID, char const* fileName,
			char const* sPropVPSStr = NULL, char const* sPropSPSStr = NULL,
			char const* sPropPPSStr = NULL,
			// The "sProp*Str" parameters are optional 'SDP format' strings
			// (comma-separated Base64-encoded) representing VPS, SPS, and/or PPS NAL-units
			// to prepend to the output
			unsigned bufferSize = 100000, Boolean oneFilePerFrame = False);
	// See "FileSink.hh" for a description of these parameters.

protected:
	H265VideoFileSink(UsageEnvironment& env, unsigned short clientID, FILE* fid,
			char const* sPropVPSStr, char const* sPropSPSStr,
			char const* sPropPPSStr, unsigned bufferSize,
			char const* perFrameFileNamePrefix);
	// called only by createNew()
	virtual ~H265VideoFileSink();
};

#endif /* INCLUDE_H265VIDEOFILESINK_H_ */
