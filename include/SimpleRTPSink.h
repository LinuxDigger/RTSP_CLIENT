/*
 * SimpleRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_SIMPLERTPSINK_H_
#define INCLUDE_SIMPLERTPSINK_H_

#include "MultiFramedRTPSink.h"

class SimpleRTPSink: public MultiFramedRTPSink {
public:
	static SimpleRTPSink*
	createNew(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat, unsigned rtpTimestampFrequency,
			char const* sdpMediaTypeString, char const* rtpPayloadFormatName,
			unsigned numChannels = 1, Boolean allowMultipleFramesPerPacket =
					True, Boolean doNormalMBitRule = True);
	// "doNormalMBitRule" means: If the medium (i.e., "sdpMediaTypeString") is other than "audio", set the RTP "M" bit
	// on each outgoing packet iff it contains the last (or only) fragment of a frame.
	// Otherwise (i.e., if "doNormalMBitRule" is False, or the medium is "audio"), leave the "M" bit unset.

	void setMBitOnNextPacket() {
		fSetMBitOnNextPacket = True;
	} // hack for optionally setting the RTP 'M' bit from outside the class

protected:
	SimpleRTPSink(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat, unsigned rtpTimestampFrequency,
			char const* sdpMediaTypeString, char const* rtpPayloadFormatName,
			unsigned numChannels, Boolean allowMultipleFramesPerPacket,
			Boolean doNormalMBitRule);
	// called only by createNew()

	virtual ~SimpleRTPSink();

protected:
	// redefined virtual functions
	virtual void doSpecialFrameHandling(unsigned fragmentationOffset,
			unsigned char* frameStart, unsigned numBytesInFrame,
			struct timeval framePresentationTime, unsigned numRemainingBytes);
	virtual Boolean frameCanAppearAfterPacketStart(
			unsigned char const* frameStart, unsigned numBytesInFrame) const;
	virtual char const* sdpMediaType() const;

private:
	char const* fSDPMediaTypeString;
	Boolean fAllowMultipleFramesPerPacket;
	Boolean fSetMBitOnLastFrames, fSetMBitOnNextPacket;
};

#endif /* INCLUDE_SIMPLERTPSINK_H_ */
