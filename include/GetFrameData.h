/*
 * GetFrameData.h
 *
 *  Created on: Aug 27, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_GETFRAMEDATA_H_
#define INCLUDE_GETFRAMEDATA_H_

#include "CycleQueue.h"

class GetFrameData {
public:
	GetFrameData(unsigned short cliID);
	~GetFrameData();

	void afterGettingFrame(void* clientData, unsigned frameSize,
			unsigned numTruncatedBytes, struct timeval presentationTime,
			unsigned durationInMicroseconds);
	void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
			struct timeval presentationTime);
private:
	DP_U8*buffer;
//	unsigned short _cliID;
//	CycleQueue *_cycQueue;
};

#endif /* INCLUDE_GETFRAMEDATA_H_ */
