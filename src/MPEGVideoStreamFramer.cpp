/*
 * MPEGVideoStreamFramer.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "MPEGVideoStreamParser.h"
#include <GroupsockHelper.h>

////////// TimeCode implementation //////////

TimeCode::TimeCode() :
		days(0), hours(0), minutes(0), seconds(0), pictures(0) {
}

TimeCode::~TimeCode() {
}

int TimeCode::operator==(TimeCode const& arg2) {
	return pictures == arg2.pictures && seconds == arg2.seconds
			&& minutes == arg2.minutes && hours == arg2.hours
			&& days == arg2.days;
}

////////// MPEGVideoStreamFramer implementation //////////

MPEGVideoStreamFramer::MPEGVideoStreamFramer(UsageEnvironment& env,
		CommonPlay *cpObj, FramedSource* inputSource) :
		FramedFilter(env, cpObj, inputSource), fFrameRate(
				0.0) /* until we learn otherwise */, fParser(NULL) {
	reset();
}

MPEGVideoStreamFramer::~MPEGVideoStreamFramer() {
	delete fParser;
}

void MPEGVideoStreamFramer::flushInput() {
	reset();
	if (fParser != NULL)
		fParser->flushInput();
}

void MPEGVideoStreamFramer::reset() {
	fPictureCount = 0;
	fPictureEndMarker = False;
	fPicturesAdjustment = 0;
	fPictureTimeBase = 0.0;
	fTcSecsBase = 0;
	fHaveSeenFirstTimeCode = False;

	// Use the current wallclock time as the base 'presentation time':
	gettimeofday(&fPresentationTimeBase, NULL);
}

#ifdef DEBUG
static struct timeval firstPT;
#endif
void MPEGVideoStreamFramer::computePresentationTime(
		unsigned numAdditionalPictures) {
	// Computes "fPresentationTime" from the most recent GOP's
	// time_code, along with the "numAdditionalPictures" parameter:
	TimeCode& tc = fCurGOPTimeCode;

	unsigned tcSecs = (((tc.days * 24) + tc.hours) * 60 + tc.minutes) * 60
			+ tc.seconds - fTcSecsBase;
	double pictureTime =
			fFrameRate == 0.0 ?
					0.0 :
					(tc.pictures + fPicturesAdjustment + numAdditionalPictures)
							/ fFrameRate;
	while (pictureTime < fPictureTimeBase) { // "if" should be enough, but just in case
		if (tcSecs > 0)
			tcSecs -= 1;
		pictureTime += 1.0;
	}
	pictureTime -= fPictureTimeBase;
	if (pictureTime < 0.0)
		pictureTime = 0.0; // sanity check
	unsigned pictureSeconds = (unsigned) pictureTime;
	double pictureFractionOfSecond = pictureTime - (double) pictureSeconds;

	fPresentationTime = fPresentationTimeBase;
	fPresentationTime.tv_sec += tcSecs + pictureSeconds;
	fPresentationTime.tv_usec += (long) (pictureFractionOfSecond * 1000000.0);
	if (fPresentationTime.tv_usec >= 1000000) {
		fPresentationTime.tv_usec -= 1000000;
		++fPresentationTime.tv_sec;
	}
#ifdef DEBUG
	if (firstPT.tv_sec == 0 && firstPT.tv_usec == 0) firstPT = fPresentationTime;
	struct timeval diffPT;
	diffPT.tv_sec = fPresentationTime.tv_sec - firstPT.tv_sec;
	diffPT.tv_usec = fPresentationTime.tv_usec - firstPT.tv_usec;
	if (fPresentationTime.tv_usec < firstPT.tv_usec) {
		--diffPT.tv_sec;
		diffPT.tv_usec += 1000000;
	}
	fprintf(stderr, "MPEGVideoStreamFramer::computePresentationTime(%d) -> %lu.%06ld [%lu.%06ld]\n", numAdditionalPictures, fPresentationTime.tv_sec, fPresentationTime.tv_usec, diffPT.tv_sec, diffPT.tv_usec);
#endif
}

void MPEGVideoStreamFramer::setTimeCode(unsigned hours, unsigned minutes,
		unsigned seconds, unsigned pictures, unsigned picturesSinceLastGOP) {
	TimeCode& tc = fCurGOPTimeCode; // abbrev
	unsigned days = tc.days;
	if (hours < tc.hours) {
		// Assume that the 'day' has wrapped around:
		++days;
	}
	tc.days = days;
	tc.hours = hours;
	tc.minutes = minutes;
	tc.seconds = seconds;
	tc.pictures = pictures;
	if (!fHaveSeenFirstTimeCode) {
		fPictureTimeBase = fFrameRate == 0.0 ? 0.0 : tc.pictures / fFrameRate;
		fTcSecsBase = (((tc.days * 24) + tc.hours) * 60 + tc.minutes) * 60
				+ tc.seconds;
		fHaveSeenFirstTimeCode = True;
	} else if (fCurGOPTimeCode == fPrevGOPTimeCode) {
		// The time code has not changed since last time.  Adjust for this:
		fPicturesAdjustment += picturesSinceLastGOP;
	} else {
		// Normal case: The time code changed since last time.
		fPrevGOPTimeCode = tc;
		fPicturesAdjustment = 0;
	}
}

void MPEGVideoStreamFramer::doGetNextFrame() {
	fParser->registerReadInterest(fTo, fMaxSize);
	continueReadProcessing();
}

void MPEGVideoStreamFramer::doStopGettingFrames() {
	flushInput();
	FramedFilter::doStopGettingFrames();
}

void MPEGVideoStreamFramer::continueReadProcessing(void* clientData,
		DP_U8* /*ptr*/, unsigned /*size*/,
		struct timeval /*presentationTime*/) {
	MPEGVideoStreamFramer* framer = (MPEGVideoStreamFramer*) clientData;
	framer->continueReadProcessing();
}

void MPEGVideoStreamFramer::continueReadProcessing() {
	unsigned acquiredFrameSize = fParser->parse();
	if (acquiredFrameSize > 0) {
		// We were able to acquire a frame from the input.
		// It has already been copied to the reader's space.
		fFrameSize = acquiredFrameSize;
		fNumTruncatedBytes = fParser->numTruncatedBytes();

		// "fPresentationTime" should have already been computed.

		// Compute "fDurationInMicroseconds" now:
		fDurationInMicroseconds =
				(fFrameRate == 0.0 || ((int) fPictureCount) < 0) ?
						0 : (unsigned) ((fPictureCount * 1000000) / fFrameRate);
#ifdef DEBUG
		fprintf(stderr, "%d bytes @%u.%06d, fDurationInMicroseconds: %d ((%d*1000000)/%f)\n", acquiredFrameSize, fPresentationTime.tv_sec, fPresentationTime.tv_usec, fDurationInMicroseconds, fPictureCount, fFrameRate);
#endif
		fPictureCount = 0;

		// Call our own 'after getting' function.  Because we're not a 'leaf'
		// source, we can call this directly, without risking infinite recursion.
		afterGetting(this);
	} else {
		// We were unable to parse a complete frame from the input, because:
		// - we had to read more data from the source stream, or
		// - the source stream has ended.
	}
}

