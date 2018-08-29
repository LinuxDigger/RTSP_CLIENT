/*
 * MPEG1or2VideoRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG1OR2VIDEORTPSINK_H_
#define INCLUDE_MPEG1OR2VIDEORTPSINK_H_

#include "VideoRTPSink.h"

class MPEG1or2VideoRTPSink: public VideoRTPSink {
public:
	static MPEG1or2VideoRTPSink* createNew(UsageEnvironment& env,
			Groupsock* RTPgs);

protected:
	MPEG1or2VideoRTPSink(UsageEnvironment& env, Groupsock* RTPgs);
	// called only by createNew()

	virtual ~MPEG1or2VideoRTPSink();

private:
	// redefined virtual functions:
	virtual Boolean sourceIsCompatibleWithUs(MediaSource& source);

	virtual void doSpecialFrameHandling(unsigned fragmentationOffset,
			unsigned char* frameStart, unsigned numBytesInFrame,
			struct timeval framePresentationTime, unsigned numRemainingBytes);
	virtual Boolean allowFragmentationAfterStart() const;
	virtual Boolean frameCanAppearAfterPacketStart(
			unsigned char const* frameStart, unsigned numBytesInFrame) const;
	virtual unsigned specialHeaderSize() const;

private:
	// MPEG video-specific state, used to decide how to fill out the
	// video-specific header, and when to include multiple 'frames' in a
	// single outgoing RTP packet.  Eventually we should somehow get this
	// state from the source (MPEG1or2VideoStreamFramer) instead, as the source
	// already has this info itself.
	struct {
		unsigned temporal_reference;
		unsigned char picture_coding_type;
		unsigned char vector_code_bits; // FBV,BFC,FFV,FFC from RFC 2250, sec. 3.4
	} fPictureState;
	Boolean fPreviousFrameWasSlice;
	// used to implement frameCanAppearAfterPacketStart()
	Boolean fSequenceHeaderPresent;
	Boolean fPacketBeginsSlice, fPacketEndsSlice;
};

#endif /* INCLUDE_MPEG1OR2VIDEORTPSINK_H_ */
