/*
 * DVVideoRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_DVVIDEORTPSOURCE_H_
#define INCLUDE_DVVIDEORTPSOURCE_H_


#include "MultiFramedRTPSource.h"

class DVVideoRTPSource: public MultiFramedRTPSource {
public:
  static DVVideoRTPSource*
  createNew(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
	    unsigned char rtpPayloadFormat,
	    unsigned rtpTimestampFrequency);

protected:
  virtual ~DVVideoRTPSource();

private:
  DVVideoRTPSource(UsageEnvironment& env, CommonPlay *cpObj, Groupsock* RTPgs,
		   unsigned char rtpPayloadFormat,
		   unsigned rtpTimestampFrequency);
      // called only by createNew()

private:
  // redefined virtual functions:
  virtual Boolean processSpecialHeader(BufferedPacket* packet,
                                       unsigned& resultSpecialHeaderSize);
  virtual char const* MIMEtype() const;
};




#endif /* INCLUDE_DVVIDEORTPSOURCE_H_ */
