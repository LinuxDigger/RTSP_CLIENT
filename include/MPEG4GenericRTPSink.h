/*
 * MPEG4GenericRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG4GENERICRTPSINK_H_
#define INCLUDE_MPEG4GENERICRTPSINK_H_

#include "MultiFramedRTPSink.h"

class MPEG4GenericRTPSink: public MultiFramedRTPSink {
public:
	static MPEG4GenericRTPSink*
	createNew(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
			u_int8_t rtpPayloadFormat, u_int32_t rtpTimestampFrequency,
			char const* sdpMediaTypeString, char const* mpeg4Mode,
			char const* configString, unsigned numChannels = 1);

protected:
	MPEG4GenericRTPSink(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* RTPgs, u_int8_t rtpPayloadFormat,
			u_int32_t rtpTimestampFrequency, char const* sdpMediaTypeString,
			char const* mpeg4Mode, char const* configString,
			unsigned numChannels);
	// called only by createNew()

	virtual ~MPEG4GenericRTPSink();

private:
	// redefined virtual functions:
	virtual Boolean frameCanAppearAfterPacketStart(
			DP_U8 const* frameStart, unsigned numBytesInFrame) const;
	virtual void doSpecialFrameHandling(unsigned fragmentationOffset,
			DP_U8* frameStart, unsigned numBytesInFrame,
			struct timeval framePresentationTime, unsigned numRemainingBytes);
	virtual unsigned specialHeaderSize() const;

	virtual char const* sdpMediaType() const;

	virtual char const* auxSDPLine(); // for the "a=fmtp:" SDP line

private:
	char const* fSDPMediaTypeString;
	char const* fMPEG4Mode;
	char const* fConfigString;
	char* fFmtpSDPLine;
};

#endif /* INCLUDE_MPEG4GENERICRTPSINK_H_ */
