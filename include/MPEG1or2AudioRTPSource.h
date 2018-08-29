/*
 * MEPG1or2AudioRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG1OR2AUDIORTPSOURCE_H_
#define INCLUDE_MPEG1OR2AUDIORTPSOURCE_H_


#ifndef _MULTI_FRAMED_RTP_SOURCE_HH
#include "MultiFramedRTPSource.h"
#endif

class MPEG1or2AudioRTPSource: public MultiFramedRTPSource {
public:
  static MPEG1or2AudioRTPSource*
  createNew(UsageEnvironment& env, Groupsock* RTPgs,
	    unsigned char rtpPayloadFormat = 14,
	    unsigned rtpTimestampFrequency = 90000);

protected:
  virtual ~MPEG1or2AudioRTPSource();

private:
  MPEG1or2AudioRTPSource(UsageEnvironment& env, Groupsock* RTPgs,
		     unsigned char rtpPayloadFormat,
		     unsigned rtpTimestampFrequency);
      // called only by createNew()

private:
  // redefined virtual functions:
  virtual Boolean processSpecialHeader(BufferedPacket* packet,
                                       unsigned& resultSpecialHeaderSize);
  virtual char const* MIMEtype() const;
};




#endif /* INCLUDE_MPEG1OR2AUDIORTPSOURCE_H_ */
