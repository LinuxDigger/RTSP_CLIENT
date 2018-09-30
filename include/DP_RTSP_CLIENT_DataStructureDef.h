/*
 * DP_RTSP_CLIENT_DataStructureDef.h
 *
 *  Created on: Sep 11, 2018
 *      Author: jhb
 */
#ifndef INCLUDE_DATASTRUCTUREDEF_H_
#define INCLUDE_DATASTRUCTUREDEF_H_
#include <pthread.h>
#include "DP_RTSP_CLIENT_GlobDefine.h"
//#pragma pack(push, 1)
typedef enum _DP_RTSP_CLIENT_NET_PROTOCOL_E {
	DP_RTSP_CLIENT_NET_PROTOCOL_TCP,
	DP_RTSP_CLIENT_NET_PROTOCOL_UDP,
	DP_RTSP_CLINET_NET_PROTOCOL_BUTT
} DP_RTSP_CLIENT_NET_PROTOCOL_E;

typedef enum _DP_RTSP_CLIENT_STREAM_TYPE_E {
	DP_RTSP_CLIENT_STREAM_VIDEO,
	DP_RTSP_CLIENT_STREAM_AUDIO,
	DP_RTSP_CLIENT_STREAM_VIDEO_AND_AUDIO,
	DP_RTSP_CLIENT_STREAM_BUTT
} DP_RTSP_CLIENT_STREAM_TYPE_E;

//支持的流媒体编码格式枚举
typedef enum _DP_RTSP_CLINET_CODEC_TYPE_E {
	DP_RTSP_CLINET_CODEC_H264,
	DP_RTSP_CLINET_CODEC_H264_IFRAME,
	DP_RTSP_CLINET_CODEC_H265,
	DP_RTSP_CLINET_CODEC_H265_IFRAME,
	DP_RTSP_CLINET_CODEC_PCM,
	DP_RTSP_CLINET_CODEC_WAV,
	DP_RTSP_CLINET_CODEC_MP3,
	DP_RTSP_CLINET_CODEC_AAC,
	DP_RTSP_CLINET_CODEC_ADTS_AAC,
	DP_RTSP_CLINET_CODEC_LATM_AAC,
	DP_RTSP_CLINET_CODEC_BUTT
} DP_RTSP_CLINET_CODEC_TYPE_E;

typedef enum _DP_RTSP_CLIENT_H264_FRAMETYPE_E {
	DP_RTSP_CLIENT_H264_IDR_FRAMETYPE = 0x05,
	DP_RTSP_CLIENT_H264_SPS_FRAMETYPE = 0x07,
	DP_RTSP_CLIENT_H264_PPS_FRAMETYPE = 0x08
} DP_RTSP_CLIENT_H264_FRAMETYPE_E;

typedef enum _DP_RTSP_CLIENT_H265_FRAMETYPE_E {
	DP_RTSP_CLIENT_H265_IDR_RANGELOW_FRAMETYPE = 16,
	DP_RTSP_CLIENT_H265_IDR_RANGEHIGH_FRAMETYPE = 21,
	DP_RTSP_CLIENT_H265_VPS_FRAMETYPE = 32,
	DP_RTSP_CLIENT_H265_SPS_FRAMETYPE = 33,
	DP_RTSP_CLIENT_H265_PPS_FRAMETYPE = 34
} DP_RTSP_CLIENT_H265_FRAMETYPE_E;

typedef struct _DP_RTSP_CLIENT_CODEC_PARAM_S {
	DP_RTSP_CLINET_CODEC_TYPE_E enMediaType;
	union {
		struct _VIDEO_MEDIA_INFO_S {
//			DP_S32 s32VideoFrmRate;
//			DP_S32 s32VideoBitrate;
			DP_S32 s32VideoWidth;
			DP_S32 s32VideoHeigth;
		} stVidMediaInfo;
		struct Audio_Media_S {
//			DP_S32 s32AudioBitWidth;
//			DP_S32 s32AudioChannel;
//			DP_S32 s32AudioSampleRate;
		} stAudioMediaInfo;
	} _unMedia;
} DP_RTSP_CLIENT_CODEC_PARAM_S;

typedef struct _DP_RTSP_CLIENT_FRAME_DATA_S {
	_DP_RTSP_CLIENT_FRAME_DATA_S(DP_U16 u16CliID) :
			u16ClientID(u16CliID), pu8Data(NULL), u32Timestamp(0), u32FrameSize(
					0), u32MaxFrameSize(0), u32FrameSequence(0), enFrameType(
					DP_RTSP_CLINET_CODEC_BUTT), bIsCodecParamChanged(false) {
		pthread_rwlock_init(&rwLock, NULL);
	}
	DP_U16 u16ClientID;
	DP_U8* pu8Data;
	DP_U64 u32Timestamp;
	DP_U32 u32FrameSize;
	DP_U32 u32MaxFrameSize;
	DP_U32 u32FrameSequence;
	DP_RTSP_CLINET_CODEC_TYPE_E enFrameType;
	DP_Bool bIsCodecParamChanged;
//	DP_RTSP_CLIENT_CODEC_PARAM_S *stCodecParam;
	pthread_rwlock_t rwLock;
	void RDLock() {
		pthread_rwlock_rdlock(&rwLock);
	}
	void WRLock() {
		pthread_rwlock_wrlock(&rwLock);
	}
	void UNLock() {
		pthread_rwlock_unlock(&rwLock);
	}
} DP_RTSP_CLIENT_FRAME_DATA_S;

//#pragma pack(pop)

#endif /* INCLUDE_DP_RTSP_CLIENT_DATASTRUCTUREDEF_H_ */
