/*
 * VP8VideoRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_VP8VIDEORTPSOURCE_H_
#define INCLUDE_VP8VIDEORTPSOURCE_H_

#include "MultiFramedRTPSource.h"

class VP8VideoRTPSource: public MultiFramedRTPSource {
public:
	static VP8VideoRTPSource*
	createNew(UsageEnvironment& env, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat, unsigned rtpTimestampFrequency =
					90000);

protected:
	VP8VideoRTPSource(UsageEnvironment& env, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat, unsigned rtpTimestampFrequency);
	// called only by createNew()

	virtual ~VP8VideoRTPSource();

protected:
	// redefined virtual functions:
	virtual Boolean processSpecialHeader(BufferedPacket* packet,
			unsigned& resultSpecialHeaderSize);
	virtual char const* MIMEtype() const;
};

#endif /* INCLUDE_VP8VIDEORTPSOURCE_H_ */
