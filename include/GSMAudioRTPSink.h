/*
 * GSMAudioRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_GSMAUDIORTPSINK_H_
#define INCLUDE_GSMAUDIORTPSINK_H_

#include "AudioRTPSink.h"

class GSMAudioRTPSink: public AudioRTPSink {
public:
	static GSMAudioRTPSink* createNew(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* RTPgs);

protected:
	GSMAudioRTPSink(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs);
	// called only by createNew()

	virtual ~GSMAudioRTPSink();

private:
	// redefined virtual functions:
	virtual Boolean frameCanAppearAfterPacketStart(
			DP_U8 const* frameStart, unsigned numBytesInFrame) const;
};

#endif /* INCLUDE_GSMAUDIORTPSINK_H_ */
