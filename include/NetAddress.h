/*
 * NetAddress.h
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_NETADDRESS_H_
#define INCLUDE_NETADDRESS_H_
#include "NetCommon.h"
#include "HashTable.h"
#include "UsageEnvironment.h"


// Definition of a type representing a low-level network address.
// At present, this is 32-bits, for IPv4.  Later, generalize it,
// to allow for IPv6.
typedef u_int32_t netAddressBits;

class NetAddress {
public:
  NetAddress(u_int8_t const* data,
	     unsigned length = 4 /* default: 32 bits */);
  NetAddress(unsigned length = 4); // sets address data to all-zeros
  NetAddress(NetAddress const& orig);
  NetAddress& operator=(NetAddress const& rightSide);
  virtual ~NetAddress();

  unsigned length() const { return fLength; }
  u_int8_t const* data() const // always in network byte order
  { return fData; }

private:
  void assign(u_int8_t const* data, unsigned length);
  void clean();

  unsigned fLength;
  u_int8_t* fData;
};

class NetAddressList {
public:
  NetAddressList(char const* hostname);
  NetAddressList(NetAddressList const& orig);
  NetAddressList& operator=(NetAddressList const& rightSide);
  virtual ~NetAddressList();

  unsigned numAddresses() const { return fNumAddresses; }

  NetAddress const* firstAddress() const;

  // Used to iterate through the addresses in a list:
  class Iterator {
  public:
    Iterator(NetAddressList const& addressList);
    NetAddress const* nextAddress(); // NULL iff none
  private:
    NetAddressList const& fAddressList;
    unsigned fNextIndex;
  };

private:
  void assign(netAddressBits numAddresses, NetAddress** addressArray);
  void clean();

  friend class Iterator;
  unsigned fNumAddresses;
  NetAddress** fAddressArray;
};

typedef u_int16_t portNumBits;

class Port {
public:
  Port(portNumBits num /* in host byte order */);

  portNumBits num() const { return fPortNum; } // in network byte order

private:
  portNumBits fPortNum; // stored in network byte order
#ifdef IRIX
  portNumBits filler; // hack to overcome a bug in IRIX C++ compiler
#endif
};

UsageEnvironment& operator<<(UsageEnvironment& s, const Port& p);


// A generic table for looking up objects by (address1, address2, port)
class AddressPortLookupTable {
public:
  AddressPortLookupTable();
  virtual ~AddressPortLookupTable();

  void* Add(netAddressBits address1, netAddressBits address2, Port port, void* value);
      // Returns the old value if different, otherwise 0
  Boolean Remove(netAddressBits address1, netAddressBits address2, Port port);
  void* Lookup(netAddressBits address1, netAddressBits address2, Port port);
      // Returns 0 if not found
  void* RemoveNext() { return fTable->RemoveNext(); }

  // Used to iterate through the entries in the table
  class Iterator {
  public:
    Iterator(AddressPortLookupTable& table);
    virtual ~Iterator();

    void* next(); // NULL iff none

  private:
    HashTable::Iterator* fIter;
  };

private:
  friend class Iterator;
  HashTable* fTable;
};


Boolean IsMulticastAddress(netAddressBits address);


// A mechanism for displaying an IPv4 address in ASCII.  This is intended to replace "inet_ntoa()", which is not thread-safe.
class AddressString {
public:
  AddressString(struct sockaddr_in const& addr);
  AddressString(struct in_addr const& addr);
  AddressString(netAddressBits addr); // "addr" is assumed to be in host byte order here

  virtual ~AddressString();

  char const* val() const { return fVal; }

private:
  void init(netAddressBits addr); // used to implement each of the constructors

private:
  char* fVal; // The result ASCII string: allocated by the constructor; deleted by the destructor
};




#endif /* INCLUDE_NETADDRESS_H_ */
