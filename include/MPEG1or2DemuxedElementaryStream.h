/*
 * MPEG1or2DemuxedElementaryStream.h
 *
 *  Created on: Aug 14, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG1OR2DEMUXEDELEMENTARYSTREAM_H_
#define INCLUDE_MPEG1OR2DEMUXEDELEMENTARYSTREAM_H_

#include "MPEG1or2Demux.h"

class MPEG1or2DemuxedElementaryStream: public FramedSource {
public:
	MPEG1or2Demux::SCR lastSeenSCR() const {
		return fLastSeenSCR;
	}

	DP_U8 mpegVersion() const {
		return fMPEGversion;
	}

	MPEG1or2Demux& sourceDemux() const {
		return fOurSourceDemux;
	}

private:
	// We are created only by a MPEG1or2Demux (a friend)
	MPEG1or2DemuxedElementaryStream(UsageEnvironment& env, CommonPlay *cpObj,
			u_int8_t streamIdTag, MPEG1or2Demux& sourceDemux);
	virtual ~MPEG1or2DemuxedElementaryStream();

private:
	// redefined virtual functions:
	virtual void doGetNextFrame();
	virtual void doStopGettingFrames();
	virtual char const* MIMEtype() const;
	virtual unsigned maxFrameSize() const;

private:
	static void afterGettingFrame(void* clientData, unsigned frameSize,
			unsigned numTruncatedBytes, struct timeval presentationTime,
			unsigned durationInMicroseconds);

	void afterGettingFrame1(unsigned frameSize, unsigned numTruncatedBytes,
			struct timeval presentationTime, unsigned durationInMicroseconds);

private:
	u_int8_t fOurStreamIdTag;
	MPEG1or2Demux& fOurSourceDemux;
	char const* fMIMEtype;
	MPEG1or2Demux::SCR fLastSeenSCR;
	DP_U8 fMPEGversion;

	friend class MPEG1or2Demux;
};

#endif /* INCLUDE_MPEG1OR2DEMUXEDELEMENTARYSTREAM_H_ */
