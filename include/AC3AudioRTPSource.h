/*
 * AC3AudioRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_AC3AUDIORTPSOURCE_H_
#define INCLUDE_AC3AUDIORTPSOURCE_H_

#include "MultiFramedRTPSource.h"

class AC3AudioRTPSource: public MultiFramedRTPSource {
public:
	static AC3AudioRTPSource*
	createNew(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
			DP_U8 rtpPayloadFormat, unsigned rtpTimestampFrequency);

protected:
	virtual ~AC3AudioRTPSource();

private:
	AC3AudioRTPSource(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
			DP_U8 rtpPayloadFormat, unsigned rtpTimestampFrequency);
	// called only by createNew()

private:
	// redefined virtual functions:
	virtual Boolean processSpecialHeader(BufferedPacket* packet,
			unsigned& resultSpecialHeaderSize);
	virtual char const* MIMEtype() const;
};

#endif /* INCLUDE_AC3AUDIORTPSOURCE_H_ */
