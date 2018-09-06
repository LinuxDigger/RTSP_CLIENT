#ifndef  _RTSP_SYS_H_
#define _RTSP_SYS_H_

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

typedef enum _RTSP_BOOL_E{
	RTSP_FALSE = 0,
	RTSP_TRUE = 1,
	RTSP_MAX,
}RTSP_BOOL_E;


typedef unsigned char 		DP_U8;
typedef  signed char 		DP_S8;
typedef DP_U8*				DP_P_U8;
typedef DP_S8*				DP_P_S8;

typedef void  				DP_V;		
typedef DP_V*				DP_P_V;		

typedef unsigned short		DP_U16;		
typedef signed short		DP_S16;		
typedef DP_U16*				DP_P_U16;		
typedef DP_S16*				DP_P_S16;		


typedef unsigned int		DP_U32;		
typedef signed int			DP_S32;	
typedef DP_U32*				DP_P_U32;		
typedef DP_S32*				DP_P_S32;		

typedef unsigned long		DP_U32L;		
typedef signed long			DP_S32L;			
typedef DP_U32L*			DP_P_U32L;	
typedef DP_S32L*			DP_P_S32L;		


typedef unsigned long long		DP_U64L;		
typedef signed long long		DP_S64L;		
typedef DP_U64L*				DP_P_U64L;		
typedef DP_S64L*				DP_P_S64L;		


typedef float				DP_S32F;		
typedef DP_S32F*			DP_P_S32F;

typedef double				DP_S64F;		
typedef DP_S64F*			DP_P_S64F;		


typedef DP_S32		DP_HANDLE_ID;	

typedef DP_S32		DP_STREAM_ID;	

#endif
