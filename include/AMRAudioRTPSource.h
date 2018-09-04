/*
 * ARMAudioRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_AMRAUDIORTPSOURCE_H_
#define INCLUDE_AMRAUDIORTPSOURCE_H_

#include <AMRAudioSource.h>
#include "RTPSource.h"

class AMRAudioRTPSource {
public:
	static AMRAudioSource* createNew(UsageEnvironment& env, CommonPlay *cpObj,
			Groupsock* RTPgs, RTPSource*& resultRTPSource,
			unsigned char rtpPayloadFormat, Boolean isWideband = False,
			unsigned numChannels = 1, Boolean isOctetAligned = True,
			unsigned interleaving = 0,
			// relevant only if "isOctetAligned"
			// The maximum # of frame-blocks in a group
			// 0 means: no interleaving
			Boolean robustSortingOrder = False,
			// relevant only if "isOctetAligned"
			Boolean CRCsArePresent = False
			// relevant only if "isOctetAligned"
			);
	// This returns a source to read from, but "resultRTPSource" will
	// point to RTP-related state.
};

#endif /* INCLUDE_AMRAUDIORTPSOURCE_H_ */
