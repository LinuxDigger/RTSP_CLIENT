/*
 * H261VideoRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H261VIDEORTPSOURCE_H_
#define INCLUDE_H261VIDEORTPSOURCE_H_

#include "MultiFramedRTPSource.h"

class H261VideoRTPSource: public MultiFramedRTPSource {
public:
	static H261VideoRTPSource*
	createNew(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat = 31,
			unsigned rtpTimestampFrequency = 90000);

	u_int32_t lastSpecialHeader() const {
		return fLastSpecialHeader;
	}

protected:
	virtual ~H261VideoRTPSource();

private:
	H261VideoRTPSource(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* RTPgs, unsigned char rtpPayloadFormat,
			unsigned rtpTimestampFrequency);
	// called only by createNew()

private:
	// redefined virtual functions:
	virtual Boolean processSpecialHeader(BufferedPacket* packet,
			unsigned& resultSpecialHeaderSize);
	virtual char const* MIMEtype() const;

private:
	u_int32_t fLastSpecialHeader;
};

#endif /* INCLUDE_H261VIDEORTPSOURCE_H_ */
