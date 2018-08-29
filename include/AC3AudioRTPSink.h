/*
 * AC3AudioRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_AC3AUDIORTPSINK_H_
#define INCLUDE_AC3AUDIORTPSINK_H_


#include "AudioRTPSink.h"

class AC3AudioRTPSink: public AudioRTPSink {
public:
  static AC3AudioRTPSink* createNew(UsageEnvironment& env,
				    Groupsock* RTPgs,
				    u_int8_t rtpPayloadFormat,
				    u_int32_t rtpTimestampFrequency);

protected:
  AC3AudioRTPSink(UsageEnvironment& env, Groupsock* RTPgs,
		  u_int8_t rtpPayloadFormat,
		  u_int32_t rtpTimestampFrequency);
	// called only by createNew()

  virtual ~AC3AudioRTPSink();

private: // redefined virtual functions:
  virtual Boolean frameCanAppearAfterPacketStart(unsigned char const* frameStart,
						 unsigned numBytesInFrame) const;
  virtual void doSpecialFrameHandling(unsigned fragmentationOffset,
                                      unsigned char* frameStart,
                                      unsigned numBytesInFrame,
                                      struct timeval framePresentationTime,
                                      unsigned numRemainingBytes);
  virtual unsigned specialHeaderSize() const;

private:
  unsigned char fTotNumFragmentsUsed; // used only if a frame gets fragmented across multiple packets
};



#endif /* INCLUDE_AC3AUDIORTPSINK_H_ */
