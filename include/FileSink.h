/*
 * FileSink.h
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_FILESINK_H_
#define INCLUDE_FILESINK_H_

#include "MediaSink.h"
#include "DP_RTSP_CLIENT_CycleQueue.h"
#include <map>
//class MediaSink;
class FileSink: public MediaSink {
public:
	static FileSink* createNew(UsageEnvironment& env, char const* fileName,
			unsigned bufferSize, unsigned short clientID,
			Boolean oneFilePerFrame, CommonPlay *cpObj);
	// "bufferSize" should be at least as large as the largest expected
	//   input frame.
	// "oneFilePerFrame" - if True - specifies that each input frame will
	//   be written to a separate file (using the presentation time as a
	//   file name suffix).  The default behavior ("oneFilePerFrame" == False)
	//   is to output all incoming data into a single file.

	virtual void addData(DP_U8 const* data, unsigned dataSize,
			struct timeval presentationTime);
	// (Available in case a client wants to add extra data to the output file)

protected:
	FileSink(UsageEnvironment& env, FILE* fid, unsigned bufferSize,
			char const* perFrameFileNamePrefix, unsigned short clientID,
			CommonPlay *cpObj);
	// called only by createNew()
	virtual ~FileSink();

protected:
	// redefined virtual functions:
	virtual Boolean continuePlaying();

protected:
	static void afterGettingFrame(void* clientData, unsigned frameSize,
			unsigned numTruncatedBytes, struct timeval presentationTime,
			unsigned durationInMicroseconds);
	virtual void afterGettingFrame(unsigned frameSize,
			unsigned numTruncatedBytes, struct timeval presentationTime);
	void afterGettingFrameGetData(unsigned frameSize,
			unsigned numTruncatedBytes, struct timeval presentationTime);

	FILE* fOutFid;
	DP_U8* fBuffer;
	unsigned fBufferSize;
	char* fPerFrameFileNamePrefix; // used if "oneFilePerFrame" is True
	char* fPerFrameFileNameBuffer; // used if "oneFilePerFrame" is True
	struct timeval fPrevPresentationTime;
	unsigned fSamePresentationTimeCounter;

	unsigned short _cliID;
	bool fGetIDRFrame;
	struct timeval tvStart;
	DP_U8 fStartCode[4];
	map<DP_U16, DP_U32> _mCliRecvFrameSequence;
	map<DP_U16, DP_U32> _mCombinFrameSize;
	map<DP_U16, DP_U8 *> _mCombinFrame;
	//
//	static DP_U32 combinFrameSize = 0;
//	static DP_U8 *u8CombinFrame = NULL;

//	 bool fGetSPSFrame;
//	bool fGetPPSFrame;
};

#endif /* INCLUDE_FILESINK_H_ */
