/*
 * FramedFilter.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_FRAMEDFILTER_H_
#define INCLUDE_FRAMEDFILTER_H_

#include "FramedSource.h"

class FramedFilter: public FramedSource {
public:
  FramedSource* inputSource() const { return fInputSource; }

  void reassignInputSource(FramedSource* newInputSource) { fInputSource = newInputSource; }

  // Call before destruction if you want to prevent the destructor from closing the input source
  void detachInputSource();

protected:
  FramedFilter(UsageEnvironment& env, FramedSource* inputSource);
	 // abstract base class
  virtual ~FramedFilter();

protected:
  // Redefined virtual functions (with default 'null' implementations):
  virtual char const* MIMEtype() const;
  virtual void getAttributes() const;
  virtual void doStopGettingFrames();

protected:
  FramedSource* fInputSource;
};





#endif /* INCLUDE_FRAMEDFILTER_H_ */
