/*
 * H265VideoRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H265VIDEORTPSOURCE_H_
#define INCLUDE_H265VIDEORTPSOURCE_H_

#include "MultiFramedRTPSource.h"

class H265VideoRTPSource: public MultiFramedRTPSource {
public:
	static H265VideoRTPSource*
	createNew(UsageEnvironment& env, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat, Boolean expectDONFields = False,
			unsigned rtpTimestampFrequency = 90000);
	// "expectDONFields" is True iff we expect incoming H.265/RTP packets to contain
	// DONL and DOND fields.  I.e., if "tx-mode == "MST" or sprop-depack-buf-nalus > 0".

	u_int64_t currentNALUnitAbsDon() const {
		return fCurrentNALUnitAbsDon;
	}
	// the 'absolute decoding order number (AbsDon)' for the most-recently delivered NAL unit

protected:
	H265VideoRTPSource(UsageEnvironment& env, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat, Boolean expectDONFields,
			unsigned rtpTimestampFrequency);
	// called only by createNew()

	virtual ~H265VideoRTPSource();

protected:
	// redefined virtual functions:
	virtual Boolean processSpecialHeader(BufferedPacket* packet,
			unsigned& resultSpecialHeaderSize);
	virtual char const* MIMEtype() const;

private:
	void computeAbsDonFromDON(u_int16_t DON);

private:
	friend class H265BufferedPacket;
	Boolean fExpectDONFields;
	unsigned char fCurPacketNALUnitType;
	u_int16_t fPreviousNALUnitDON;
	u_int64_t fCurrentNALUnitAbsDon;
};

#endif /* INCLUDE_H265VIDEORTPSOURCE_H_ */
