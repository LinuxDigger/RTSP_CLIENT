/*
 * TheoraVideoRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_THEORAVIDEORTPSOURCE_H_
#define INCLUDE_THEORAVIDEORTPSOURCE_H_

#include "MultiFramedRTPSource.h"

class TheoraVideoRTPSource: public MultiFramedRTPSource {
public:
	static TheoraVideoRTPSource*
	createNew(UsageEnvironment& env,CommonPlay *cpObj, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat);

	u_int32_t curPacketIdent() const {
		return fCurPacketIdent;
	} // The current "Ident" field; only the low-order 24 bits are used

protected:
	TheoraVideoRTPSource(UsageEnvironment& env,CommonPlay *cpObj, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat);
	// called only by createNew()

	virtual ~TheoraVideoRTPSource();

protected:
	// redefined virtual functions:
	virtual Boolean processSpecialHeader(BufferedPacket* packet,
			unsigned& resultSpecialHeaderSize);
	virtual char const* MIMEtype() const;

private:
	u_int32_t fCurPacketIdent; // only the low-order 24 bits are used
};

#endif /* INCLUDE_THEORAVIDEORTPSOURCE_H_ */
