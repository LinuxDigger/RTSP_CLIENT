/*
 * MPEGVideoStreamParser.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#include "MPEGVideoStreamParser.h"

MPEGVideoStreamParser::MPEGVideoStreamParser(MPEGVideoStreamFramer* usingSource,
		FramedSource* inputSource) :
		StreamParser(inputSource, FramedSource::handleClosure, usingSource,
				&MPEGVideoStreamFramer::continueReadProcessing, usingSource), fUsingSource(
				usingSource), fStartOfFrame(NULL), fTo(NULL), fLimit(NULL), fNumTruncatedBytes(
				0), fSavedTo(NULL), fSavedNumTruncatedBytes(0) {
}

MPEGVideoStreamParser::~MPEGVideoStreamParser() {
}

void MPEGVideoStreamParser::restoreSavedParserState() {
	StreamParser::restoreSavedParserState();
	fTo = fSavedTo;
	fNumTruncatedBytes = fSavedNumTruncatedBytes;
}

void MPEGVideoStreamParser::registerReadInterest(unsigned char* to,
		unsigned maxSize) {
	fStartOfFrame = fTo = fSavedTo = to;
	fLimit = to + maxSize;
	fNumTruncatedBytes = fSavedNumTruncatedBytes = 0;
}

