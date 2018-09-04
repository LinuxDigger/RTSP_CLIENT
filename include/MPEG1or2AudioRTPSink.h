/*
 * MPEG1or2AudioRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG1OR2AUDIORTPSINK_H_
#define INCLUDE_MPEG1OR2AUDIORTPSINK_H_

#include "AudioRTPSink.h"

class MPEG1or2AudioRTPSink: public AudioRTPSink {
public:
	static MPEG1or2AudioRTPSink* createNew(UsageEnvironment& env,
			CommonPlay *cpObj, Groupsock* RTPgs);

protected:
	MPEG1or2AudioRTPSink(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* RTPgs);
	// called only by createNew()

	virtual ~MPEG1or2AudioRTPSink();

private:
	// redefined virtual functions:
	virtual void doSpecialFrameHandling(unsigned fragmentationOffset,
			unsigned char* frameStart, unsigned numBytesInFrame,
			struct timeval framePresentationTime, unsigned numRemainingBytes);
	virtual unsigned specialHeaderSize() const;
};

#endif /* INCLUDE_MPEG1OR2AUDIORTPSINK_H_ */
