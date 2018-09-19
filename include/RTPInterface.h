/*
 * RTPInterface.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_RTPINTERFACE_H_
#define INCLUDE_RTPINTERFACE_H_

#include "Media.h"
#include "GroupSock.h"

// Typedef for an optional auxilliary handler function, to be called
// when each new packet is read:
typedef void AuxHandlerFunc(void* clientData, DP_U8* packet,
		unsigned& packetSize);

typedef void ServerRequestAlternativeByteHandler(void* instance,
		u_int8_t requestByte);
// A hack that allows a handler for RTP/RTCP packets received over TCP to process RTSP commands that may also appear within
// the same TCP connection.  A RTSP server implementation would supply a function like this - as a parameter to
// "ServerMediaSubsession::startStream()".

class tcpStreamRecord {
public:
	tcpStreamRecord(int streamSocketNum, DP_U8 streamChannelId,
			tcpStreamRecord* next);
	virtual ~tcpStreamRecord();

public:
	tcpStreamRecord* fNext;
	int fStreamSocketNum;
	DP_U8 fStreamChannelId;
};

class RTPInterface {
public:
	RTPInterface(Medium* owner, Groupsock* gs);
	virtual ~RTPInterface();

	Groupsock* gs() const {
		return fGS;
	}

	void setStreamSocket(int sockNum, DP_U8 streamChannelId);
	void addStreamSocket(int sockNum, DP_U8 streamChannelId);
	void removeStreamSocket(int sockNum, DP_U8 streamChannelId);
	static void setServerRequestAlternativeByteHandler(DP_U16 scheID,
			UsageEnvironment& env, int socketNum,
			ServerRequestAlternativeByteHandler* handler, void* clientData);
	static void clearServerRequestAlternativeByteHandler(DP_U16 scheID,
			UsageEnvironment& env, int socketNum);

	Boolean sendPacket(DP_U8* packet, unsigned packetSize);
	void startNetworkReading(TaskScheduler::BackgroundHandlerProc* handlerProc);
	Boolean handleRead(DP_U8* buffer, unsigned bufferMaxSize,
			// out parameters:
			unsigned& bytesRead, struct sockaddr_in& fromAddress,
			int& tcpSocketNum, DP_U8& tcpStreamChannelId,
			Boolean& packetReadWasIncomplete);
	// Note: If "tcpSocketNum" < 0, then the packet was received over UDP, and "tcpStreamChannelId"
	//   is undefined (and irrelevant).

	// Otherwise (if "tcpSocketNum" >= 0), the packet was received (interleaved) over TCP, and
	//   "tcpStreamChannelId" will return the channel id.

	void stopNetworkReading();

	UsageEnvironment& envir() const {
		return fOwner->envir();
	}

	Medium *getOwnerMedium() const {
		return fOwner;
	}

	void setAuxilliaryReadHandler(AuxHandlerFunc* handlerFunc,
			void* handlerClientData) {
		fAuxReadHandlerFunc = handlerFunc;
		fAuxReadHandlerClientData = handlerClientData;
	}

	void forgetOurGroupsock() {
		fGS = NULL;
	}
	friend class SocketDescriptor;
	// This may be called - *only immediately prior* to deleting this - to prevent our destructor
	// from turning off background reading on the 'groupsock'.  (This is in case the 'groupsock'
	// is also being read from elsewhere.)

private:
	// Helper functions for sending a RTP or RTCP packet over a TCP connection:
	Boolean sendRTPorRTCPPacketOverTCP(DP_U8* packet, unsigned packetSize,
			int socketNum, DP_U8 streamChannelId);
	Boolean sendDataOverTCP(int socketNum, u_int8_t const* data,
			unsigned dataSize, Boolean forceSendToSucceed);

private:
	Medium* fOwner;
	Groupsock* fGS;
	tcpStreamRecord* fTCPStreams; // optional, for RTP-over-TCP streaming/receiving

	unsigned short fNextTCPReadSize;
	// how much data (if any) is available to be read from the TCP stream
	int fNextTCPReadStreamSocketNum;
	DP_U8 fNextTCPReadStreamChannelId;
	TaskScheduler::BackgroundHandlerProc* fReadHandlerProc; // if any

	AuxHandlerFunc* fAuxReadHandlerFunc;
	void* fAuxReadHandlerClientData;
};

#endif /* INCLUDE_RTPINTERFACE_H_ */
