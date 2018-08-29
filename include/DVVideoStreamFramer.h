/*
 * DVVideoStreamFramer.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_DVVIDEOSTREAMFRAMER_H_
#define INCLUDE_DVVIDEOSTREAMFRAMER_H_

#include "FramedFilter.h"

#define DV_DIF_BLOCK_SIZE 80
#define DV_NUM_BLOCKS_PER_SEQUENCE 150
#define DV_SAVED_INITIAL_BLOCKS_SIZE ((DV_NUM_BLOCKS_PER_SEQUENCE+6-1)*DV_DIF_BLOCK_SIZE)
/* enough data to ensure that it contains an intact 6-block header (which occurs at the start of a 150-block sequence) */

class DVVideoStreamFramer: public FramedFilter {
public:
	static DVVideoStreamFramer*
	createNew(UsageEnvironment& env, FramedSource* inputSource,
			Boolean sourceIsSeekable = False,
			Boolean leavePresentationTimesUnmodified = False);
	// Set "sourceIsSeekable" to True if the input source is a seekable object (e.g. a file), and the server that uses us
	// does a seek-to-zero on the source before reading from it.  (Our RTSP server implementation does this.)
	char const* profileName();
	Boolean getFrameParameters(unsigned& frameSize/*bytes*/,
			double& frameDuration/*microseconds*/);

protected:
	DVVideoStreamFramer(UsageEnvironment& env, FramedSource* inputSource,
			Boolean sourceIsSeekable, Boolean leavePresentationTimesUnmodified);
	// called only by createNew(), or by subclass constructors
	virtual ~DVVideoStreamFramer();

protected:
	// redefined virtual functions:
	virtual Boolean isDVVideoStreamFramer() const;
	virtual void doGetNextFrame();

protected:
	void getAndDeliverData(); // used to implement "doGetNextFrame()"
	static void afterGettingFrame(void* clientData, unsigned frameSize,
			unsigned numTruncatedBytes, struct timeval presentationTime,
			unsigned durationInMicroseconds);
	void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
			struct timeval presentationTime);
	void getProfile();

protected:
	Boolean fLeavePresentationTimesUnmodified;
	void const* fOurProfile;
	struct timeval fNextFramePresentationTime;
	unsigned char fSavedInitialBlocks[DV_SAVED_INITIAL_BLOCKS_SIZE];
	char fInitialBlocksPresent;
	Boolean fSourceIsSeekable;
};

#endif /* INCLUDE_DVVIDEOSTREAMFRAMER_H_ */
