/*
 * DVVideoRTPSink.cpp
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */


#include "DVVideoRTPSink.h"

////////// DVVideoRTPSink implementation //////////

DVVideoRTPSink
::DVVideoRTPSink(UsageEnvironment& env, Groupsock* RTPgs, unsigned char rtpPayloadFormat)
  : VideoRTPSink(env, RTPgs, rtpPayloadFormat, 90000, "DV"),
    fFmtpSDPLine(NULL) {
}

DVVideoRTPSink::~DVVideoRTPSink() {
  delete[] fFmtpSDPLine;
}

DVVideoRTPSink*
DVVideoRTPSink::createNew(UsageEnvironment& env, Groupsock* RTPgs, unsigned char rtpPayloadFormat) {
  return new DVVideoRTPSink(env, RTPgs, rtpPayloadFormat);
}

Boolean DVVideoRTPSink::sourceIsCompatibleWithUs(MediaSource& source) {
  // Our source must be an appropriate framer:
  return source.isDVVideoStreamFramer();
}

void DVVideoRTPSink::doSpecialFrameHandling(unsigned fragmentationOffset,
					      unsigned char* /*frameStart*/,
					      unsigned /*numBytesInFrame*/,
					      struct timeval framePresentationTime,
					      unsigned numRemainingBytes) {
  if (numRemainingBytes == 0) {
    // This packet contains the last (or only) fragment of the frame.
    // Set the RTP 'M' ('marker') bit:
    setMarkerBit();
  }

  // Also set the RTP timestamp:
  setTimestamp(framePresentationTime);
}

unsigned DVVideoRTPSink::computeOverflowForNewFrame(unsigned newFrameSize) const {
  unsigned initialOverflow = MultiFramedRTPSink::computeOverflowForNewFrame(newFrameSize);

  // Adjust (increase) this overflow, if necessary, so that the amount of frame data that we use is an integral number
  // of DIF blocks:
  unsigned numFrameBytesUsed = newFrameSize - initialOverflow;
  initialOverflow += numFrameBytesUsed%DV_DIF_BLOCK_SIZE;

  return initialOverflow;
}

char const* DVVideoRTPSink::auxSDPLine() {
  // Generate a new "a=fmtp:" line each time, using parameters from
  // our framer source (in case they've changed since the last time that
  // we were called):
  DVVideoStreamFramer* framerSource = (DVVideoStreamFramer*)fSource;
  if (framerSource == NULL) return NULL; // we don't yet have a source

  return auxSDPLineFromFramer(framerSource);
}

char const* DVVideoRTPSink::auxSDPLineFromFramer(DVVideoStreamFramer* framerSource) {
  char const* const profileName = framerSource->profileName();
  if (profileName == NULL) return NULL;

  char const* const fmtpSDPFmt = "a=fmtp:%d encode=%s;audio=bundled\r\n";
  unsigned fmtpSDPFmtSize = strlen(fmtpSDPFmt)
    + 3 // max payload format code length
    + strlen(profileName);
  delete[] fFmtpSDPLine; // if it already exists
  fFmtpSDPLine = new char[fmtpSDPFmtSize];
  sprintf(fFmtpSDPLine, fmtpSDPFmt, rtpPayloadType(), profileName);

  return fFmtpSDPLine;
}


