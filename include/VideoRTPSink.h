/*
 * VideoRTPSink.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_VIDEORTPSINK_H_
#define INCLUDE_VIDEORTPSINK_H_


#include "MultiFramedRTPSink.h"

class VideoRTPSink: public MultiFramedRTPSink {
protected:
  VideoRTPSink(UsageEnvironment& env,
	       Groupsock* rtpgs, unsigned char rtpPayloadType,
	       unsigned rtpTimestampFrequency,
	       char const* rtpPayloadFormatName);
  // (we're an abstract base class)
  virtual ~VideoRTPSink();

private: // redefined virtual functions:
  virtual char const* sdpMediaType() const;
};




#endif /* INCLUDE_VIDEORTPSINK_H_ */
