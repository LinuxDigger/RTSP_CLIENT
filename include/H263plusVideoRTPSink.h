/*
 * H263plusVideoRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H263PLUSVIDEORTPSINK_H_
#define INCLUDE_H263PLUSVIDEORTPSINK_H_

#include "VideoRTPSink.h"
class H263plusVideoRTPSink: public VideoRTPSink {
public:
	static H263plusVideoRTPSink* createNew(UsageEnvironment& env,
			CommonPlay *cpObj, Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
			u_int32_t rtpTimestampFrequency = 90000);

protected:
	H263plusVideoRTPSink(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* RTPgs, DP_U8 rtpPayloadFormat,
			u_int32_t rtpTimestampFrequency);
	// called only by createNew()

	virtual ~H263plusVideoRTPSink();

private:
	// redefined virtual functions:
	virtual void doSpecialFrameHandling(unsigned fragmentationOffset,
			DP_U8* frameStart, unsigned numBytesInFrame,
			struct timeval framePresentationTime, unsigned numRemainingBytes);
	virtual Boolean frameCanAppearAfterPacketStart(
			DP_U8 const* frameStart, unsigned numBytesInFrame) const;
	virtual unsigned specialHeaderSize() const;
};

#endif /* INCLUDE_H263PLUSVIDEORTPSINK_H_ */
