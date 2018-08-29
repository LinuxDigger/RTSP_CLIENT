/*
 * MPEG1or2Demux.h
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG1OR2DEMUX_H_
#define INCLUDE_MPEG1OR2DEMUX_H_

#include "FramedSource.h"

class MPEG1or2DemuxedElementaryStream;
// forward

class MPEG1or2Demux: public Medium {
public:
	static MPEG1or2Demux* createNew(UsageEnvironment& env,
			FramedSource* inputSource, Boolean reclaimWhenLastESDies = False);
	// If "reclaimWhenLastESDies" is True, the the demux is deleted when
	// all "MPEG1or2DemuxedElementaryStream"s that we created get deleted.

	MPEG1or2DemuxedElementaryStream* newElementaryStream(u_int8_t streamIdTag);

	// Specialized versions of the above for audio and video:
	MPEG1or2DemuxedElementaryStream* newAudioStream();
	MPEG1or2DemuxedElementaryStream* newVideoStream();

	// A hack for getting raw, undemuxed PES packets from the Program Stream:
	MPEG1or2DemuxedElementaryStream* newRawPESStream();

	void getNextFrame(u_int8_t streamIdTag, unsigned char* to, unsigned maxSize,
			FramedSource::afterGettingFunc* afterGettingFunc,
			void* afterGettingClientData,
			FramedSource::onCloseFunc* onCloseFunc, void* onCloseClientData);
	// similar to FramedSource::getNextFrame(), except that it also
	// takes a stream id tag as parameter.

	void stopGettingFrames(u_int8_t streamIdTag);
	// similar to FramedSource::stopGettingFrames(), except that it also
	// takes a stream id tag as parameter.

	static void handleClosure(void* clientData);
	// This should be called (on ourself) if the source is discovered
	// to be closed (i.e., no longer readable)

	FramedSource* inputSource() const {
		return fInputSource;
	}

	class SCR {
	public:
		SCR();

		u_int8_t highBit;
		u_int32_t remainingBits;
		u_int16_t extension;

		Boolean isValid;
	};
	SCR& lastSeenSCR() {
		return fLastSeenSCR;
	}

	unsigned char mpegVersion() const {
		return fMPEGversion;
	}

	void flushInput(); // should be called before any 'seek' on the underlying source

private:
	MPEG1or2Demux(UsageEnvironment& env, FramedSource* inputSource,
			Boolean reclaimWhenLastESDies);
	// called only by createNew()
	virtual ~MPEG1or2Demux();

	void registerReadInterest(u_int8_t streamIdTag, unsigned char* to,
			unsigned maxSize, FramedSource::afterGettingFunc* afterGettingFunc,
			void* afterGettingClientData,
			FramedSource::onCloseFunc* onCloseFunc, void* onCloseClientData);

	Boolean useSavedData(u_int8_t streamIdTag, unsigned char* to,
			unsigned maxSize, FramedSource::afterGettingFunc* afterGettingFunc,
			void* afterGettingClientData);

	static void continueReadProcessing(void* clientData, unsigned char* ptr,
			unsigned size, struct timeval presentationTime);
	void continueReadProcessing();

private:
	friend class MPEG1or2DemuxedElementaryStream;
	void noteElementaryStreamDeletion(MPEG1or2DemuxedElementaryStream* es);

private:
	FramedSource* fInputSource;
	SCR fLastSeenSCR;
	unsigned char fMPEGversion;

	unsigned char fNextAudioStreamNumber;
	unsigned char fNextVideoStreamNumber;
	Boolean fReclaimWhenLastESDies;
	unsigned fNumOutstandingESs;

	// A descriptor for each possible stream id tag:
	typedef struct OutputDescriptor {
		// input parameters
		unsigned char* to;
		unsigned maxSize;
		FramedSource::afterGettingFunc* fAfterGettingFunc;
		void* afterGettingClientData;
		FramedSource::onCloseFunc* fOnCloseFunc;
		void* onCloseClientData;

		// output parameters
		unsigned frameSize;
		struct timeval presentationTime;
		class SavedData; // forward
		SavedData* savedDataHead;
		SavedData* savedDataTail;
		unsigned savedDataTotalSize;

		// status parameters
		Boolean isPotentiallyReadable;
		Boolean isCurrentlyActive;
		Boolean isCurrentlyAwaitingData;
	} OutputDescriptor_t;
	OutputDescriptor_t fOutput[256];

	unsigned fNumPendingReads;
	Boolean fHaveUndeliveredData;

private:
	// parsing state
	class MPEGProgramStreamParser* fParser;
	friend class MPEGProgramStreamParser;
	// hack
};

#endif /* INCLUDE_MPEG1OR2DEMUX_H_ */
