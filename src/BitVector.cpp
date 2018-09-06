/*
 * BitVector.cpp
 *
 *  Created on: Jun 12, 2018
 *      Author: jhb
 */

#include "BitVector.h"

BitVector::BitVector(DP_U8* baseBytePtr, unsigned baseBitOffset,
		unsigned totNumBits) {
	setup(baseBytePtr, baseBitOffset, totNumBits);
}

void BitVector::setup(DP_U8* baseBytePtr, unsigned baseBitOffset,
		unsigned totNumBits) {
	fBaseBytePtr = baseBytePtr;
	fBaseBitOffset = baseBitOffset;
	fTotNumBits = totNumBits;
	fCurBitIndex = 0;
}

static DP_U8 const singleBitMask[8] = { 0x80, 0x40, 0x20, 0x10, 0x08,
		0x04, 0x02, 0x01 };

#define MAX_LENGTH 32

void BitVector::putBits(unsigned from, unsigned numBits) {
	if (numBits == 0)
		return;

	DP_U8 tmpBuf[4];
	unsigned overflowingBits = 0;

	if (numBits > MAX_LENGTH) {
		numBits = MAX_LENGTH;
	}

	if (numBits > fTotNumBits - fCurBitIndex) {
		overflowingBits = numBits - (fTotNumBits - fCurBitIndex);
	}

	tmpBuf[0] = (DP_U8) (from >> 24);
	tmpBuf[1] = (DP_U8) (from >> 16);
	tmpBuf[2] = (DP_U8) (from >> 8);
	tmpBuf[3] = (DP_U8) from;

	shiftBits(fBaseBytePtr, fBaseBitOffset + fCurBitIndex, /* to */
	tmpBuf, MAX_LENGTH - numBits, /* from */
	numBits - overflowingBits /* num bits */);
	fCurBitIndex += numBits - overflowingBits;
}

void BitVector::put1Bit(unsigned bit) {
	// The following is equivalent to "putBits(..., 1)", except faster:
	if (fCurBitIndex >= fTotNumBits) { /* overflow */
		return;
	} else {
		unsigned totBitOffset = fBaseBitOffset + fCurBitIndex++;
		DP_U8 mask = singleBitMask[totBitOffset % 8];
		if (bit) {
			fBaseBytePtr[totBitOffset / 8] |= mask;
		} else {
			fBaseBytePtr[totBitOffset / 8] &= ~mask;
		}
	}
}

unsigned BitVector::getBits(unsigned numBits) {
	if (numBits == 0)
		return 0;

	DP_U8 tmpBuf[4];
	unsigned overflowingBits = 0;

	if (numBits > MAX_LENGTH) {
		numBits = MAX_LENGTH;
	}

	if (numBits > fTotNumBits - fCurBitIndex) {
		overflowingBits = numBits - (fTotNumBits - fCurBitIndex);
	}

	shiftBits(tmpBuf, 0, /* to */
	fBaseBytePtr, fBaseBitOffset + fCurBitIndex, /* from */
	numBits - overflowingBits /* num bits */);
	fCurBitIndex += numBits - overflowingBits;

	unsigned result = (tmpBuf[0] << 24) | (tmpBuf[1] << 16) | (tmpBuf[2] << 8)
			| tmpBuf[3];
	result >>= (MAX_LENGTH - numBits); // move into low-order part of word
	result &= (0xFFFFFFFF << overflowingBits); // so any overflow bits are 0
	return result;
}

unsigned BitVector::get1Bit() {
	// The following is equivalent to "getBits(1)", except faster:

	if (fCurBitIndex >= fTotNumBits) { /* overflow */
		return 0;
	} else {
		unsigned totBitOffset = fBaseBitOffset + fCurBitIndex++;
		DP_U8 curFromByte = fBaseBytePtr[totBitOffset / 8];
		unsigned result = (curFromByte >> (7 - (totBitOffset % 8))) & 0x01;
		return result;
	}
}

void BitVector::skipBits(unsigned numBits) {
	if (numBits > fTotNumBits - fCurBitIndex) { /* overflow */
		fCurBitIndex = fTotNumBits;
	} else {
		fCurBitIndex += numBits;
	}
}

unsigned BitVector::get_expGolomb() {
	unsigned numLeadingZeroBits = 0;
	unsigned codeStart = 1;

	while (get1Bit() == 0 && fCurBitIndex < fTotNumBits) {
		++numLeadingZeroBits;
		codeStart *= 2;
	}

	return codeStart - 1 + getBits(numLeadingZeroBits);
}

int BitVector::get_expGolombSigned() {
	unsigned codeNum = get_expGolomb();

	if ((codeNum & 1) == 0) { // even
		return -(codeNum / 2);
	} else { // odd
		return (codeNum + 1) / 2;
	}
}

void shiftBits(DP_U8* toBasePtr, unsigned toBitOffset,
		DP_U8 const* fromBasePtr, unsigned fromBitOffset,
		unsigned numBits) {
	if (numBits == 0)
		return;

	/* Note that from and to may overlap, if from>to */
	DP_U8 const* fromBytePtr = fromBasePtr + fromBitOffset / 8;
	unsigned fromBitRem = fromBitOffset % 8;
	DP_U8* toBytePtr = toBasePtr + toBitOffset / 8;
	unsigned toBitRem = toBitOffset % 8;

	while (numBits-- > 0) {
		DP_U8 fromBitMask = singleBitMask[fromBitRem];
		DP_U8 fromBit = (*fromBytePtr) & fromBitMask;
		DP_U8 toBitMask = singleBitMask[toBitRem];

		if (fromBit != 0) {
			*toBytePtr |= toBitMask;
		} else {
			*toBytePtr &= ~toBitMask;
		}

		if (++fromBitRem == 8) {
			++fromBytePtr;
			fromBitRem = 0;
		}
		if (++toBitRem == 8) {
			++toBytePtr;
			toBitRem = 0;
		}
	}
}

