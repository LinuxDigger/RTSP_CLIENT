/*
 * OggFileSink.h
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_OGGFILESINK_H_
#define INCLUDE_OGGFILESINK_H_


#include "FileSink.h"

class OggFileSink: public FileSink {
public:
  static OggFileSink* createNew(UsageEnvironment& env, char const* fileName,
				unsigned samplingFrequency = 0, // used for granule_position
				char const* configStr = NULL,
      // "configStr" is an optional 'SDP format' string (Base64-encoded)
      // representing 'packed configuration headers' ("identification", "comment", "setup")
      // to prepend to the output.  (For 'Vorbis" audio and 'Theora' video.)
				unsigned bufferSize = 100000,
				Boolean oneFilePerFrame = False, CommonPlay *cpObj=NULL);
      // See "FileSink.hh" for a description of these parameters.

protected:
  OggFileSink(UsageEnvironment& env, FILE* fid, unsigned samplingFrequency, char const* configStr,
	      unsigned bufferSize, char const* perFrameFileNamePrefix, CommonPlay *cpObj);
      // called only by createNew()
  virtual ~OggFileSink();

protected: // redefined virtual functions:
  virtual Boolean continuePlaying();
  virtual void addData(DP_U8 const* data, unsigned dataSize,
		       struct timeval presentationTime);
  virtual void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
				 struct timeval presentationTime);

private:
  static void ourOnSourceClosure(void* clientData);
  void ourOnSourceClosure();

private:
  unsigned fSamplingFrequency;
  char const* fConfigStr;
  Boolean fHaveWrittenFirstFrame, fHaveSeenEOF;
  struct timeval fFirstPresentationTime;
  int64_t fGranulePosition;
  int64_t fGranulePositionAdjustment; // used to ensure that "fGranulePosition" stays monotonic
  u_int32_t fPageSequenceNumber;
  u_int8_t fPageHeaderBytes[27];
      // the header of each Ogg page, through the "number_page_segments" byte

  // Special fields used for Theora video:
  Boolean fIsTheora;
  u_int64_t fGranuleIncrementPerFrame; // == 1 << KFGSHIFT

  // Because the last Ogg page before EOF needs to have a special 'eos' bit set in the header,
  // we need to defer the writing of each incoming frame.  To do this, we maintain a 2nd buffer:
  DP_U8* fAltBuffer;
  unsigned fAltFrameSize, fAltNumTruncatedBytes;
  struct timeval fAltPresentationTime;
};



#endif /* INCLUDE_OGGFILESINK_H_ */
