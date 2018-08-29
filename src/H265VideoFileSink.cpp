/*
 * H265VideoFileSink.cpp
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#include "H265VideoFileSink.h"
#include "OutputFile.h"
#include <iostream>
using namespace std;

////////// H265VideoFileSink //////////

H265VideoFileSink::H265VideoFileSink(UsageEnvironment& env,
		unsigned short clientID, FILE* fid, char const* sPropVPSStr,
		char const* sPropSPSStr, char const* sPropPPSStr, unsigned bufferSize,
		char const* perFrameFileNamePrefix) :
		H264or5VideoFileSink(env, clientID, fid, bufferSize,
				perFrameFileNamePrefix, sPropVPSStr, sPropSPSStr, sPropPPSStr) {
}

H265VideoFileSink::~H265VideoFileSink() {
}

H265VideoFileSink*
H265VideoFileSink::createNew(UsageEnvironment& env, unsigned short clientID,
		char const* fileName, char const* sPropVPSStr, char const* sPropSPSStr,
		char const* sPropPPSStr, unsigned bufferSize, Boolean oneFilePerFrame) {
	cout << "26555555555555555555555555555555555555createNew :: " << endl;
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

		return new H265VideoFileSink(env, clientID, fid, sPropVPSStr,
				sPropSPSStr, sPropPPSStr, bufferSize, perFrameFileNamePrefix);
	} while (0);

	return NULL;
}
