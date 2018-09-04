/*
 * MPEG4VideoStreamFramer.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG4VIDEOSTREAMFRAMER_H_
#define INCLUDE_MPEG4VIDEOSTREAMFRAMER_H_

#include "MPEGVideoStreamFramer.h"

class MPEG4VideoStreamFramer: public MPEGVideoStreamFramer {
public:
	static MPEG4VideoStreamFramer*
	createNew(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource);

	u_int8_t profile_and_level_indication() const {
		return fProfileAndLevelIndication;
	}

	unsigned char* getConfigBytes(unsigned& numBytes) const;

	void setConfigInfo(u_int8_t profileAndLevelIndication,
			char const* configStr);
	// Assigns the "profile_and_level_indication" number, and the 'config' bytes.
	// If this function is not called, then this data is only assigned later, when it appears in the input stream.

protected:
	MPEG4VideoStreamFramer(UsageEnvironment& env, CommonPlay *cpObj,
			FramedSource* inputSource, Boolean createParser = True);
	// called only by createNew(), or by subclass constructors
	virtual ~MPEG4VideoStreamFramer();

	void startNewConfig();
	void appendToNewConfig(unsigned char* newConfigBytes, unsigned numNewBytes);
	void completeNewConfig();

private:
	// redefined virtual functions:
	virtual Boolean isMPEG4VideoStreamFramer() const;

protected:
	u_int8_t fProfileAndLevelIndication;
	unsigned char* fConfigBytes;
	unsigned fNumConfigBytes;

private:
	unsigned char* fNewConfigBytes;
	unsigned fNumNewConfigBytes;
	friend class MPEG4VideoStreamParser;
	// hack
};

#endif /* INCLUDE_MPEG4VIDEOSTREAMFRAMER_H_ */
