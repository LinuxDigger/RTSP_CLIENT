/*
 * H263plusVideoRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H263PLUSVIDEORTPSOURCE_H_
#define INCLUDE_H263PLUSVIDEORTPSOURCE_H_

#include "MultiFramedRTPSource.h"

#define SPECIAL_HEADER_BUFFER_SIZE 1000

class H263plusVideoRTPSource: public MultiFramedRTPSource {
public:
  static H263plusVideoRTPSource*
  createNew(UsageEnvironment& env,CommonPlay *cpObj, Groupsock* RTPgs,
	    unsigned char rtpPayloadFormat,
	    unsigned rtpTimestampFrequency = 90000);

  // A data structure that stores copies of the special header bytes
  // from the most recent frame's RTP packets:
  unsigned char fNumSpecialHeaders;
  unsigned fSpecialHeaderBytesLength;
  unsigned char fSpecialHeaderBytes[SPECIAL_HEADER_BUFFER_SIZE];
  unsigned fPacketSizes[256];

protected:
  virtual ~H263plusVideoRTPSource();

private:
  H263plusVideoRTPSource(UsageEnvironment& env,CommonPlay *cpObj, Groupsock* RTPgs,
			 unsigned char rtpPayloadFormat,
			 unsigned rtpTimestampFrequency);
      // called only by createNew()

private:
  // redefined virtual functions:
  virtual Boolean processSpecialHeader(BufferedPacket* packet,
                                       unsigned& resultSpecialHeaderSize);
  virtual char const* MIMEtype() const;
};





#endif /* INCLUDE_H263PLUSVIDEORTPSOURCE_H_ */
