/*
 * SimpleRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_SIMPLERTPSOURCE_H_
#define INCLUDE_SIMPLERTPSOURCE_H_

#include "MultiFramedRTPSource.h"

class SimpleRTPSource: public MultiFramedRTPSource {
public:
	static SimpleRTPSource* createNew(UsageEnvironment& env, CommonPlay *cpObj,Groupsock* RTPgs,
			unsigned char rtpPayloadFormat, unsigned rtpTimestampFrequency,
			char const* mimeTypeString, unsigned offset = 0,
			Boolean doNormalMBitRule = True);
	// "doNormalMBitRule" means: If the medium is not audio, use the RTP "M"
	// bit on each incoming packet to indicate the last (or only) fragment
	// of a frame.  Otherwise (i.e., if "doNormalMBitRule" is False, or the medium is "audio"), the "M" bit is ignored.

protected:
	virtual ~SimpleRTPSource();

protected:
	SimpleRTPSource(UsageEnvironment& env, CommonPlay *cpObj,Groupsock* RTPgs,
			unsigned char rtpPayloadFormat, unsigned rtpTimestampFrequency,
			char const* mimeTypeString, unsigned offset,
			Boolean doNormalMBitRule);
	// called only by createNew()

private:
	// redefined virtual functions:
	virtual Boolean processSpecialHeader(BufferedPacket* packet,
			unsigned& resultSpecialHeaderSize);
	virtual char const* MIMEtype() const;

private:
	char const* fMIMEtypeString;
	unsigned fOffset;
	Boolean fUseMBitForFrameEnd;
};

#endif /* INCLUDE_SIMPLERTPSOURCE_H_ */
