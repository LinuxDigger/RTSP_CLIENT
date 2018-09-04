/*
 * VP9VideoRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_VP9VIDEORTPSOURCE_H_
#define INCLUDE_VP9VIDEORTPSOURCE_H_

#include "MultiFramedRTPSource.h"

class VP9VideoRTPSource: public MultiFramedRTPSource {
public:
	static VP9VideoRTPSource*
	createNew(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat, unsigned rtpTimestampFrequency =
					90000);

protected:
	VP9VideoRTPSource(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* RTPgs, unsigned char rtpPayloadFormat,
			unsigned rtpTimestampFrequency);
	// called only by createNew()

	virtual ~VP9VideoRTPSource();

protected:
	// redefined virtual functions:
	virtual Boolean processSpecialHeader(BufferedPacket* packet,
			unsigned& resultSpecialHeaderSize);
	virtual char const* MIMEtype() const;
};

#endif /* INCLUDE_VP9VIDEORTPSOURCE_H_ */
