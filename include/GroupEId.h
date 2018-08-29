/*
 * GroupEId.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_GROUPEID_H_
#define INCLUDE_GROUPEID_H_



#include "Boolean.h"

#include "NetAddress.h"

class GroupEId {
public:
  GroupEId(struct in_addr const& groupAddr,
	   portNumBits portNum, u_int8_t ttl);
      // used for a 'source-independent multicast' group
  GroupEId(struct in_addr const& groupAddr,
	   struct in_addr const& sourceFilterAddr,
	   portNumBits portNum);
      // used for a 'source-specific multicast' group

  struct in_addr const& groupAddress() const { return fGroupAddress; }
  struct in_addr const& sourceFilterAddress() const { return fSourceFilterAddress; }

  Boolean isSSM() const;

  portNumBits portNum() const { return fPortNum; }

  u_int8_t ttl() const { return fTTL; }

private:
  void init(struct in_addr const& groupAddr,
	    struct in_addr const& sourceFilterAddr,
	    portNumBits portNum,
	    u_int8_t ttl);

private:
  struct in_addr fGroupAddress;
  struct in_addr fSourceFilterAddress;
  portNumBits fPortNum; // in network byte order
  u_int8_t fTTL;
};



#endif /* INCLUDE_GROUPEID_H_ */
