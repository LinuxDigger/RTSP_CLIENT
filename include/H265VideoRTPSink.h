/*
 * H265VideoRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H265VIDEORTPSINK_H_
#define INCLUDE_H265VIDEORTPSINK_H_

#include "H264or5VideoRTPSink.h"

class H265VideoRTPSink: public H264or5VideoRTPSink {
public:
	static H265VideoRTPSink*
	createNew(UsageEnvironment& env, Groupsock* RTPgs,
			DP_U8 rtpPayloadFormat);
	static H265VideoRTPSink*
	createNew(UsageEnvironment& env, Groupsock* RTPgs,
			DP_U8 rtpPayloadFormat, u_int8_t const* vps,
			unsigned vpsSize, u_int8_t const* sps, unsigned spsSize,
			u_int8_t const* pps, unsigned ppsSize);
	// an optional variant of "createNew()", useful if we know, in advance,
	// the stream's VPS, SPS and PPS NAL units.
	// This avoids us having to 'pre-read' from the input source in order to get these values.
	static H265VideoRTPSink*
	createNew(UsageEnvironment& env, Groupsock* RTPgs,
			DP_U8 rtpPayloadFormat, char const* sPropVPSStr,
			char const* sPropSPSStr, char const* sPropPPSStr);
	// an optional variant of "createNew()", useful if we know, in advance,
	// the stream's VPS, SPS and PPS NAL units.
	// This avoids us having to 'pre-read' from the input source in order to get these values.

protected:
	H265VideoRTPSink(UsageEnvironment& env, Groupsock* RTPgs,
			DP_U8 rtpPayloadFormat, u_int8_t const* vps = NULL,
			unsigned vpsSize = 0, u_int8_t const* sps = NULL, unsigned spsSize =
					0, u_int8_t const* pps = NULL, unsigned ppsSize = 0);
	// called only by createNew()
	virtual ~H265VideoRTPSink();

protected:
	// redefined virtual functions:
	virtual char const* auxSDPLine();

private:
	// redefined virtual functions:
	virtual Boolean sourceIsCompatibleWithUs(MediaSource& source);
};

#endif /* INCLUDE_H265VIDEORTPSINK_H_ */
