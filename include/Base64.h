/*
 * Base64.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_BASE64_H_
#define INCLUDE_BASE64_H_

#include "Boolean.h"

DP_U8* base64Decode(char const* in, unsigned& resultSize,
		Boolean trimTrailingZeros = True);
// returns a newly allocated array - of size "resultSize" - that
// the caller is responsible for delete[]ing.

DP_U8* base64Decode(char const* in, unsigned inSize,
		unsigned& resultSize, Boolean trimTrailingZeros = True);
// As above, but includes the size of the input string (i.e., the number of bytes to decode) as a parameter.
// This saves an extra call to "strlen()" if we already know the length of the input string.

char* base64Encode(char const* orig, unsigned origLength);
// returns a 0-terminated string that
// the caller is responsible for delete[]ing.

#endif /* INCLUDE_BASE64_H_ */
