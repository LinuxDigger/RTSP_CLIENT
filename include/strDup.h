/*
 * strDup.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_STRDUP_H_
#define INCLUDE_STRDUP_H_

#include <string.h>

char* strDup(char const* str);
// Note: strDup(NULL) returns NULL

char* strDupSize(char const* str);
// Like "strDup()", except that it *doesn't* copy the original.
// (Instead, it just allocates a string of the same size as the original.)

char* strDupSize(char const* str, size_t& resultBufSize);
// An alternative form of "strDupSize()" that also returns the size of the allocated buffer.

#endif /* INCLUDE_STRDUP_H_ */
