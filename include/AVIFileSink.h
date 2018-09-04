/*
 * AVIFileSink.h
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_AVIFILESINK_H_
#define INCLUDE_AVIFILESINK_H_

#include "MediaSession.h"
//#include "CommonPlay.h"
class CommonPlay;
class AVIFileSink: public Medium {
public:
	static AVIFileSink* createNew(UsageEnvironment& env,
			MediaSession& inputSession, char const* outputFileName,
			unsigned bufferSize = 20000, unsigned short movieWidth = 240,
			unsigned short movieHeight = 180, unsigned movieFPS = 15,
			Boolean packetLossCompensate = False, CommonPlay *cpObj = NULL);

	typedef void (afterPlayingFunc)(void* clientData, CommonPlay *cpObj);
	Boolean startPlaying(afterPlayingFunc* afterFunc, void* afterClientData,
			CommonPlay *cpObj);

	unsigned numActiveSubsessions() const {
		return fNumSubsessions;
	}

private:
	AVIFileSink(UsageEnvironment& env, MediaSession& inputSession,
			char const* outputFileName, unsigned bufferSize,
			unsigned short movieWidth, unsigned short movieHeight,
			unsigned movieFPS, Boolean packetLossCompensate, CommonPlay *cpObj);
	// called only by createNew()
	virtual ~AVIFileSink();

	Boolean continuePlaying();
	static void afterGettingFrame(void* clientData, unsigned frameSize,
			unsigned numTruncatedBytes, struct timeval presentationTime,
			unsigned durationInMicroseconds);
	static void onSourceClosure(void* clientData);
	void onSourceClosure1();
	static void onRTCPBye(void* clientData, CommonPlay *cpObj);
	void addIndexRecord(class AVIIndexRecord* newIndexRecord);
	void completeOutputFile();

private:
	friend class AVISubsessionIOState;
	MediaSession& fInputSession;
	FILE* fOutFid;
	class AVIIndexRecord *fIndexRecordsHead, *fIndexRecordsTail;
	unsigned fNumIndexRecords;
	unsigned fBufferSize;
	Boolean fPacketLossCompensate;
	Boolean fAreCurrentlyBeingPlayed;
	afterPlayingFunc* fAfterFunc;
	void* fAfterClientData;
	unsigned fNumSubsessions;
	unsigned fNumBytesWritten;
	struct timeval fStartTime;
	Boolean fHaveCompletedOutputFile;

private:
	///// Definitions specific to the AVI file format:

	unsigned addWord(unsigned word); // outputs "word" in little-endian order
	unsigned addHalfWord(unsigned short halfWord);
	unsigned addByte(unsigned char byte) {
		putc(byte, fOutFid);
		return 1;
	}
	unsigned addZeroWords(unsigned numWords);
	unsigned add4ByteString(char const* str);
	void setWord(unsigned filePosn, unsigned size);

	// Define member functions for outputting various types of file header:
#define _header(name) unsigned addFileHeader_##name()
	_header(AVI);_header(hdrl);_header(avih);_header(strl);_header(strh);_header(strf);_header(JUNK);
//        _header(JUNK);
	_header(movi);
private:
	unsigned short fMovieWidth, fMovieHeight;
	unsigned fMovieFPS;
	unsigned fRIFFSizePosition, fRIFFSizeValue;
	unsigned fAVIHMaxBytesPerSecondPosition;
	unsigned fAVIHFrameCountPosition;
	unsigned fMoviSizePosition, fMoviSizeValue;
	class AVISubsessionIOState* fCurrentIOState;
	unsigned fJunkNumber;
	CommonPlay *fcpObj;
};

#endif /* INCLUDE_AVIFILESINK_H_ */
