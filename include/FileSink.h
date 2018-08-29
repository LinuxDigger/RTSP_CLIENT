/*
 * FileSink.h
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_FILESINK_H_
#define INCLUDE_FILESINK_H_

#include "MediaSink.h"
#include "GetFrameData.h"
class FileSink: public MediaSink {
public:
	static FileSink* createNew(UsageEnvironment& env, char const* fileName,
			unsigned bufferSize, unsigned short clientID,
			Boolean oneFilePerFrame);
	// "bufferSize" should be at least as large as the largest expected
	//   input frame.
	// "oneFilePerFrame" - if True - specifies that each input frame will
	//   be written to a separate file (using the presentation time as a
	//   file name suffix).  The default behavior ("oneFilePerFrame" == False)
	//   is to output all incoming data into a single file.

	virtual void addData(unsigned char const* data, unsigned dataSize,
			struct timeval presentationTime);
	// (Available in case a client wants to add extra data to the output file)

protected:
	FileSink(UsageEnvironment& env, FILE* fid, unsigned bufferSize,
			char const* perFrameFileNamePrefix, unsigned short clientID);
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
	unsigned char* fBuffer;
	unsigned fBufferSize;
	char* fPerFrameFileNamePrefix; // used if "oneFilePerFrame" is True
	char* fPerFrameFileNameBuffer; // used if "oneFilePerFrame" is True
	struct timeval fPrevPresentationTime;
	unsigned fSamePresentationTimeCounter;
	unsigned short _cliID;
};

#endif /* INCLUDE_FILESINK_H_ */
