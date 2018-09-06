/*
 * VP8VideoRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_VP8VIDEORTPSINK_H_
#define INCLUDE_VP8VIDEORTPSINK_H_

#include "VideoRTPSink.h"

class VP8VideoRTPSink: public VideoRTPSink {
public:
	static VP8VideoRTPSink* createNew(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* RTPgs, DP_U8 rtpPayloadFormat);

protected:
	VP8VideoRTPSink(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
			DP_U8 rtpPayloadFormat);
	// called only by createNew()

	virtual ~VP8VideoRTPSink();

private:
	// redefined virtual functions:
	virtual void doSpecialFrameHandling(unsigned fragmentationOffset,
			DP_U8* frameStart, unsigned numBytesInFrame,
			struct timeval framePresentationTime, unsigned numRemainingBytes);
	virtual Boolean frameCanAppearAfterPacketStart(
			DP_U8 const* frameStart, unsigned numBytesInFrame) const;
	virtual unsigned specialHeaderSize() const;
};

#endif /* INCLUDE_VP8VIDEORTPSINK_H_ */
