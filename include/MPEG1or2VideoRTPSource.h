/*
 * MPEG1or2VideoRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG1OR2VIDEORTPSOURCE_H_
#define INCLUDE_MPEG1OR2VIDEORTPSOURCE_H_


#include "MultiFramedRTPSource.h"

class MPEG1or2VideoRTPSource: public MultiFramedRTPSource {
public:
  static MPEG1or2VideoRTPSource*
  createNew(UsageEnvironment& env,CommonPlay *cpObj, Groupsock* RTPgs,
	    unsigned char rtpPayloadFormat = 32,
	    unsigned rtpPayloadFrequency = 90000);

protected:
  virtual ~MPEG1or2VideoRTPSource();

private:
  MPEG1or2VideoRTPSource(UsageEnvironment& env, CommonPlay *cpObj,Groupsock* RTPgs,
		     unsigned char rtpPayloadFormat,
		     unsigned rtpTimestampFrequency);
      // called only by createNew()

private:
  // redefined virtual functions:
  virtual Boolean processSpecialHeader(BufferedPacket* packet,
                                       unsigned& resultSpecialHeaderSize);
  virtual Boolean packetIsUsableInJitterCalculation(unsigned char* packet,
						    unsigned packetSize);
  virtual char const* MIMEtype() const;
};



#endif /* INCLUDE_MPEG1OR2VIDEORTPSOURCE_H_ */
