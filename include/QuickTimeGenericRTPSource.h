/*
 * QuickTimeGenericRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_QUICKTIMEGENERICRTPSOURCE_H_
#define INCLUDE_QUICKTIMEGENERICRTPSOURCE_H_

#include "MultiFramedRTPSource.h"

class QuickTimeGenericRTPSource: public MultiFramedRTPSource {
public:
	static QuickTimeGenericRTPSource*
	createNew(UsageEnvironment& env,CommonPlay *cpObj, Groupsock* RTPgs,
			DP_U8 rtpPayloadFormat, unsigned rtpTimestampFrequency,
			char const* mimeTypeString);

	// QuickTime-specific information, set from the QuickTime header
	// in each packet.  This, along with the data following the header,
	// is used by receivers.
	struct QTState {
		char PCK;
		unsigned timescale;
		char* sdAtom;
		unsigned sdAtomSize;
		unsigned short width, height;
		// later add other state as needed #####
	} qtState;

protected:
	virtual ~QuickTimeGenericRTPSource();

private:
	QuickTimeGenericRTPSource(UsageEnvironment& env,CommonPlay *cpObj, Groupsock* RTPgs,
			DP_U8 rtpPayloadFormat, unsigned rtpTimestampFrequency,
			char const* mimeTypeString);
	// called only by createNew()

private:
	// redefined virtual functions:
	virtual Boolean processSpecialHeader(BufferedPacket* packet,
			unsigned& resultSpecialHeaderSize);
	virtual char const* MIMEtype() const;

private:
	char const* fMIMEtypeString;
};

#endif /* INCLUDE_QUICKTIMEGENERICRTPSOURCE_H_ */
