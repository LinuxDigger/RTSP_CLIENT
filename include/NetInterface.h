/*
 * NetInterface.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_NETINTERFACE_H_
#define INCLUDE_NETINTERFACE_H_

#include "NetAddress.h"

class NetInterface {
public:
  virtual ~NetInterface();

  static UsageEnvironment* DefaultUsageEnvironment;
      // if non-NULL, used for each new interfaces

protected:
  NetInterface(); // virtual base class
};

class DirectedNetInterface: public NetInterface {
public:
  virtual ~DirectedNetInterface();

  virtual Boolean write(DP_U8* data, unsigned numBytes) = 0;

  virtual Boolean SourceAddrOKForRelaying(UsageEnvironment& env,
					  unsigned addr) = 0;

protected:
  DirectedNetInterface(); // virtual base class
};

class DirectedNetInterfaceSet {
public:
  DirectedNetInterfaceSet();
  virtual ~DirectedNetInterfaceSet();

  DirectedNetInterface* Add(DirectedNetInterface const* interf);
      // Returns the old value if different, otherwise 0
  Boolean Remove(DirectedNetInterface const* interf);

  Boolean IsEmpty() { return fTable->IsEmpty(); }

  // Used to iterate through the interfaces in the set
  class Iterator {
  public:
    Iterator(DirectedNetInterfaceSet& interfaces);
    virtual ~Iterator();

    DirectedNetInterface* next(); // NULL iff none

  private:
    HashTable::Iterator* fIter;
  };

private:
  friend class Iterator;
  HashTable* fTable;
};

class Socket: public NetInterface {
public:
  virtual ~Socket();
  void reset(); // closes the socket, and sets "fSocketNum" to -1

  virtual Boolean handleRead(DP_U8* buffer, unsigned bufferMaxSize,
			     unsigned& bytesRead,
			     struct sockaddr_in& fromAddress) = 0;
      // Returns False on error; resultData == NULL if data ignored

  int socketNum() const { return fSocketNum; }

  Port port() const {
    return fPort;
  }

  UsageEnvironment& env() const { return fEnv; }

  static int DebugLevel;

protected:
  Socket(DP_U16 scheID,UsageEnvironment& env, Port port); // virtual base class

  Boolean changePort(Port newPort); // will also cause socketNum() to change

private:
  int fSocketNum;
  UsageEnvironment& fEnv;
  Port fPort;

  DP_U16 _u16ScheID;
};

UsageEnvironment& operator<<(UsageEnvironment& s, const Socket& sock);

// A data structure for looking up a Socket by port:

class SocketLookupTable {
public:
  virtual ~SocketLookupTable();

  Socket* Fetch(UsageEnvironment& env, Port port, Boolean& isNew);
  // Creates a new Socket if none already exists
  Boolean Remove(Socket const* sock);

protected:
  SocketLookupTable(); // abstract base class
  virtual Socket* CreateNew(UsageEnvironment& env, Port port) = 0;

private:
  HashTable* fTable;
};

// A data structure for counting traffic:

class NetInterfaceTrafficStats {
public:
  NetInterfaceTrafficStats();

  void countPacket(unsigned packetSize);

  float totNumPackets() const {return fTotNumPackets;}
  float totNumBytes() const {return fTotNumBytes;}

  Boolean haveSeenTraffic() const;

private:
  float fTotNumPackets;
  float fTotNumBytes;
};



#endif /* INCLUDE_NETINTERFACE_H_ */
