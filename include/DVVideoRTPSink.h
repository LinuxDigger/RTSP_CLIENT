/*
 * DVVideoRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_DVVIDEORTPSINK_H_
#define INCLUDE_DVVIDEORTPSINK_H_

#include "VideoRTPSink.h"
#include "DVVideoStreamFramer.h"

class DVVideoRTPSink: public VideoRTPSink {
public:
	static DVVideoRTPSink* createNew(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* RTPgs, unsigned char rtpPayloadFormat);
	char const* auxSDPLineFromFramer(DVVideoStreamFramer* framerSource);

protected:
	DVVideoRTPSink(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat);
	// called only by createNew()

	virtual ~DVVideoRTPSink();

private:
	// redefined virtual functions:
	virtual Boolean sourceIsCompatibleWithUs(MediaSource& source);
	virtual void doSpecialFrameHandling(unsigned fragmentationOffset,
			unsigned char* frameStart, unsigned numBytesInFrame,
			struct timeval framePresentationTime, unsigned numRemainingBytes);
	virtual unsigned computeOverflowForNewFrame(unsigned newFrameSize) const;
	virtual char const* auxSDPLine();

private:
	char* fFmtpSDPLine;
};

#endif /* INCLUDE_DVVIDEORTPSINK_H_ */
