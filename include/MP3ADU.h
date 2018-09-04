/*
 * MP3ADU.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MP3ADU_H_
#define INCLUDE_MP3ADU_H_

#include "FramedFilter.h"

class ADUFromMP3Source: public FramedFilter {
public:
	static ADUFromMP3Source* createNew(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource, Boolean includeADUdescriptors = True);

	void resetInput();
	// This is called whenever there's a discontinuity in the input MP3 source
	// (e.g., due to seeking within the source).  It causes any still-unprocessed
	// MP3 frame data within our queue to be discarded, so that it does not
	// erroneously get used by backpointers from the new MP3 frames.

	Boolean setScaleFactor(int scale);

protected:
	ADUFromMP3Source(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource, Boolean includeADUdescriptors);
	// called only by createNew()
	virtual ~ADUFromMP3Source();

private:
	// Redefined virtual functions:
	virtual void doGetNextFrame();
	virtual char const* MIMEtype() const;

private:
	Boolean doGetNextFrame1();

private:
	Boolean fAreEnqueueingMP3Frame;
	class SegmentQueue* fSegments;
	Boolean fIncludeADUdescriptors;
	unsigned fTotalDataSizeBeforePreviousRead;
	int fScale;
	unsigned fFrameCounter;
};

class MP3FromADUSource: public FramedFilter {
public:
	static MP3FromADUSource* createNew(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource, Boolean includeADUdescriptors = True);

protected:
	MP3FromADUSource(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource, Boolean includeADUdescriptors);
	// called only by createNew()
	virtual ~MP3FromADUSource();

private:
	// Redefined virtual functions:
	virtual void doGetNextFrame();
	virtual char const* MIMEtype() const;

private:
	Boolean needToGetAnADU();
	void insertDummyADUsIfNecessary();
	Boolean generateFrameFromHeadADU();

private:
	Boolean fAreEnqueueingADU;
	class SegmentQueue* fSegments;
};

// Definitions of external C functions that implement various MP3 operations:
extern "C" int mp3ZeroOutSideInfo(unsigned char*, unsigned, unsigned);

#endif /* INCLUDE_MP3ADU_H_ */
