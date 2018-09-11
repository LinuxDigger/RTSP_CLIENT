/*
 * DP_RTSP_CLIENT_Interface.h
 *
 *  Created on: Aug 5, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_DP_RTSP_CLIENT_INTERFACE_H_
#define INCLUDE_DP_RTSP_CLIENT_INTERFACE_H_
#include <vector>
#include <string.h>
#include <map>
#include "DP_RTSP_CLIENT_GlobDefine.h"
#include "DP_RTSP_CLIENT_CycleQueue.h"
#include "EfficientAddOrUpdate.h"
#include "Mutex.h"
using namespace std;
//#pragma comment(lib, "pthreadVC2.lib")     //必须加上这句
class DP_RTSP_CLIENT_FrameDataMemManage;
typedef DP_RTSP_CLIENT_FrameDataMemManage FrameDataMemManage;
#pragma pack(push, 1)

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
	DP_RTSP_CLIENT_H264_IDR_FRAMETYPE = 5,
	DP_RTSP_CLIENT_H264_SPS_FRAMETYPE = 7,
	DP_RTSP_CLIENT_H264_PPS_FRAMETYPE = 8
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
					0), u32FrameSequence(0), enFrameType(
					DP_RTSP_CLINET_CODEC_BUTT), bIsCodecParamChanged(false) {
		pthread_rwlock_init(&rwLock, NULL);
	}
	DP_U16 u16ClientID;
	DP_U8* pu8Data;
	DP_U32 u32Timestamp;
	DP_U32 u32FrameSize;
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

#pragma pack(pop)

static DP_U32 DP_RTSP_CLIENT_LinesOfStream = 0;
static DP_U16 DP_RTSP_CLIENT_ClientNum = 0;
static DP_U16 DP_RTSP_CLIENT_ClientMaxNum = 64;

class DP_RTSP_CLIENT_Client {
public:
	DP_RTSP_CLIENT_Client();
	~DP_RTSP_CLIENT_Client();

	DP_Bool DP_RTSP_CLIENT_GlobalSetting(DP_U16 u16MaxURLNum =
			DP_RTSP_CLIENT_ClientMaxNum);

	//u32FrmNums:min :1 or 0 max :256
	DP_S32 DP_RTSP_CLIENT_Init(const char *ps8URL,
			DP_RTSP_CLIENT_STREAM_TYPE_E enStreamType,
			DP_RTSP_CLIENT_NET_PROTOCOL_E enNetProtocol, DP_U16 u32FrmNums,
			DP_U8 *pu8UsrName, DP_U8 *pu8UsrPassword);

	DP_Bool DP_RTSP_CLIENT_GetMediaCodeParam(DP_U16 s32CliID,
			DP_RTSP_CLIENT_CODEC_PARAM_S *stCodeParam);

//	DP_U32 DP_RTSPClient_StartPullStream(
//			DP_RTSPClient_TRANS_VIDEO_OR_AUDIO_E vOrA, DP_U8 *localFileName);
//	DP_RTSP_CLIENT_FRAME_DATA_S * DP_RTSPClient_GetDataStruct(DP_U16 clientID);
	DP_S32 DP_RTSP_CLIENT_GetStreamData(DP_RTSP_CLIENT_FRAME_DATA_S *stDataRecv,
			DP_U32 timeout, FrameDataMemManage &memManage);

//	DP_Bool DP_RTSPClient_StopPullStream(DP_S32 clientID);

	DP_S32 DP_RTSP_CLIENT_Close(DP_U16 s32CliID);

	static DP_U8 DP_RTSP_CLIENT_H264NaluFlag(DP_U8 frame) {
		return (frame & 0x1F);
	}
	static DP_U8 DP_RTSP_CLIENT_H265NaluFlag(DP_U8 frame) {
		return (frame & 0x7E) >> 1;
	}

	static map<DP_U16, pthread_cond_t*> mCliCondSet;
	static map<DP_U16, pthread_mutex_t*> mCliMuxSet;
	static map<DP_U16, DP_RTSP_CLIENT_CycleQueue*> _mDataQueueSet;

private:
	DP_U16 _u16ClientNum;
	DP_U16 _u16ClientMaxNum;
	DP_U32 _u32MaxFrameSize;
	vector<DP_U16> _vClientIDSet;
	Mutex _mutex;

	typedef struct _ClientInitArgs_S {
		_ClientInitArgs_S(DP_U16 cliID, const char *URL,
				DP_RTSP_CLIENT_STREAM_TYPE_E streamType,
				DP_RTSP_CLIENT_NET_PROTOCOL_E netProtocol, DP_U16 frmNums,
				DP_U8 *usrName, DP_U8 *usrPassword, DP_RTSP_CLIENT_Client *cli) :
				clientID(cliID), ps8URL(URL), enStreamType(streamType), enNetProtocol(
						netProtocol), u32FrmNums(frmNums), pu8UsrName(usrName), pu8UsrPassword(
						usrPassword), client(cli) {
		}
		DP_U16 clientID;
		const char *ps8URL;
		DP_RTSP_CLIENT_STREAM_TYPE_E enStreamType;
		DP_RTSP_CLIENT_NET_PROTOCOL_E enNetProtocol;
		DP_U16 u32FrmNums;
		DP_U8 *pu8UsrName;
		DP_U8 *pu8UsrPassword;
		DP_RTSP_CLIENT_Client *client;
	} ClientInitArgs_S;

	static void* sClientInit(void*args);
};

class DP_RTSP_CLIENT_FrameDataMemManage {
public:
	DP_RTSP_CLIENT_FrameDataMemManage() :
			_stDataRecv(NULL) {
	}
	~DP_RTSP_CLIENT_FrameDataMemManage() {
		if (_stDataRecv->pu8Data != NULL) {
			delete[] _stDataRecv->pu8Data;
			_stDataRecv->pu8Data = NULL;
		}
	}
	void DP_RTSP_CLIENT_SetFrameDataStruct(
			DP_RTSP_CLIENT_FRAME_DATA_S *stDataRecv) {
		_stDataRecv = stDataRecv;
	}
private:
	DP_RTSP_CLIENT_FRAME_DATA_S *_stDataRecv;
};

#endif /* INCLUDE_DP_RTSP_CLIENT_INTERFACE_H_ */
