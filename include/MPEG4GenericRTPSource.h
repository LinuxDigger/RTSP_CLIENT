/*
 * MPEG4GenericRTPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEG4GENERICRTPSOURCE_H_
#define INCLUDE_MPEG4GENERICRTPSOURCE_H_


#include "MultiFramedRTPSource.h"

class MPEG4GenericRTPSource: public MultiFramedRTPSource {
public:
  static MPEG4GenericRTPSource*
  createNew(UsageEnvironment& env, Groupsock* RTPgs,
	    unsigned char rtpPayloadFormat,
	    unsigned rtpTimestampFrequency,
	    char const* mediumName,
	    char const* mode, unsigned sizeLength, unsigned indexLength,
	    unsigned indexDeltaLength
	    // add other parameters later
	    );
  // mediumName is "audio", "video", or "application"
  // it *cannot* be NULL

protected:
  virtual ~MPEG4GenericRTPSource();

private:
  MPEG4GenericRTPSource(UsageEnvironment& env, Groupsock* RTPgs,
			unsigned char rtpPayloadFormat,
			unsigned rtpTimestampFrequency,
			char const* mediumName,
			char const* mode,
			unsigned sizeLength, unsigned indexLength,
			unsigned indexDeltaLength
			);
      // called only by createNew()

private:
  // redefined virtual functions:
  virtual Boolean processSpecialHeader(BufferedPacket* packet,
                                       unsigned& resultSpecialHeaderSize);
  virtual char const* MIMEtype() const;

private:
  char* fMIMEType;

  char* fMode;
  unsigned fSizeLength, fIndexLength, fIndexDeltaLength;
  unsigned fNumAUHeaders; // in the most recently read packet
  unsigned fNextAUHeader; // index of the next AU Header to read
  struct AUHeader* fAUHeaders;

  friend class MPEG4GenericBufferedPacket;
};



// A function that looks up the sampling frequency from an
// "AudioSpecificConfig" string.  (0 means 'unknown')
unsigned samplingFrequencyFromAudioSpecificConfig(char const* configStr);



#endif /* INCLUDE_MPEG4GENERICRTPSOURCE_H_ */
