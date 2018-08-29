/*
 * TextRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_TEXTRTPSINK_H_
#define INCLUDE_TEXTRTPSINK_H_

#include "MultiFramedRTPSink.h"

class TextRTPSink: public MultiFramedRTPSink {
protected:
	TextRTPSink(UsageEnvironment& env, Groupsock* rtpgs,
			unsigned char rtpPayloadType, unsigned rtpTimestampFrequency,
			char const* rtpPayloadFormatName);
	// (we're an abstract base class)
	virtual ~TextRTPSink();

private:
	// redefined virtual functions:
	virtual char const* sdpMediaType() const;
};

#endif /* INCLUDE_TEXTRTPSINK_H_ */
