/*
 * BasicUDPSource.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */




#include "BasicUDPSource.h"
#include "GroupsockHelper.h"

BasicUDPSource* BasicUDPSource::createNew(UsageEnvironment& env,
					Groupsock* inputGS) {
  return new BasicUDPSource(env, inputGS);
}

BasicUDPSource::BasicUDPSource(UsageEnvironment& env, Groupsock* inputGS)
  : FramedSource(env), fInputGS(inputGS), fHaveStartedReading(False) {
  // Try to use a large receive buffer (in the OS):
  increaseReceiveBufferTo(env, inputGS->socketNum(), 50*1024);

  // Make the socket non-blocking, even though it will be read from only asynchronously, when packets arrive.
  // The reason for this is that, in some OSs, reads on a blocking socket can (allegedly) sometimes block,
  // even if the socket was previously reported (e.g., by "select()") as having data available.
  // (This can supposedly happen if the UDP checksum fails, for example.)
  makeSocketNonBlocking(fInputGS->socketNum());
}

BasicUDPSource::~BasicUDPSource(){
  envir().taskScheduler().turnOffBackgroundReadHandling(fInputGS->socketNum());
}

void BasicUDPSource::doGetNextFrame() {
  if (!fHaveStartedReading) {
    // Await incoming packets:
    envir().taskScheduler().turnOnBackgroundReadHandling(fInputGS->socketNum(),
	 (TaskScheduler::BackgroundHandlerProc*)&incomingPacketHandler, this);
    fHaveStartedReading = True;
  }
}

void BasicUDPSource::doStopGettingFrames() {
  envir().taskScheduler().turnOffBackgroundReadHandling(fInputGS->socketNum());
  fHaveStartedReading = False;
}


void BasicUDPSource::incomingPacketHandler(BasicUDPSource* source, int /*mask*/){
  source->incomingPacketHandler1();
}

void BasicUDPSource::incomingPacketHandler1() {
  if (!isCurrentlyAwaitingData()) return; // we're not ready for the data yet

  // Read the packet into our desired destination:
  struct sockaddr_in fromAddress;
  if (!fInputGS->handleRead(fTo, fMaxSize, fFrameSize, fromAddress)) return;

  // Tell our client that we have new data:
  afterGetting(this); // we're preceded by a net read; no infinite recursion
}

