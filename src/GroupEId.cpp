/*
 * GroupEId.cpp
 *
 *  Created on: Jun 12, 2018
 *      Author: jhb
 */


#include "GroupEId.h"


GroupEId::GroupEId(struct in_addr const& groupAddr,
		   portNumBits portNum, u_int8_t ttl) {
  struct in_addr sourceFilterAddr;
  sourceFilterAddr.s_addr = ~0; // indicates no source filter

  init(groupAddr, sourceFilterAddr, portNum, ttl);
}

GroupEId::GroupEId(struct in_addr const& groupAddr,
		   struct in_addr const& sourceFilterAddr,
		   portNumBits portNum) {
  init(groupAddr, sourceFilterAddr, portNum, 255);
}

Boolean GroupEId::isSSM() const {
  return fSourceFilterAddress.s_addr != netAddressBits(~0);
}

void GroupEId::init(struct in_addr const& groupAddr,
		    struct in_addr const& sourceFilterAddr,
		    portNumBits portNum,
		    u_int8_t ttl) {
  fGroupAddress = groupAddr;
  fSourceFilterAddress = sourceFilterAddr;
  fPortNum = portNum;
  fTTL = ttl;
}



