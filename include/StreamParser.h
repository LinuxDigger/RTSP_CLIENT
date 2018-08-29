/*
 * StreamParser.h
 *
 *  Created on: Aug 13, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_STREAMPARSER_H_
#define INCLUDE_STREAMPARSER_H_

#include "FramedSource.h"

class StreamParser {
public:
	virtual void flushInput();

protected:
	// we're a virtual base class
	typedef void (clientContinueFunc)(void* clientData, unsigned char* ptr,
			unsigned size, struct timeval presentationTime);
	StreamParser(FramedSource* inputSource,
			FramedSource::onCloseFunc* onInputCloseFunc,
			void* onInputCloseClientData,
			clientContinueFunc* clientContinueFunc,
			void* clientContinueClientData);
	virtual ~StreamParser();

	void saveParserState();
	virtual void restoreSavedParserState();

	u_int32_t get4Bytes() { // byte-aligned; returned in big-endian order
		u_int32_t result = test4Bytes();
		fCurParserIndex += 4;
		fRemainingUnparsedBits = 0;

		return result;
	}
	u_int32_t test4Bytes() { // as above, but doesn't advance ptr
		ensureValidBytes(4);

		unsigned char const* ptr = nextToParse();
		return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
	}

	u_int16_t get2Bytes() {
		ensureValidBytes(2);

		unsigned char const* ptr = nextToParse();
		u_int16_t result = (ptr[0] << 8) | ptr[1];

		fCurParserIndex += 2;
		fRemainingUnparsedBits = 0;

		return result;
	}

	u_int8_t get1Byte() { // byte-aligned
		ensureValidBytes(1);
		fRemainingUnparsedBits = 0;
		return curBank()[fCurParserIndex++];
	}
	u_int8_t test1Byte() { // as above, but doesn't advance ptr
		ensureValidBytes(1);
		return nextToParse()[0];
	}

	void getBytes(u_int8_t* to, unsigned numBytes) {
		testBytes(to, numBytes);
		fCurParserIndex += numBytes;
		fRemainingUnparsedBits = 0;
	}
	void testBytes(u_int8_t* to, unsigned numBytes) { // as above, but doesn't advance ptr
		ensureValidBytes(numBytes);
		memmove(to, nextToParse(), numBytes);
	}
	void skipBytes(unsigned numBytes) {
		ensureValidBytes(numBytes);
		fCurParserIndex += numBytes;
	}

	void skipBits(unsigned numBits);
	unsigned getBits(unsigned numBits);
	// numBits <= 32; returns data into low-order bits of result

	unsigned curOffset() const {
		return fCurParserIndex;
	}

	unsigned& totNumValidBytes() {
		return fTotNumValidBytes;
	}

	Boolean haveSeenEOF() const {
		return fHaveSeenEOF;
	}

	unsigned bankSize() const;

private:
	unsigned char* curBank() {
		return fCurBank;
	}
	unsigned char* nextToParse() {
		return &curBank()[fCurParserIndex];
	}
	unsigned char* lastParsed() {
		return &curBank()[fCurParserIndex - 1];
	}

	// makes sure that at least "numBytes" valid bytes remain:
	void ensureValidBytes(unsigned numBytesNeeded) {
		// common case: inlined:
		if (fCurParserIndex + numBytesNeeded <= fTotNumValidBytes)
			return;

		ensureValidBytes1(numBytesNeeded);
	}
	void ensureValidBytes1(unsigned numBytesNeeded);

	static void afterGettingBytes(void* clientData, unsigned numBytesRead,
			unsigned numTruncatedBytes, struct timeval presentationTime,
			unsigned durationInMicroseconds);
	void afterGettingBytes1(unsigned numBytesRead,
			struct timeval presentationTime);

	static void onInputClosure(void* clientData);
	void onInputClosure1();

private:
	FramedSource* fInputSource; // should be a byte-stream source??
	FramedSource::onCloseFunc* fClientOnInputCloseFunc;
	void* fClientOnInputCloseClientData;
	clientContinueFunc* fClientContinueFunc;
	void* fClientContinueClientData;

	// Use a pair of 'banks', and swap between them as they fill up:
	unsigned char* fBank[2];
	unsigned char fCurBankNum;
	unsigned char* fCurBank;

	// The most recent 'saved' parse position:
	unsigned fSavedParserIndex; // <= fCurParserIndex
	unsigned char fSavedRemainingUnparsedBits;

	// The current position of the parser within the current bank:
	unsigned fCurParserIndex; // <= fTotNumValidBytes
	unsigned char fRemainingUnparsedBits; // in previous byte: [0,7]

	// The total number of valid bytes stored in the current bank:
	unsigned fTotNumValidBytes; // <= BANK_SIZE

	// Whether we have seen EOF on the input source:
	Boolean fHaveSeenEOF;

	struct timeval fLastSeenPresentationTime; // hack used for EOF handling
};

#endif /* INCLUDE_STREAMPARSER_H_ */
