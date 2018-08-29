/*
 * H264or5VideoStreamFramer.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H264OR5VIDEOSTREAMFRAMER_H_
#define INCLUDE_H264OR5VIDEOSTREAMFRAMER_H_


#include "MPEGVideoStreamFramer.h"

class H264or5VideoStreamFramer: public MPEGVideoStreamFramer {
public:
  void getVPSandSPSandPPS(u_int8_t*& vps, unsigned& vpsSize,
			  u_int8_t*& sps, unsigned& spsSize,
			  u_int8_t*& pps, unsigned& ppsSize) const {
    // Returns pointers to copies of the most recently seen VPS (video parameter set)
    // SPS (sequence parameter set) and PPS (picture parameter set) NAL units.
    // (NULL pointers are returned if the NAL units have not yet been seen.)
    vps = fLastSeenVPS; vpsSize = fLastSeenVPSSize;
    sps = fLastSeenSPS; spsSize = fLastSeenSPSSize;
    pps = fLastSeenPPS; ppsSize = fLastSeenPPSSize;
  }

  void setVPSandSPSandPPS(u_int8_t* vps, unsigned vpsSize,
			  u_int8_t* sps, unsigned spsSize,
			  u_int8_t* pps, unsigned ppsSize) {
    // Assigns copies of the VPS, SPS and PPS NAL units.  If this function is not called,
    // then these NAL units are assigned only if/when they appear in the input stream.
    saveCopyOfVPS(vps, vpsSize);
    saveCopyOfSPS(sps, spsSize);
    saveCopyOfPPS(pps, ppsSize);
  }

protected:
  H264or5VideoStreamFramer(int hNumber, // 264 or 265
			   UsageEnvironment& env, FramedSource* inputSource,
			   Boolean createParser, Boolean includeStartCodeInOutput);
      // We're an abstract base class.
  virtual ~H264or5VideoStreamFramer();

  void saveCopyOfVPS(u_int8_t* from, unsigned size);
  void saveCopyOfSPS(u_int8_t* from, unsigned size);
  void saveCopyOfPPS(u_int8_t* from, unsigned size);

  void setPresentationTime() { fPresentationTime = fNextPresentationTime; }

  Boolean isVPS(u_int8_t nal_unit_type);
  Boolean isSPS(u_int8_t nal_unit_type);
  Boolean isPPS(u_int8_t nal_unit_type);
  Boolean isVCL(u_int8_t nal_unit_type);

protected:
  int fHNumber;
  u_int8_t* fLastSeenVPS;
  unsigned fLastSeenVPSSize;
  u_int8_t* fLastSeenSPS;
  unsigned fLastSeenSPSSize;
  u_int8_t* fLastSeenPPS;
  unsigned fLastSeenPPSSize;
  struct timeval fNextPresentationTime; // the presentation time to be used for the next NAL unit to be parsed/delivered after this
  friend class H264or5VideoStreamParser; // hack
};

// A general routine for making a copy of a (H.264 or H.265) NAL unit,
// removing 'emulation' bytes from the copy:
unsigned removeH264or5EmulationBytes(u_int8_t* to, unsigned toMaxSize,
				     u_int8_t const* from, unsigned fromSize);
    // returns the size of the copy; it will be <= min(toMaxSize,fromSize)



#endif /* INCLUDE_H264OR5VIDEOSTREAMFRAMER_H_ */
