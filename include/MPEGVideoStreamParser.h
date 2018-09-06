/*
 * MPEGVideoStreamParser.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MPEGVIDEOSTREAMPARSER_H_
#define INCLUDE_MPEGVIDEOSTREAMPARSER_H_


#include "StreamParser.h"
#include "MPEGVideoStreamFramer.h"

////////// MPEGVideoStreamParser definition //////////

class MPEGVideoStreamParser: public StreamParser {
public:
  MPEGVideoStreamParser(MPEGVideoStreamFramer* usingSource,
			FramedSource* inputSource);
  virtual ~MPEGVideoStreamParser();

public:
  void registerReadInterest(DP_U8* to, unsigned maxSize);

  virtual unsigned parse() = 0;
      // returns the size of the frame that was acquired, or 0 if none was
      // The number of truncated bytes (if any) is given by:
  unsigned numTruncatedBytes() const { return fNumTruncatedBytes; }

protected:
  void setParseState() {
    fSavedTo = fTo;
    fSavedNumTruncatedBytes = fNumTruncatedBytes;
    saveParserState();
  }

  // Record "byte" in the current output frame:
  void saveByte(u_int8_t byte) {
    if (fTo >= fLimit) { // there's no space left
      ++fNumTruncatedBytes;
      return;
    }

    *fTo++ = byte;
  }

  void save4Bytes(u_int32_t word) {
    if (fTo+4 > fLimit) { // there's no space left
      fNumTruncatedBytes += 4;
      return;
    }

    *fTo++ = word>>24; *fTo++ = word>>16; *fTo++ = word>>8; *fTo++ = word;
  }

  // Save data until we see a sync word (0x000001xx):
  void saveToNextCode(u_int32_t& curWord) {
    saveByte(curWord>>24);
    curWord = (curWord<<8)|get1Byte();
    while ((curWord&0xFFFFFF00) != 0x00000100) {
      if ((unsigned)(curWord&0xFF) > 1) {
	// a sync word definitely doesn't begin anywhere in "curWord"
	save4Bytes(curWord);
	curWord = get4Bytes();
      } else {
	// a sync word might begin in "curWord", although not at its start
	saveByte(curWord>>24);
	DP_U8 newByte = get1Byte();
	curWord = (curWord<<8)|newByte;
      }
    }
  }

  // Skip data until we see a sync word (0x000001xx):
  void skipToNextCode(u_int32_t& curWord) {
    curWord = (curWord<<8)|get1Byte();
    while ((curWord&0xFFFFFF00) != 0x00000100) {
      if ((unsigned)(curWord&0xFF) > 1) {
	// a sync word definitely doesn't begin anywhere in "curWord"
	curWord = get4Bytes();
      } else {
	// a sync word might begin in "curWord", although not at its start
	DP_U8 newByte = get1Byte();
	curWord = (curWord<<8)|newByte;
      }
    }
  }

protected:
  MPEGVideoStreamFramer* fUsingSource;

  // state of the frame that's currently being read:
  DP_U8* fStartOfFrame;
  DP_U8* fTo;
  DP_U8* fLimit;
  unsigned fNumTruncatedBytes;
  unsigned curFrameSize() { return fTo - fStartOfFrame; }
  DP_U8* fSavedTo;
  unsigned fSavedNumTruncatedBytes;

private: // redefined virtual functions
  virtual void restoreSavedParserState();
};



#endif /* INCLUDE_MPEGVIDEOSTREAMPARSER_H_ */
