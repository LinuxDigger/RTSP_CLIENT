/*
 * FloorAndCeiling.h
 *
 *  Created on: Aug 30, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_FLOORANDCEILING_H_
#define INCLUDE_FLOORANDCEILING_H_

//number of Leading of zeros instruction.
int nlz(int x) {
	int ret = 0;
	unsigned int temp = ~x;
	while (temp & 0x80000000) {
		temp <<= 1;
		ret++;
	}
	return ret;
}

//floor     5->4
int flp(int x) {
	return (1 << (31 - nlz(x)));
}

//ceiling    5->8
int clp(int x) {
	return (1 << (32 - nlz(x - 1)));
}

#endif /* INCLUDE_FLOORANDCEILING_H_ */
