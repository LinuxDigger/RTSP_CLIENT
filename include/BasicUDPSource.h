/*
 * BasicUDPSource.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_BASICUDPSOURCE_H_
#define INCLUDE_BASICUDPSOURCE_H_



#include "FramedSource.h"
#include "GroupSock.h"

class BasicUDPSource: public FramedSource {
public:
  static BasicUDPSource* createNew(UsageEnvironment& env, Groupsock* inputGS);

  virtual ~BasicUDPSource();

  Groupsock* gs() const { return fInputGS; }

private:
  BasicUDPSource(UsageEnvironment& env, Groupsock* inputGS);
      // called only by createNew()

  static void incomingPacketHandler(BasicUDPSource* source, int mask);
  void incomingPacketHandler1();

private: // redefined virtual functions:
  virtual void doGetNextFrame();
  virtual void doStopGettingFrames();

private:
  Groupsock* fInputGS;
  Boolean fHaveStartedReading;
};



#endif /* INCLUDE_BASICUDPSOURCE_H_ */
