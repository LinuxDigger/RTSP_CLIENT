/*
 * BitVector.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_BITVECTOR_H_
#define INCLUDE_BITVECTOR_H_

#include "Boolean.h"

class BitVector {
public:
	BitVector(DP_U8* baseBytePtr, unsigned baseBitOffset,
			unsigned totNumBits);

	void setup(DP_U8* baseBytePtr, unsigned baseBitOffset,
			unsigned totNumBits);

	void putBits(unsigned from, unsigned numBits); // "numBits" <= 32
	void put1Bit(unsigned bit);

	unsigned getBits(unsigned numBits); // "numBits" <= 32
	unsigned get1Bit();
	Boolean get1BitBoolean() {
		return get1Bit() != 0;
	}

	void skipBits(unsigned numBits);

	unsigned curBitIndex() const {
		return fCurBitIndex;
	}
	unsigned totNumBits() const {
		return fTotNumBits;
	}
	unsigned numBitsRemaining() const {
		return fTotNumBits - fCurBitIndex;
	}

	unsigned get_expGolomb();
	// Returns the value of the next bits, assuming that they were encoded using an exponential-Golomb code of order 0
	int get_expGolombSigned(); // signed version of the above

private:
	DP_U8* fBaseBytePtr;
	unsigned fBaseBitOffset;
	unsigned fTotNumBits;
	unsigned fCurBitIndex;
};

// A general bit copy operation:
void shiftBits(DP_U8* toBasePtr, unsigned toBitOffset,
		DP_U8 const* fromBasePtr, unsigned fromBitOffset,
		unsigned numBits);

#endif /* INCLUDE_BITVECTOR_H_ */
