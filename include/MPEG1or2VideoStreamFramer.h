/*
 * MPEG1or2VideoStreamFramer.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG1OR2VIDEOSTREAMFRAMER_H_
#define INCLUDE_MPEG1OR2VIDEOSTREAMFRAMER_H_

#include "MPEGVideoStreamFramer.h"

class MPEG1or2VideoStreamFramer: public MPEGVideoStreamFramer {
public:
	static MPEG1or2VideoStreamFramer*
	createNew(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource, Boolean iFramesOnly = False,
			double vshPeriod = 5.0
			/* how often (in seconds) to inject a Video_Sequence_Header,
			 if one doesn't already appear in the stream */);

protected:
	MPEG1or2VideoStreamFramer(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource, Boolean iFramesOnly, double vshPeriod,
			Boolean createParser = True);
	// called only by createNew(), or by subclass constructors
	virtual ~MPEG1or2VideoStreamFramer();

private:
	// redefined virtual functions:
	virtual Boolean isMPEG1or2VideoStreamFramer() const;

private:
	double getCurrentPTS() const;

	friend class MPEG1or2VideoStreamParser;
	// hack
};

#endif /* INCLUDE_MPEG1OR2VIDEOSTREAMFRAMER_H_ */
