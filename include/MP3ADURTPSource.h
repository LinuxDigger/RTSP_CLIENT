/*
 * MP3AUDRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MP3ADURTPSOURCE_H_
#define INCLUDE_MP3ADURTPSOURCE_H_


#ifndef _MULTI_FRAMED_RTP_SOURCE_HH
#include "MultiFramedRTPSource.h"
#endif

class MP3ADURTPSource: public MultiFramedRTPSource {
public:
  static MP3ADURTPSource*
  createNew(UsageEnvironment& env,CommonPlay *cpObj, Groupsock* RTPgs,
	    unsigned char rtpPayloadFormat,
	    unsigned rtpTimestampFrequency = 90000);

protected:
  virtual ~MP3ADURTPSource();

private:
  MP3ADURTPSource(UsageEnvironment& env,CommonPlay *cpObj, Groupsock* RTPgs,
		  unsigned char rtpPayloadFormat,
		  unsigned rtpTimestampFrequency);
      // called only by createNew()

private:
  // redefined virtual functions:
  virtual char const* MIMEtype() const;
};



#endif /* INCLUDE_MP3ADURTPSOURCE_H_ */
