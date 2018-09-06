/*
 * H264VideoRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H264VIDEORTPSINK_H_
#define INCLUDE_H264VIDEORTPSINK_H_

#include "H264or5VideoRTPSink.h"

class H264VideoRTPSink: public H264or5VideoRTPSink {
public:
	static H264VideoRTPSink*
	createNew(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
			DP_U8 rtpPayloadFormat);
	static H264VideoRTPSink*
	createNew(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
			DP_U8 rtpPayloadFormat, u_int8_t const* sps,
			unsigned spsSize, u_int8_t const* pps, unsigned ppsSize);
	// an optional variant of "createNew()", useful if we know, in advance,
	// the stream's SPS and PPS NAL units.
	// This avoids us having to 'pre-read' from the input source in order to get these values.
	static H264VideoRTPSink*
	createNew(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
			DP_U8 rtpPayloadFormat, char const* sPropParameterSetsStr);
	// an optional variant of "createNew()", useful if we know, in advance,
	// the stream's SPS and PPS NAL units.
	// This avoids us having to 'pre-read' from the input source in order to get these values.

protected:
	H264VideoRTPSink(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
			DP_U8 rtpPayloadFormat, u_int8_t const* sps = NULL,
			unsigned spsSize = 0, u_int8_t const* pps = NULL, unsigned ppsSize =
					0);
	// called only by createNew()
	virtual ~H264VideoRTPSink();

protected:
	// redefined virtual functions:
	virtual char const* auxSDPLine();

private:
	// redefined virtual functions:
	virtual Boolean sourceIsCompatibleWithUs(MediaSource& source);
};

#endif /* INCLUDE_H264VIDEORTPSINK_H_ */
