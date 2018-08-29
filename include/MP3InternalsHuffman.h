/*
 * MP3InternalsHuffman.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MP3INTERNALSHUFFMAN_H_
#define INCLUDE_MP3INTERNALSHUFFMAN_H_

#include "MP3Internals.h"



void updateSideInfoForHuffman(MP3SideInfo& sideInfo, Boolean isMPEG2,
		unsigned char const* mainDataPtr, unsigned p23L0, unsigned p23L1,
		unsigned& part23Length0a, unsigned& part23Length0aTruncation,
		unsigned& part23Length0b, unsigned& part23Length0bTruncation,
		unsigned& part23Length1a, unsigned& part23Length1aTruncation,
		unsigned& part23Length1b, unsigned& part23Length1bTruncation);

#define SSLIMIT 18

class MP3HuffmanEncodingInfo {
public:
	MP3HuffmanEncodingInfo(Boolean includeDecodedValues = False);
	~MP3HuffmanEncodingInfo();

public:
	unsigned numSamples;
	unsigned allBitOffsets[SBLIMIT * SSLIMIT + 1];
	unsigned reg1Start, reg2Start, bigvalStart; /* special bit offsets */
	unsigned* decodedValues;
};

/* forward */
void MP3HuffmanDecode(MP3SideInfo::gr_info_s_t* gr, Boolean isMPEG2,
		unsigned char const* fromBasePtr, unsigned fromBitOffset,
		unsigned fromLength, unsigned& scaleFactorsLength,
		MP3HuffmanEncodingInfo& hei);

extern unsigned char huffdec[]; // huffman table datas

// The following are used if we process Huffman-decoded values
#ifdef FOUR_BYTE_SAMPLES
#define BYTES_PER_SAMPLE_VALUE 4
#else
#ifdef TWO_BYTE_SAMPLES
#define BYTES_PER_SAMPLE_VALUE 2
#else
// ONE_BYTE_SAMPLES
#define BYTES_PER_SAMPLE_VALUE 1
#endif
#endif

#ifdef DO_HUFFMAN_ENCODING
unsigned MP3HuffmanEncode(MP3SideInfo::gr_info_s_t const* gr,
		unsigned char const* fromPtr,
		unsigned char* toPtr, unsigned toBitOffset,
		unsigned numHuffBits);
#endif

#endif /* INCLUDE_MP3INTERNALSHUFFMAN_H_ */
