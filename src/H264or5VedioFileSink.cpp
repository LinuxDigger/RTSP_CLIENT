/*
 * H264or5VedioFileSink.cpp
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#include "H264or5VideoFileSink.h"
#include "H264VideoRTPSource.h" // for "parseSPropParameterSets()"

////////// H264or5VideoFileSink //////////

H264or5VideoFileSink::H264or5VideoFileSink(UsageEnvironment& env,
		unsigned short clientID, FILE* fid, unsigned bufferSize,
		char const* perFrameFileNamePrefix, char const* sPropParameterSetsStr1,
		char const* sPropParameterSetsStr2, char const* sPropParameterSetsStr3,
		CommonPlay *cpObj) :
		FileSink(env, fid, bufferSize, perFrameFileNamePrefix, clientID, cpObj), fHaveWrittenFirstFrame(
				False) {
	cout << "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM: "<<clientID<<endl;
	fSPropParameterSetsStr[0] = sPropParameterSetsStr1;
	fSPropParameterSetsStr[1] = sPropParameterSetsStr2;
	fSPropParameterSetsStr[2] = sPropParameterSetsStr3;
}

H264or5VideoFileSink::~H264or5VideoFileSink() {
}

void H264or5VideoFileSink::afterGettingFrame(unsigned frameSize,
		unsigned numTruncatedBytes, struct timeval presentationTime) {
	DP_U8 const start_code[4] = { 0x00, 0x00, 0x00, 0x01 };

	if (!fHaveWrittenFirstFrame) {
		// If we have NAL units encoded in "sprop parameter strings", prepend these to the file:
		for (unsigned j = 0; j < 3; ++j) {
			unsigned numSPropRecords;
			SPropRecord* sPropRecords = parseSPropParameterSets(
					fSPropParameterSetsStr[j], numSPropRecords);
			for (unsigned i = 0; i < numSPropRecords; ++i) {

				/////////////////////////add first VPS SPS PPS
//				addData(start_code, 4, presentationTime);
//				addData(sPropRecords[i].sPropBytes, sPropRecords[i].sPropLength,
//						presentationTime);
			}
			delete[] sPropRecords;
		}
		fHaveWrittenFirstFrame = True; // for next time
	}

	// Write the input data to the file, with the start code in front:
	addData(start_code, 4, presentationTime);

	// Call the parent class to complete the normal file write with the input data:

	FileSink::afterGettingFrame(frameSize, numTruncatedBytes, presentationTime);
}

