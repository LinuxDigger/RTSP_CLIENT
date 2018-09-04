/*
 * MP3ADUinterleaving.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MP3ADUINTERLEAVING_H_
#define INCLUDE_MP3ADUINTERLEAVING_H_

#include "FramedFilter.h"

// A data structure used to represent an interleaving
#define MAX_CYCLE_SIZE 256
class Interleaving {
public:
	Interleaving(unsigned cycleSize, unsigned char const* cycleArray);
	virtual ~Interleaving();

	unsigned cycleSize() const {
		return fCycleSize;
	}
	unsigned char lookupInverseCycle(unsigned char index) const {
		return fInverseCycle[index];
	}

private:
	unsigned fCycleSize;
	unsigned char fInverseCycle[MAX_CYCLE_SIZE];
};

// This class is used only as a base for the following two:

class MP3ADUinterleaverBase: public FramedFilter {
protected:
	MP3ADUinterleaverBase(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource);
	// abstract base class
	virtual ~MP3ADUinterleaverBase();

	static FramedSource* getInputSource(UsageEnvironment& env,
			char const* inputSourceName);
	static void afterGettingFrame(void* clientData, unsigned numBytesRead,
			unsigned numTruncatedBytes, struct timeval presentationTime,
			unsigned durationInMicroseconds);
	virtual void afterGettingFrame(unsigned numBytesRead,
			struct timeval presentationTime,
			unsigned durationInMicroseconds) = 0;
};

// This class is used to convert an ADU sequence from non-interleaved
// to interleaved form:

class MP3ADUinterleaver: public MP3ADUinterleaverBase {
public:
	static MP3ADUinterleaver* createNew(UsageEnvironment& env,
			CommonPlay *cpObj, Interleaving const& interleaving,
			FramedSource* inputSource);

protected:
	MP3ADUinterleaver(UsageEnvironment& env, CommonPlay *cpObj,
			Interleaving const& interleaving, FramedSource* inputSource);
	// called only by createNew()
	virtual ~MP3ADUinterleaver();

private:
	// redefined virtual functions:
	virtual void doGetNextFrame();
	virtual void afterGettingFrame(unsigned numBytesRead,
			struct timeval presentationTime, unsigned durationInMicroseconds);

private:
	void releaseOutgoingFrame();

private:
	Interleaving const fInterleaving;
	class InterleavingFrames* fFrames;
	unsigned char fPositionOfNextIncomingFrame;
	unsigned fII, fICC;
};

// This class is used to convert an ADU sequence from interleaved
// to non-interleaved form:

class MP3ADUdeinterleaver: public MP3ADUinterleaverBase {
public:
	static MP3ADUdeinterleaver* createNew(UsageEnvironment& env,
			CommonPlay *cpObj, FramedSource* inputSource);

protected:
	MP3ADUdeinterleaver(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource);
	// called only by createNew()
	virtual ~MP3ADUdeinterleaver();

private:
	// redefined virtual functions:
	virtual void doGetNextFrame();
	virtual void afterGettingFrame(unsigned numBytesRead,
			struct timeval presentationTime, unsigned durationInMicroseconds);

private:
	void releaseOutgoingFrame();

private:
	class DeinterleavingFrames* fFrames;
	unsigned fIIlastSeen, fICClastSeen;
};

#endif /* INCLUDE_MP3ADUINTERLEAVING_H_ */
