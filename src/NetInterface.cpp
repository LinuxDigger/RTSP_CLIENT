/*
 * NetInterface.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#include "NetInterface.h"
#include "GroupsockHelper.h"

#include <sstream>
#include <stdio.h>

////////// NetInterface //////////

UsageEnvironment* NetInterface::DefaultUsageEnvironment = NULL;

NetInterface::NetInterface() {
}

NetInterface::~NetInterface() {
}

////////// NetInterface //////////

DirectedNetInterface::DirectedNetInterface() {
}

DirectedNetInterface::~DirectedNetInterface() {
}

////////// DirectedNetInterfaceSet //////////

DirectedNetInterfaceSet::DirectedNetInterfaceSet() :
		fTable(HashTable::create(ONE_WORD_HASH_KEYS)) {
}

DirectedNetInterfaceSet::~DirectedNetInterfaceSet() {
	delete fTable;
}

DirectedNetInterface*
DirectedNetInterfaceSet::Add(DirectedNetInterface const* interf) {
	return (DirectedNetInterface*) fTable->Add((char*) interf, (void*) interf);
}

Boolean DirectedNetInterfaceSet::Remove(DirectedNetInterface const* interf) {
	return fTable->Remove((char*) interf);
}

DirectedNetInterfaceSet::Iterator::Iterator(DirectedNetInterfaceSet& interfaces) :
		fIter(HashTable::Iterator::create(*(interfaces.fTable))) {
}

DirectedNetInterfaceSet::Iterator::~Iterator() {
	delete fIter;
}

DirectedNetInterface* DirectedNetInterfaceSet::Iterator::next() {
	char const* key; // dummy
	return (DirectedNetInterface*) fIter->next(key);
}
;

////////// Socket //////////

int Socket::DebugLevel = 1; // default value

Socket::Socket(DP_U16 scheID, UsageEnvironment& env, Port port) :
		fEnv(DefaultUsageEnvironment != NULL ? *DefaultUsageEnvironment : env), fPort(
				port), _u16ScheID(scheID) {
	fSocketNum = setupDatagramSocket(fEnv, port);
}

void Socket::reset() {
	if (fSocketNum >= 0)
		closeSocket(fSocketNum);
	fSocketNum = -1;
}

Socket::~Socket() {
	reset();
}

Boolean Socket::changePort(Port newPort) {
	int oldSocketNum = fSocketNum;
	unsigned oldReceiveBufferSize = getReceiveBufferSize(fEnv, fSocketNum);
	unsigned oldSendBufferSize = getSendBufferSize(fEnv, fSocketNum);
	closeSocket(fSocketNum);

	fSocketNum = setupDatagramSocket(fEnv, newPort);
	if (fSocketNum < 0) {
		fEnv.taskScheduler(_u16ScheID)->turnOffBackgroundReadHandling(
				oldSocketNum);
		return False;
	}

	setReceiveBufferTo(fEnv, fSocketNum, oldReceiveBufferSize);
	setSendBufferTo(fEnv, fSocketNum, oldSendBufferSize);
	if (fSocketNum != oldSocketNum) { // the socket number has changed, so move any event handling for it:
		fEnv.taskScheduler(_u16ScheID)->moveSocketHandling(
				oldSocketNum, fSocketNum);
	}
	return True;
}

UsageEnvironment& operator<<(UsageEnvironment& s, const Socket& sock) {
	return s << timestampString() << " Socket(" << sock.socketNum() << ")";
}

////////// SocketLookupTable //////////

SocketLookupTable::SocketLookupTable() :
		fTable(HashTable::create(ONE_WORD_HASH_KEYS)) {
}

SocketLookupTable::~SocketLookupTable() {
	delete fTable;
}

Socket* SocketLookupTable::Fetch(UsageEnvironment& env, Port port,
		Boolean& isNew) {
	isNew = False;
	Socket* sock;
	do {
		sock = (Socket*) fTable->Lookup((char*) (long) (port.num()));
		if (sock == NULL) { // we need to create one:
			sock = CreateNew(env, port);
			if (sock == NULL || sock->socketNum() < 0)
				break;

			fTable->Add((char*) (long) (port.num()), (void*) sock);
			isNew = True;
		}

		return sock;
	} while (0);

	delete sock;
	return NULL;
}

Boolean SocketLookupTable::Remove(Socket const* sock) {
	return fTable->Remove((char*) (long) (sock->port().num()));
}

////////// NetInterfaceTrafficStats //////////

NetInterfaceTrafficStats::NetInterfaceTrafficStats() {
	fTotNumPackets = fTotNumBytes = 0.0;
}

void NetInterfaceTrafficStats::countPacket(unsigned packetSize) {
	fTotNumPackets += 1.0;
	fTotNumBytes += packetSize;
}

Boolean NetInterfaceTrafficStats::haveSeenTraffic() const {
	return fTotNumPackets != 0.0;
}

