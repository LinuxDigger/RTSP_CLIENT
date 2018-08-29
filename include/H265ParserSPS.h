/*
 * H265ParserSPS.h
 *
 *  Created on: Aug 29, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_H265PARSERSPS_H_
#define INCLUDE_H265PARSERSPS_H_

#include <string.h>

typedef unsigned char uint8;

typedef unsigned short uint16;

typedef unsigned long uint32;
typedef unsigned long long uint64;
typedef signed char int8;
typedef signed short int16;
typedef signed long int32;
typedef signed long long int64;

struct vc_params_t {
	long width, height;
	unsigned long profile, level;
	unsigned long nal_length_size;
	void clear() {
		memset(this, 0, sizeof(*this));
	}
};

bool ParseSequenceParameterSet(unsigned char * data, int size,
		vc_params_t& params);
class NALBitstream {
public:
	NALBitstream() :
			m_data(NULL), m_len(0), m_idx(0), m_bits(0), m_byte(0), m_zeros(0) {
	}
	NALBitstream(void * data, int len) {
		Init(data, len);
	}
	void Init(void * data, int len) {
		m_data = (unsigned char *) data;
		m_len = len;
		m_idx = 0;
		m_bits = 0;
		m_byte = 0;
		m_zeros = 0;
	}
	unsigned char GetBYTE() {
		if (m_idx >= m_len)
			return 0;
		unsigned char b = m_data[m_idx++];

		// to avoid start-code emulation, a byte 0x03 is inserted
		// after any 00 00 pair. Discard that here.
		if (b == 0) {
			m_zeros++;
			if ((m_idx < m_len) && (m_zeros == 2) && (m_data[m_idx] == 0x03)) {

				m_idx++;
				m_zeros = 0;
			}
		}

		else {
			m_zeros = 0;

		}
		return b;
	}
	;

	unsigned int GetBit() {

		if (m_bits == 0) {
			m_byte = GetBYTE();
			m_bits = 8;

		}
		m_bits--;
		return (m_byte >> m_bits) & 0x1;
	}
	;

	unsigned int GetWord(int bits) {

		unsigned int u = 0;
		while (bits > 0)

		{
			u <<= 1;
			u |= GetBit();
			bits--;
		}
		return u;
	}
	;
	unsigned int GetUE() {

		// Exp-Golomb entropy coding: leading zeros, then a one, then
		// the data bits. The number of leading zeros is the number of
		// data bits, counting up from that number of 1s as the base.
		// That is, if you see
		//      0001010
		// You have three leading zeros, so there are three data bits (010)
		// counting up from a base of 111: thus 111 + 010 = 1001 = 9
		int zeros = 0;
		while (m_idx < m_len && GetBit() == 0)
			zeros++;
		return GetWord(zeros) + ((1 << zeros) - 1);
	}
	;

	int GetSE() {

		// same as UE but signed.
		// basically the unsigned numbers are used as codes to indicate signed numbers in pairs
		// in increasing value. Thus the encoded values
		//      0, 1, 2, 3, 4
		// mean
		//      0, 1, -1, 2, -2 etc
		unsigned int UE = GetUE();
		bool positive = UE & 1;
		int SE = (UE + 1) >> 1;
		if (!positive) {
			SE = -SE;
		}
		return SE;
	}
	;

private:
	unsigned char * m_data;
	int m_len;
	int m_idx;
	int m_bits;
	unsigned char m_byte;
	int m_zeros;
};

#endif /* INCLUDE_H265PARSERSPS_H_ */
