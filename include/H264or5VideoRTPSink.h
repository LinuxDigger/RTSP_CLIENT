/*
 * H264or5VideoRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H264OR5VIDEORTPSINK_H_
#define INCLUDE_H264OR5VIDEORTPSINK_H_

#include "VideoRTPSink.h"
#include "FramedFilter.h"

class H264or5VideoRTPSink: public VideoRTPSink {
protected:
	H264or5VideoRTPSink(
			int hNumber, // 264 or 265
			UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat, u_int8_t const* vps = NULL,
			unsigned vpsSize = 0, u_int8_t const* sps = NULL, unsigned spsSize =
					0, u_int8_t const* pps = NULL, unsigned ppsSize = 0);
	// we're an abstrace base class
	virtual ~H264or5VideoRTPSink();

private:
	// redefined virtual functions:
	virtual Boolean continuePlaying();
	virtual void doSpecialFrameHandling(unsigned fragmentationOffset,
			unsigned char* frameStart, unsigned numBytesInFrame,
			struct timeval framePresentationTime, unsigned numRemainingBytes);
	virtual Boolean frameCanAppearAfterPacketStart(
			unsigned char const* frameStart, unsigned numBytesInFrame) const;

protected:
	int fHNumber;
	FramedFilter* fOurFragmenter;
	char* fFmtpSDPLine;
	u_int8_t* fVPS;
	unsigned fVPSSize;
	u_int8_t* fSPS;
	unsigned fSPSSize;
	u_int8_t* fPPS;
	unsigned fPPSSize;
};

#endif /* INCLUDE_H264OR5VIDEORTPSINK_H_ */
