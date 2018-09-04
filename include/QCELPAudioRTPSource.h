/*
 * QCELPAudioRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_QCELPAUDIORTPSOURCE_H_
#define INCLUDE_QCELPAUDIORTPSOURCE_H_

#ifndef _RTP_SOURCE_HH
#include "RTPSource.h"
#endif

class QCELPAudioRTPSource {
public:
	static FramedSource* createNew(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* RTPgs, RTPSource*& resultRTPSource,
			unsigned char rtpPayloadFormat = 12,
			unsigned rtpTimestampFrequency = 8000);
	// This returns a source to read from, but "resultRTPSource" will
	// point to RTP-related state.
};

#endif /* INCLUDE_QCELPAUDIORTPSOURCE_H_ */
