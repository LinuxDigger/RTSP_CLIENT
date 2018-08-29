/*
 * BasicUDPSink.h
 *
 *  Created on: Aug 10, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_BASICUDPSINK_H_
#define INCLUDE_BASICUDPSINK_H_

#include "MediaSink.h"
#include "GroupSock.h"

class BasicUDPSink: public MediaSink {
public:
	static BasicUDPSink* createNew(UsageEnvironment& env, Groupsock* gs,
			unsigned maxPayloadSize = 1450);
protected:
	BasicUDPSink(UsageEnvironment& env, Groupsock* gs, unsigned maxPayloadSize);
	// called only by createNew()
	virtual ~BasicUDPSink();

private:
	// redefined virtual functions:
	virtual Boolean continuePlaying();

private:
	void continuePlaying1();

	static void afterGettingFrame(void* clientData, unsigned frameSize,
			unsigned numTruncatedBytes, struct timeval presentationTime,
			unsigned durationInMicroseconds);
	void afterGettingFrame1(unsigned frameSize, unsigned numTruncatedBytes,
			unsigned durationInMicroseconds);

	static void sendNext(void* firstArg);

private:
	Groupsock* fGS;
	unsigned fMaxPayloadSize;
	unsigned char* fOutputBuffer;
	struct timeval fNextSendTime;
};

#endif /* INCLUDE_BASICUDPSINK_H_ */
