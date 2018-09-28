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
#include <pthread.h>
#include "DP_RTSP_CLIENT_GlobDefine.h"
#include "EfficientAddOrUpdate.h"
#include "DP_RTSP_CLIENT_DataQueue.h"
#include "Mutex.h"
#include "DP_RTSP_CLIENT_DataStructureDef.h"
#include "Logger.h"
using namespace FrameWork;
using namespace std;
class UsageEnvironment;
class DP_RTSP_CLIENT_FrameDataMemManage;
typedef DP_RTSP_CLIENT_FrameDataMemManage FrameDataMemManage;

static DP_U32 DP_RTSP_CLIENT_LinesOfStream = 0;
static DP_U16 DP_RTSP_CLIENT_ClientNum = 0;
static DP_U16 DP_RTSP_CLIENT_ClientMaxNum = 64;

class DP_RTSP_CLIENT_Client {
public:
	DP_RTSP_CLIENT_Client();
	~DP_RTSP_CLIENT_Client();

	DP_Bool DP_RTSP_CLIENT_GlobalSetting(DP_U16 u16MaxURLNum,
			DP_U16 URLNumsEachSche);

	//u32FrmNums:min :1 or 0 max :256
//	DP_S32 DP_RTSP_CLIENT_Init(const char *ps8URL,
//			DP_RTSP_CLIENT_STREAM_TYPE_E enStreamType,
//			DP_RTSP_CLIENT_NET_PROTOCOL_E enNetProtocol, DP_U16 u32FrmNums,
//			const DP_C_S8 *pu8UsrName, const DP_C_S8 *pu8UsrPassword);

	DP_S32 DP_RTSP_CLIENT_Init(const char *ps8URL,
			DP_RTSP_CLIENT_STREAM_TYPE_E enStreamType,
			DP_RTSP_CLIENT_NET_PROTOCOL_E enNetProtocol, DP_U16 u32FrmNums,
			const DP_C_S8 *pu8UsrName, const DP_C_S8 *pu8UsrPassword);

	DP_Bool DP_RTSP_CLIENT_GetMediaCodeParam(DP_U16 s32CliID,
			DP_RTSP_CLIENT_CODEC_PARAM_S *stCodeParam);

	DP_S32 DP_RTSP_CLIENT_GetStreamData(DP_RTSP_CLIENT_FRAME_DATA_S *stDataRecv,
			DP_U32 timeout, DP_Bool &firstPutout);

	DP_S32 DP_RTSP_CLIENT_Close(DP_U16 s32CliID);

	inline static DP_U8 DP_RTSP_CLIENT_H264NaluFlag(DP_U8 frame) {
		return (frame & 0x1F);
	}
	inline static DP_U8 DP_RTSP_CLIENT_H265NaluFlag(DP_U8 frame) {
		return (frame & 0x7E) >> 1;
	}

	static map<DP_U16, pthread_cond_t*> mCliCondSet;
	static map<DP_U16, pthread_mutex_t*> mCliMuxSet;
	static map<DP_U16, DP_RTSP_CLIENT_DataQueue*> _mDataQueueSet;

private:
	DP_U16 _u16ClientNum;
	DP_U16 _u16ClientMaxNum;
	DP_U32 _u32MaxFrameSize;
	vector<DP_U16> _vClientIDSet;
	Mutex _mutex;

	UsageEnvironment* _env;
//#pragma pack(push, 1)
	typedef struct _ClientInitArgs_S {
		_ClientInitArgs_S(DP_U16 cliID, const char *URL,
				DP_RTSP_CLIENT_STREAM_TYPE_E streamType,
				DP_RTSP_CLIENT_NET_PROTOCOL_E netProtocol, DP_U16 frmNums,
				const DP_C_S8 *usrName, const DP_C_S8 *usrPassword,
				DP_RTSP_CLIENT_Client *cli) :
				clientID(cliID), ps8URL(URL), enStreamType(streamType), enNetProtocol(
						netProtocol), u32FrmNums(frmNums), pu8UsrName(usrName), pu8UsrPassword(
						usrPassword), client(cli) {
		}
		DP_U16 clientID;
		const char *ps8URL;
		DP_RTSP_CLIENT_STREAM_TYPE_E enStreamType;
		DP_RTSP_CLIENT_NET_PROTOCOL_E enNetProtocol;
		DP_U16 u32FrmNums;
		const DP_C_S8 *pu8UsrName;
		const DP_C_S8 *pu8UsrPassword;
		DP_RTSP_CLIENT_Client *client;
	} ClientInitArgs_S;

	typedef struct _EnvScheID_S {
		_EnvScheID_S(UsageEnvironment* env, DP_U16 cliID) :
				_env(env), clientID(cliID) {
		}
		UsageEnvironment* _env;
		DP_U16 clientID;
	} EnvScheID_S;

//#pragma pack(pop))

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
