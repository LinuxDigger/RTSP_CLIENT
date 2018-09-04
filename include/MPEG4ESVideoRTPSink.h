/*
 * MPEG4ESVideoRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG4ESVIDEORTPSINK_H_
#define INCLUDE_MPEG4ESVIDEORTPSINK_H_

#include "VideoRTPSink.h"

class MPEG4ESVideoRTPSink: public VideoRTPSink {
public:
	static MPEG4ESVideoRTPSink* createNew(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* RTPgs, unsigned char rtpPayloadFormat,
			u_int32_t rtpTimestampFrequency = 90000);
	static MPEG4ESVideoRTPSink* createNew(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* RTPgs, unsigned char rtpPayloadFormat,
			u_int32_t rtpTimestampFrequency, u_int8_t profileAndLevelIndication,
			char const* configStr);
	// an optional variant of "createNew()", useful if we know, in advance, the stream's 'configuration' info.

protected:
	MPEG4ESVideoRTPSink(UsageEnvironment& env,  CommonPlay *cpObj,Groupsock* RTPgs,
			unsigned char rtpPayloadFormat, u_int32_t rtpTimestampFrequency,
			u_int8_t profileAndLevelIndication = 0,
			char const* configStr = NULL);
	// called only by createNew()

	virtual ~MPEG4ESVideoRTPSink();

protected:
	// redefined virtual functions:
	virtual Boolean sourceIsCompatibleWithUs(MediaSource& source);

	virtual void doSpecialFrameHandling(unsigned fragmentationOffset,
			unsigned char* frameStart, unsigned numBytesInFrame,
			struct timeval framePresentationTime, unsigned numRemainingBytes);
	virtual Boolean allowFragmentationAfterStart() const;
	virtual Boolean
	frameCanAppearAfterPacketStart(unsigned char const* frameStart,
			unsigned numBytesInFrame) const;

	virtual char const* auxSDPLine();

protected:
	Boolean fVOPIsPresent;

private:
	u_int8_t fProfileAndLevelIndication;
	unsigned char* fConfigBytes;
	unsigned fNumConfigBytes;

	char* fFmtpSDPLine;
};

#endif /* INCLUDE_MPEG4ESVIDEORTPSINK_H_ */
