/*
 * H264VideoRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H264VIDEORTPSOURCE_H_
#define INCLUDE_H264VIDEORTPSOURCE_H_

#include "MultiFramedRTPSource.h"

class H264VideoRTPSource: public MultiFramedRTPSource {
public:
	static H264VideoRTPSource*
	createNew(UsageEnvironment& env, CommonPlay *cpObj,Groupsock* RTPgs,
			unsigned char rtpPayloadFormat, unsigned rtpTimestampFrequency =
					90000);

protected:
	H264VideoRTPSource(UsageEnvironment& env,CommonPlay *cpObj, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat, unsigned rtpTimestampFrequency);
	// called only by createNew()

	virtual ~H264VideoRTPSource();

protected:
	// redefined virtual functions:
	virtual Boolean processSpecialHeader(BufferedPacket* packet,
			unsigned& resultSpecialHeaderSize);
	virtual char const* MIMEtype() const;

private:
	friend class H264BufferedPacket;
	unsigned char fCurPacketNALUnitType;
};

class SPropRecord {
public:
	~SPropRecord() {
		delete[] sPropBytes;
	}

	unsigned sPropLength; // in bytes
	unsigned char* sPropBytes;
};

SPropRecord* parseSPropParameterSets(char const* sPropParameterSetsStr,
// result parameter:
		unsigned& numSPropRecords);
// Returns the binary value of each 'parameter set' specified in a
// "sprop-parameter-sets" string (in the SDP description for a H.264/RTP stream).
// The value is returned as an array (length "numSPropRecords") of "SPropRecord"s.
// This array is dynamically allocated by this routine, and must be delete[]d by the caller.

#endif /* INCLUDE_H264VIDEORTPSOURCE_H_ */
