/*
 * MP3ADUinterleaving.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "MP3ADUinterleaving.h"
#include "MP3ADUdescriptor.h"
//#include ""
#include <string.h>

#include "GroupsockHelper.h"

////////// Interleaving //////////

Interleaving::Interleaving(unsigned cycleSize, DP_U8 const* cycleArray) :
		fCycleSize(cycleSize) {
	for (unsigned i = 0; i < fCycleSize; ++i) {
		fInverseCycle[cycleArray[i]] = i;
	}
}

Interleaving::~Interleaving() {
}

////////// MP3ADUinterleaverBase //////////

MP3ADUinterleaverBase::MP3ADUinterleaverBase(UsageEnvironment& env,
		CommonPlay *cpObj, FramedSource* inputSource) :
		FramedFilter(env, cpObj, inputSource) {
}
MP3ADUinterleaverBase::~MP3ADUinterleaverBase() {
}

FramedSource* MP3ADUinterleaverBase::getInputSource(UsageEnvironment& env,
		char const* inputSourceName) {
	FramedSource* inputSource;
	if (!FramedSource::lookupByName(env, inputSourceName, inputSource))
		return NULL;

	if (strcmp(inputSource->MIMEtype(), "audio/MPA-ROBUST") != 0) {
		env.setResultMsg(inputSourceName, " is not an MP3 ADU source");
		return NULL;
	}

	return inputSource;
}

void MP3ADUinterleaverBase::afterGettingFrame(void* clientData,
		unsigned numBytesRead, unsigned /*numTruncatedBytes*/,
		struct timeval presentationTime, unsigned durationInMicroseconds) {
	MP3ADUinterleaverBase* interleaverBase = (MP3ADUinterleaverBase*) clientData;
	// Finish up after reading:
	interleaverBase->afterGettingFrame(numBytesRead, presentationTime,
			durationInMicroseconds);

	// Then, continue to deliver an outgoing frame:
	interleaverBase->doGetNextFrame();
}

////////// InterleavingFrames (definition) //////////

class InterleavingFrames {
public:
	InterleavingFrames(unsigned maxCycleSize);
	virtual ~InterleavingFrames();

	Boolean haveReleaseableFrame();
	void getIncomingFrameParams(DP_U8 index, DP_U8*& dataPtr,
			unsigned& bytesAvailable);
	void getReleasingFrameParams(DP_U8 index, DP_U8*& dataPtr,
			unsigned& bytesInUse, struct timeval& presentationTime,
			unsigned& durationInMicroseconds);
	void setFrameParams(DP_U8 index, DP_U8 icc,
			DP_U8 ii, unsigned frameSize,
			struct timeval presentationTime, unsigned durationInMicroseconds);
	unsigned nextIndexToRelease() {
		return fNextIndexToRelease;
	}
	void releaseNext();

private:
	unsigned fMaxCycleSize;
	unsigned fNextIndexToRelease;
	class InterleavingFrameDescriptor* fDescriptors;
};

////////// MP3ADUinterleaver //////////

MP3ADUinterleaver::MP3ADUinterleaver(UsageEnvironment& env, CommonPlay *cpObj,
		Interleaving const& interleaving, FramedSource* inputSource) :
		MP3ADUinterleaverBase(env, cpObj, inputSource), fInterleaving(
				interleaving), fFrames(
				new InterleavingFrames(interleaving.cycleSize())), fPositionOfNextIncomingFrame(
				0), fII(0), fICC(0) {
}

MP3ADUinterleaver::~MP3ADUinterleaver() {
	delete fFrames;
}

MP3ADUinterleaver* MP3ADUinterleaver::createNew(UsageEnvironment& env,
		CommonPlay *cpObj, Interleaving const& interleaving,
		FramedSource* inputSource) {
	return new MP3ADUinterleaver(env, cpObj, interleaving, inputSource);
}

void MP3ADUinterleaver::doGetNextFrame() {
	// If there's a frame immediately available, deliver it, otherwise get new
	// frames from the source until one's available:
	if (fFrames->haveReleaseableFrame()) {
		releaseOutgoingFrame();

		// Call our own 'after getting' function.  Because we're not a 'leaf'
		// source, we can call this directly, without risking infinite recursion.
		afterGetting(this);
	} else {
		fPositionOfNextIncomingFrame = fInterleaving.lookupInverseCycle(fII);
		DP_U8* dataPtr;
		unsigned bytesAvailable;
		fFrames->getIncomingFrameParams(fPositionOfNextIncomingFrame, dataPtr,
				bytesAvailable);

		// Read the next incoming frame (asynchronously)
		fInputSource->getNextFrame(dataPtr, bytesAvailable,
				&MP3ADUinterleaverBase::afterGettingFrame, this, handleClosure,
				this);
	}
}

void MP3ADUinterleaver::releaseOutgoingFrame() {
	DP_U8* fromPtr;
	fFrames->getReleasingFrameParams(fFrames->nextIndexToRelease(), fromPtr,
			fFrameSize, fPresentationTime, fDurationInMicroseconds);

	if (fFrameSize > fMaxSize) {
		fNumTruncatedBytes = fFrameSize - fMaxSize;
		fFrameSize = fMaxSize;
	}
	memmove(fTo, fromPtr, fFrameSize);

	fFrames->releaseNext();
}

void MP3ADUinterleaver::afterGettingFrame(unsigned numBytesRead,
		struct timeval presentationTime, unsigned durationInMicroseconds) {
	// Set the (icc,ii) and frame size of the newly-read frame:
	fFrames->setFrameParams(fPositionOfNextIncomingFrame, fICC, fII,
			numBytesRead, presentationTime, durationInMicroseconds);

	// Prepare our counters for the next frame:
	if (++fII == fInterleaving.cycleSize()) {
		fII = 0;
		fICC = (fICC + 1) % 8;
	}
}

////////// DeinterleavingFrames (definition) //////////

class DeinterleavingFrames {
public:
	DeinterleavingFrames();
	virtual ~DeinterleavingFrames();

	Boolean haveReleaseableFrame();
	void getIncomingFrameParams(DP_U8*& dataPtr,
			unsigned& bytesAvailable);
	void getIncomingFrameParamsAfter(unsigned frameSize,
			struct timeval presentationTime, unsigned durationInMicroseconds,
			DP_U8& icc, DP_U8& ii);
	void getReleasingFrameParams(DP_U8*& dataPtr, unsigned& bytesInUse,
			struct timeval& presentationTime, unsigned& durationInMicroseconds);
	void moveIncomingFrameIntoPlace();
	void releaseNext();
	void startNewCycle();

private:
	unsigned fNextIndexToRelease;
	Boolean fHaveEndedCycle;
	unsigned fIIlastSeen;
	unsigned fMinIndexSeen, fMaxIndexSeen; // actually, max+1
	class DeinterleavingFrameDescriptor* fDescriptors;
};

////////// MP3ADUdeinterleaver //////////

MP3ADUdeinterleaver::MP3ADUdeinterleaver(UsageEnvironment& env,
		CommonPlay *cpObj, FramedSource* inputSource) :
		MP3ADUinterleaverBase(env, cpObj, inputSource), fFrames(
				new DeinterleavingFrames), fIIlastSeen(~0), fICClastSeen(~0) {
}

MP3ADUdeinterleaver::~MP3ADUdeinterleaver() {
	delete fFrames;
}

MP3ADUdeinterleaver* MP3ADUdeinterleaver::createNew(UsageEnvironment& env,
		CommonPlay *cpObj, FramedSource* inputSource) {
	return new MP3ADUdeinterleaver(env, cpObj, inputSource);
}

void MP3ADUdeinterleaver::doGetNextFrame() {
	// If there's a frame immediately available, deliver it, otherwise get new
	// frames from the source until one's available:
	if (fFrames->haveReleaseableFrame()) {
		releaseOutgoingFrame();

		// Call our own 'after getting' function.  Because we're not a 'leaf'
		// source, we can call this directly, without risking infinite recursion.
		afterGetting(this);
	} else {
#ifdef TEST_LOSS
		NOTE: This code no longer works, because it uses synchronous reads,
		which are no longer supported.
		static unsigned const framesPerPacket = 3;
		static unsigned const frameCount = 0;
		static Boolean packetIsLost;
		while (1) {
			unsigned packetCount = frameCount/framesPerPacket;
			if ((frameCount++)%framesPerPacket == 0) {
				packetIsLost = (our_random()%10 == 0); // simulate 10% packet loss #####
			}

			if (packetIsLost) {
				// Read and discard the next input frame (that would be part of
				// a lost packet):
				DP_U8 dummyBuf[2000];
				unsigned numBytesRead;
				struct timeval presentationTime;
				// (this works only if the source can be read synchronously)
				fInputSource->syncGetNextFrame(dummyBuf, sizeof dummyBuf,
						numBytesRead, presentationTime);
			} else {
				break; // from while (1)
			}
		}
#endif
		DP_U8* dataPtr;
		unsigned bytesAvailable;
		fFrames->getIncomingFrameParams(dataPtr, bytesAvailable);

		// Read the next incoming frame (asynchronously)
		fInputSource->getNextFrame(dataPtr, bytesAvailable,
				&MP3ADUinterleaverBase::afterGettingFrame, this, handleClosure,
				this);
	}
}

void MP3ADUdeinterleaver::afterGettingFrame(unsigned numBytesRead,
		struct timeval presentationTime, unsigned durationInMicroseconds) {
	// Get the (icc,ii) and set the frame size of the newly-read frame:
	DP_U8 icc, ii;
	fFrames->getIncomingFrameParamsAfter(numBytesRead, presentationTime,
			durationInMicroseconds, icc, ii);

	// Compare these to the values we saw last:
	if (icc != fICClastSeen || ii == fIIlastSeen) {
		// We've started a new interleave cycle
		// (or interleaving was not used).  Release all
		// pending ADU frames to the ADU->MP3 conversion step:
		fFrames->startNewCycle();
	} else {
		// We're still in the same cycle as before.
		// Move the newly-read frame into place, so it can be used:
		fFrames->moveIncomingFrameIntoPlace();
	}

	fICClastSeen = icc;
	fIIlastSeen = ii;
}

void MP3ADUdeinterleaver::releaseOutgoingFrame() {
	DP_U8* fromPtr;
	fFrames->getReleasingFrameParams(fromPtr, fFrameSize, fPresentationTime,
			fDurationInMicroseconds);

	if (fFrameSize > fMaxSize) {
		fNumTruncatedBytes = fFrameSize - fMaxSize;
		fFrameSize = fMaxSize;
	}
	memmove(fTo, fromPtr, fFrameSize);

	fFrames->releaseNext();
}

////////// InterleavingFrames (implementation) //////////

#define MAX_FRAME_SIZE 2000 /* conservatively high */

class InterleavingFrameDescriptor {
public:
	InterleavingFrameDescriptor() :
			presentationTime(), durationInMicroseconds(0) {
		frameDataSize = 0;
	}

	unsigned frameDataSize; // includes ADU descriptor and (modified) MPEG hdr
	struct timeval presentationTime;
	unsigned durationInMicroseconds;
	DP_U8 frameData[MAX_FRAME_SIZE]; // ditto
};

InterleavingFrames::InterleavingFrames(unsigned maxCycleSize) :
		fMaxCycleSize(maxCycleSize), fNextIndexToRelease(0), fDescriptors(
				new InterleavingFrameDescriptor[maxCycleSize]) {
}
InterleavingFrames::~InterleavingFrames() {
	delete[] fDescriptors;
}

Boolean InterleavingFrames::haveReleaseableFrame() {
	return fDescriptors[fNextIndexToRelease].frameDataSize > 0;
}

void InterleavingFrames::getIncomingFrameParams(DP_U8 index,
		DP_U8*& dataPtr, unsigned& bytesAvailable) {
	InterleavingFrameDescriptor& desc = fDescriptors[index];
	dataPtr = &desc.frameData[0];
	bytesAvailable = MAX_FRAME_SIZE;
}

void InterleavingFrames::getReleasingFrameParams(DP_U8 index,
		DP_U8*& dataPtr, unsigned& bytesInUse,
		struct timeval& presentationTime, unsigned& durationInMicroseconds) {
	InterleavingFrameDescriptor& desc = fDescriptors[index];
	dataPtr = &desc.frameData[0];
	bytesInUse = desc.frameDataSize;
	presentationTime = desc.presentationTime;
	durationInMicroseconds = desc.durationInMicroseconds;
}

void InterleavingFrames::setFrameParams(DP_U8 index, DP_U8 icc,
		DP_U8 ii, unsigned frameSize, struct timeval presentationTime,
		unsigned durationInMicroseconds) {
	InterleavingFrameDescriptor& desc = fDescriptors[index];
	desc.frameDataSize = frameSize;
	desc.presentationTime = presentationTime;
	desc.durationInMicroseconds = durationInMicroseconds;

	// Advance over the ADU descriptor, to get to the MPEG 'syncword':
	DP_U8* ptr = &desc.frameData[0];
	(void) ADUdescriptor::getRemainingFrameSize(ptr);

	// Replace the next 11 bits with (ii,icc):
	*ptr++ = ii;
	*ptr &= ~0xE0;
	*ptr |= (icc << 5);
}

void InterleavingFrames::releaseNext() {
	fDescriptors[fNextIndexToRelease].frameDataSize = 0;
	fNextIndexToRelease = (fNextIndexToRelease + 1) % fMaxCycleSize;
}

////////// DeinterleavingFrames (implementation) //////////

class DeinterleavingFrameDescriptor {
public:
	DeinterleavingFrameDescriptor() :
			presentationTime(), durationInMicroseconds(0) {
		frameDataSize = 0;
		frameData = NULL;
	}
	virtual ~DeinterleavingFrameDescriptor() {
		delete[] frameData;
	}

	unsigned frameDataSize; // includes ADU descriptor and (modified) MPEG hdr
	struct timeval presentationTime;
	unsigned durationInMicroseconds;
	DP_U8* frameData;
};

DeinterleavingFrames::DeinterleavingFrames() :
		fNextIndexToRelease(0), fHaveEndedCycle(False), fIIlastSeen(0), fMinIndexSeen(
		MAX_CYCLE_SIZE), fMaxIndexSeen(0), fDescriptors(
				new DeinterleavingFrameDescriptor[MAX_CYCLE_SIZE + 1]) {
}
DeinterleavingFrames::~DeinterleavingFrames() {
	delete[] fDescriptors;
}

Boolean DeinterleavingFrames::haveReleaseableFrame() {
	if (!fHaveEndedCycle) {
		// Check just the next frame in the sequence
		return fDescriptors[fNextIndexToRelease].frameDataSize > 0;
	} else {
		// We've just ended a cycle, so we can skip over frames that didn't
		// get filled in (due to packet loss):
		if (fNextIndexToRelease < fMinIndexSeen) {
			fNextIndexToRelease = fMinIndexSeen;
		}
		while (fNextIndexToRelease < fMaxIndexSeen
				&& fDescriptors[fNextIndexToRelease].frameDataSize == 0) {
			++fNextIndexToRelease;
		}
		if (fNextIndexToRelease >= fMaxIndexSeen) {
			// No more frames are available from the cycle that we just ended, so
			// clear out all previously stored frames, then make available
			// the last-read frame, and return false for now:
			for (unsigned i = fMinIndexSeen; i < fMaxIndexSeen; ++i) {
				fDescriptors[i].frameDataSize = 0;
			}

			fMinIndexSeen = MAX_CYCLE_SIZE;
			fMaxIndexSeen = 0;
			moveIncomingFrameIntoPlace();

			fHaveEndedCycle = False;
			fNextIndexToRelease = 0;
			return False;
		}

		return True;
	}
}

void DeinterleavingFrames::getIncomingFrameParams(DP_U8*& dataPtr,
		unsigned& bytesAvailable) {
	// Use fDescriptors[MAX_CYCLE_SIZE] to store the incoming frame,
	// prior to figuring out its real position:
	DeinterleavingFrameDescriptor& desc = fDescriptors[MAX_CYCLE_SIZE];
	if (desc.frameData == NULL) {
		// There's no buffer yet, so allocate a new one:
		desc.frameData = new DP_U8[MAX_FRAME_SIZE];
	}
	dataPtr = desc.frameData;
	bytesAvailable = MAX_FRAME_SIZE;
}

void DeinterleavingFrames::getIncomingFrameParamsAfter(unsigned frameSize,
		struct timeval presentationTime, unsigned durationInMicroseconds,
		DP_U8& icc, DP_U8& ii) {
	DeinterleavingFrameDescriptor& desc = fDescriptors[MAX_CYCLE_SIZE];
	desc.frameDataSize = frameSize;
	desc.presentationTime = presentationTime;
	desc.durationInMicroseconds = durationInMicroseconds;

	// Advance over the ADU descriptor, to get to the MPEG 'syncword':
	DP_U8* ptr = desc.frameData;
	(void) ADUdescriptor::getRemainingFrameSize(ptr);

	// Read the next 11 bits into (ii,icc), and replace them with all-1s:
	fIIlastSeen = ii = *ptr;
	*ptr++ = 0xFF;
	icc = (*ptr & 0xE0) >> 5;
	*ptr |= 0xE0;
}

void DeinterleavingFrames::getReleasingFrameParams(DP_U8*& dataPtr,
		unsigned& bytesInUse, struct timeval& presentationTime,
		unsigned& durationInMicroseconds) {
	DeinterleavingFrameDescriptor& desc = fDescriptors[fNextIndexToRelease];
	dataPtr = desc.frameData;
	bytesInUse = desc.frameDataSize;
	presentationTime = desc.presentationTime;
	durationInMicroseconds = desc.durationInMicroseconds;
}

void DeinterleavingFrames::moveIncomingFrameIntoPlace() {
	DeinterleavingFrameDescriptor& fromDesc = fDescriptors[MAX_CYCLE_SIZE];
	DeinterleavingFrameDescriptor& toDesc = fDescriptors[fIIlastSeen];

	toDesc.frameDataSize = fromDesc.frameDataSize;
	toDesc.presentationTime = fromDesc.presentationTime;

	// Move the data pointer into place by swapping the data pointers:
	DP_U8* tmp = toDesc.frameData;
	toDesc.frameData = fromDesc.frameData;
	fromDesc.frameData = tmp;

	if (fIIlastSeen < fMinIndexSeen) {
		fMinIndexSeen = fIIlastSeen;
	}
	if (fIIlastSeen + 1 > fMaxIndexSeen) {
		fMaxIndexSeen = fIIlastSeen + 1;
	}
}

void DeinterleavingFrames::releaseNext() {
	fDescriptors[fNextIndexToRelease].frameDataSize = 0;
	fNextIndexToRelease = (fNextIndexToRelease + 1) % MAX_CYCLE_SIZE;
}

void DeinterleavingFrames::startNewCycle() {
	fHaveEndedCycle = True;
}

