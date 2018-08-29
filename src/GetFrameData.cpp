/*
 * GetFrameData.cpp
 *
 *  Created on: Aug 27, 2018
 *      Author: jhb
 */

#include "GetFrameData.h"

//GetFrameData::GetFrameData(unsigned short cliID) :
//		_cliID(cliID), _cycQueue(NULL) {
//
//}
GetFrameData::~GetFrameData() {

}

void afterGettingFrame(void* clientData, unsigned frameSize,
		unsigned numTruncatedBytes, struct timeval presentationTime,
		unsigned durationInMicroseconds) {

}

void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
		struct timeval presentationTime) {

}
