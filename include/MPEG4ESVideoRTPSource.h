/*
 * MPEG4ESVideoRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG4ESVIDEORTPSOURCE_H_
#define INCLUDE_MPEG4ESVIDEORTPSOURCE_H_


#include "MultiFramedRTPSource.h"

class MPEG4ESVideoRTPSource: public MultiFramedRTPSource {
public:
  static MPEG4ESVideoRTPSource*
  createNew(UsageEnvironment& env, CommonPlay *cpObj,Groupsock* RTPgs,
	    unsigned char rtpPayloadFormat,
	    unsigned rtpTimestampFrequency);

protected:
  virtual ~MPEG4ESVideoRTPSource();

private:
  MPEG4ESVideoRTPSource(UsageEnvironment& env,CommonPlay *cpObj, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat,
			unsigned rtpTimestampFrequency);
      // called only by createNew()

private:
  // redefined virtual functions:
  virtual Boolean processSpecialHeader(BufferedPacket* packet,
                                       unsigned& resultSpecialHeaderSize);
  virtual char const* MIMEtype() const;
};




#endif /* INCLUDE_MPEG4ESVIDEORTPSOURCE_H_ */
