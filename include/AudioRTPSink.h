/*
 * AudioRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_AUDIORTPSINK_H_
#define INCLUDE_AUDIORTPSINK_H_


#include "MultiFramedRTPSink.h"

class AudioRTPSink: public MultiFramedRTPSink {
protected:
  AudioRTPSink(UsageEnvironment& env,CommonPlay *cpObj,
	       Groupsock* rtpgs, DP_U8 rtpPayloadType,
	       unsigned rtpTimestampFrequency,
	       char const* rtpPayloadFormatName,
	       unsigned numChannels = 1);
  // (we're an abstract base class)
  virtual ~AudioRTPSink();

private: // redefined virtual functions:
  virtual char const* sdpMediaType() const;
};




#endif /* INCLUDE_AUDIORTPSINK_H_ */
