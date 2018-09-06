/*
 * AMRAudioFileSink.cpp
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#include "AMRAudioFileSink.h"
#include "AMRAudioSource.h"
#include "OutputFile.h"

////////// AMRAudioFileSink //////////

AMRAudioFileSink::AMRAudioFileSink(UsageEnvironment& env, FILE* fid,
		unsigned bufferSize, char const* perFrameFileNamePrefix,
		CommonPlay *cpObj) :
		FileSink(env, fid, bufferSize, perFrameFileNamePrefix, 0, cpObj), fHaveWrittenHeader(
				False) {
}

AMRAudioFileSink::~AMRAudioFileSink() {
}

AMRAudioFileSink*
AMRAudioFileSink::createNew(UsageEnvironment& env, char const* fileName,
		unsigned bufferSize, Boolean oneFilePerFrame,CommonPlay *cpObj) {
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

		return new AMRAudioFileSink(env, fid, bufferSize,
				perFrameFileNamePrefix,cpObj);
	} while (0);

	return NULL;
}

Boolean AMRAudioFileSink::sourceIsCompatibleWithUs(MediaSource& source) {
	// The input source must be a AMR Audio source:
	return source.isAMRAudioSource();
}

void AMRAudioFileSink::afterGettingFrame(unsigned frameSize,
		unsigned numTruncatedBytes, struct timeval presentationTime) {
	AMRAudioSource* source = (AMRAudioSource*) fSource;
	if (source == NULL)
		return; // sanity check

	if (!fHaveWrittenHeader && fPerFrameFileNameBuffer == NULL) {
		// Output the appropriate AMR header to the start of the file.
		// This header is defined in RFC 4867, section 5.
		// (However, we don't do this if we're creating one file per frame.)
		char headerBuffer[100];
		sprintf(headerBuffer, "#!AMR%s%s\n", source->isWideband() ? "-WB" : "",
				source->numChannels() > 1 ? "_MC1.0" : "");
		unsigned headerLength = strlen(headerBuffer);
		if (source->numChannels() > 1) {
			// Also add a 32-bit channel description field:
			headerBuffer[headerLength++] = 0;
			headerBuffer[headerLength++] = 0;
			headerBuffer[headerLength++] = 0;
			headerBuffer[headerLength++] = source->numChannels();
		}

		addData((DP_U8*) headerBuffer, headerLength, presentationTime);
	}
	fHaveWrittenHeader = True;

	// Add the 1-byte header, before writing the file data proper:
	// (Again, we don't do this if we're creating one file per frame.)
	if (fPerFrameFileNameBuffer == NULL) {
		u_int8_t frameHeader = source->lastFrameHeader();
		addData(&frameHeader, 1, presentationTime);
	}

	// Call the parent class to complete the normal file write with the input data:
	FileSink::afterGettingFrame(frameSize, numTruncatedBytes, presentationTime);
}

