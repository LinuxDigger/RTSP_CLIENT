/*
 * ArmAudioSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_AMRAUDIOSOURCE_H_
#define INCLUDE_AMRAUDIOSOURCE_H_

#include "FramedSource.h"

class AMRAudioSource: public FramedSource {
public:
	Boolean isWideband() const {
		return fIsWideband;
	}
	unsigned numChannels() const {
		return fNumChannels;
	}

	u_int8_t lastFrameHeader() const {
		return fLastFrameHeader;
	}
	// The frame header for the most recently read frame (RFC 4867, sec. 5.3)

protected:
	AMRAudioSource(UsageEnvironment& env, Boolean isWideband,
			unsigned numChannels);
	// virtual base class
	virtual ~AMRAudioSource();

private:
	// redefined virtual functions:
	virtual char const* MIMEtype() const;
	virtual Boolean isAMRAudioSource() const;

protected:
	Boolean fIsWideband;
	unsigned fNumChannels;
	u_int8_t fLastFrameHeader;
};

#endif /* INCLUDE_AMRAUDIOSOURCE_H_ */
