/*
 * JPEGVideoRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_JPEGVIDEORTPSOURCE_H_
#define INCLUDE_JPEGVIDEORTPSOURCE_H_


#include "MultiFramedRTPSource.h"

#define MAX_JPEG_HEADER_SIZE 1024

class JPEGVideoRTPSource: public MultiFramedRTPSource {
public:
  static JPEGVideoRTPSource*
  createNew(UsageEnvironment& env,CommonPlay *cpObj, Groupsock* RTPgs,
	    DP_U8 rtpPayloadFormat = 26,
	    unsigned rtpPayloadFrequency = 90000,
	    unsigned defaultWidth = 0, unsigned defaultHeight = 0);

protected:
  virtual ~JPEGVideoRTPSource();

private:
  JPEGVideoRTPSource(UsageEnvironment& env,CommonPlay *cpObj, Groupsock* RTPgs,
		     DP_U8 rtpPayloadFormat,
		     unsigned rtpTimestampFrequency,
		     unsigned defaultWidth, unsigned defaultHeight);
      // called only by createNew()

  // Image dimensions from the SDP description, if any
  unsigned fDefaultWidth, fDefaultHeight;

private:
  // redefined virtual functions:
  virtual Boolean processSpecialHeader(BufferedPacket* packet,
                                       unsigned& resultSpecialHeaderSize);

  virtual char const* MIMEtype() const;
};




#endif /* INCLUDE_JPEGVIDEORTPSOURCE_H_ */
