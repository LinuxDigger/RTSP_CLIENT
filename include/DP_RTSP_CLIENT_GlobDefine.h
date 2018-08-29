/*
 * DP_RTSP_CLIENT_GlobDefine.h
 *
 *  Created on: Aug 2, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_DP_RTSP_CLIENT_GLOBDEFINE_H_
#define INCLUDE_DP_RTSP_CLIENT_GLOBDEFINE_H_

// Defines unsigned types
typedef unsigned char           DP_U8;
typedef unsigned short          DP_U16;
typedef unsigned int            DP_U32;
typedef unsigned long long      DP_U64;

// Defines signed types
typedef char                    DP_S8;
typedef signed short            DP_S16;
typedef int                     DP_S32;
typedef signed long long        DP_S64;

typedef bool 					DP_Bool;

static const DP_Bool DP_True = true;
static const DP_Bool DP_False = false;
static const DP_S32 DP_RetOk = 0;
static const DP_S32 DP_RetError = -1;

#endif /* INCLUDE_DP_RTSP_CLIENT_GLOBDEFINE_H_ */
