/*
 * H265VideoStreamFramer.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H265VIDEOSTREAMFRAMER_H_
#define INCLUDE_H265VIDEOSTREAMFRAMER_H_


#include "H264or5VideoStreamFramer.h"

class H265VideoStreamFramer: public H264or5VideoStreamFramer {
public:
  static H265VideoStreamFramer* createNew(UsageEnvironment& env, FramedSource* inputSource,
					  Boolean includeStartCodeInOutput = False);

protected:
  H265VideoStreamFramer(UsageEnvironment& env, FramedSource* inputSource, Boolean createParser, Boolean includeStartCodeInOutput);
      // called only by "createNew()"
  virtual ~H265VideoStreamFramer();

  // redefined virtual functions:
  virtual Boolean isH265VideoStreamFramer() const;
};


#endif /* INCLUDE_H265VIDEOSTREAMFRAMER_H_ */
