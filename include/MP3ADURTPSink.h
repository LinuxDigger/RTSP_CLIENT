/*
 * MP3ADURTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MP3ADURTPSINK_H_
#define INCLUDE_MP3ADURTPSINK_H_

#include "AudioRTPSink.h"

class MP3ADURTPSink: public AudioRTPSink {
public:
	static MP3ADURTPSink* createNew(UsageEnvironment& env, Groupsock* RTPgs,
			unsigned char RTPPayloadType);

protected:
	virtual ~MP3ADURTPSink();

private:
	MP3ADURTPSink(UsageEnvironment& env, Groupsock* RTPgs,
			unsigned char RTPPayloadType);
	// called only by createNew()

private:
	// Redefined virtual functions:
	virtual void doSpecialFrameHandling(unsigned fragmentationOffset,
			unsigned char* frameStart, unsigned numBytesInFrame,
			struct timeval framePresentationTime, unsigned numRemainingBytes);
	virtual unsigned specialHeaderSize() const;

private:
	unsigned fCurADUSize; // used when fragmenting over multiple RTP packets
};

#endif /* INCLUDE_MP3ADURTPSINK_H_ */
