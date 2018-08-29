/*
 * MPEG4LATMAudioRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG4LATMAUDIORTPSINK_H_
#define INCLUDE_MPEG4LATMAUDIORTPSINK_H_

#include "AudioRTPSink.h"

class MPEG4LATMAudioRTPSink: public AudioRTPSink {
public:
	static MPEG4LATMAudioRTPSink* createNew(UsageEnvironment& env,
			Groupsock* RTPgs, unsigned char rtpPayloadFormat,
			u_int32_t rtpTimestampFrequency, char const* streamMuxConfigString,
			unsigned numChannels, Boolean allowMultipleFramesPerPacket = False);

protected:
	MPEG4LATMAudioRTPSink(UsageEnvironment& env, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat, u_int32_t rtpTimestampFrequency,
			char const* streamMuxConfigString, unsigned numChannels,
			Boolean allowMultipleFramesPerPacket);
	// called only by createNew()

	virtual ~MPEG4LATMAudioRTPSink();

private:
	// redefined virtual functions:
	virtual void doSpecialFrameHandling(unsigned fragmentationOffset,
			unsigned char* frameStart, unsigned numBytesInFrame,
			struct timeval framePresentationTime, unsigned numRemainingBytes);
	virtual Boolean
	frameCanAppearAfterPacketStart(unsigned char const* frameStart,
			unsigned numBytesInFrame) const;

	virtual char const* auxSDPLine(); // for the "a=fmtp:" SDP line

private:
	char const* fStreamMuxConfigString;
	char* fFmtpSDPLine;
	Boolean fAllowMultipleFramesPerPacket;
};

#endif /* INCLUDE_MPEG4LATMAUDIORTPSINK_H_ */
