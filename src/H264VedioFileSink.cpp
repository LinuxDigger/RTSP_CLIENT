/*
 * H264VedioFileSink.cpp
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#include "H264VideoFileSink.h"
#include "OutputFile.h"

////////// H264VideoFileSink //////////

H264VideoFileSink::H264VideoFileSink(UsageEnvironment& env,
		unsigned short cliID, FILE* fid, char const* sPropParameterSetsStr,
		unsigned bufferSize, char const* perFrameFileNamePrefix) :
		H264or5VideoFileSink(env, cliID, fid, bufferSize,
				perFrameFileNamePrefix, sPropParameterSetsStr, NULL, NULL) {
}

H264VideoFileSink::~H264VideoFileSink() {
}

H264VideoFileSink*
H264VideoFileSink::createNew(UsageEnvironment& env, unsigned short cliID,
		char const* fileName, char const* sPropParameterSetsStr,
		unsigned bufferSize, Boolean oneFilePerFrame) {
	do {
		FILE* fid;
		char const* perFrameFileNamePrefix;
		if (oneFilePerFrame) {
			// Create the fid for each frame
			fid = NULL;
			perFrameFileNamePrefix = fileName;
		} else {
			// Normal case: create the fid once
			fid = OpenOutputFile(env, fileName);
			if (fid == NULL)
				break;
			perFrameFileNamePrefix = NULL;
		}

		return new H264VideoFileSink(env, cliID, fid, sPropParameterSetsStr,
				bufferSize, perFrameFileNamePrefix);
	} while (0);

	return NULL;
}

