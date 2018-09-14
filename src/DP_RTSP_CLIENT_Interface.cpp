/*
 * DP_RTSP_CLIENT_Interface.cpp
 *
 *  Created on: Aug 6, 2018
 *      Author: jhb
 */

#include <iostream>
#include <time.h>
#include "DP_RTSP_CLIENT_Interface.h"
#include "Media.h"
#include "playCommon.h"
#include "BasicUsageEnvironment.h"
#include "BasicTaskScheduler.h"
#include "BasicTaskScheduler0.h"
#include "EfficientAddOrUpdate.h"
#include "CommonPlay.h"
#include "Logger.h"
using namespace FrameWork;
using namespace std;

map<DP_U16, pthread_cond_t*> DP_RTSP_CLIENT_Client::mCliCondSet;
map<DP_U16, pthread_mutex_t*> DP_RTSP_CLIENT_Client::mCliMuxSet;
map<DP_U16, DP_RTSP_CLIENT_DataQueue*> DP_RTSP_CLIENT_Client::_mDataQueueSet;

DP_RTSP_CLIENT_Client::DP_RTSP_CLIENT_Client() :
		_u16ClientNum(0), _u16ClientMaxNum(DP_RTSP_CLIENT_ClientMaxNum), _u32MaxFrameSize(
				0), _vClientIDSet(), _mutex() {
}

DP_RTSP_CLIENT_Client::~DP_RTSP_CLIENT_Client() {
}

DP_S32 DP_RTSP_CLIENT_Client::DP_RTSP_CLIENT_Init(const char *ps8URL,
		DP_RTSP_CLIENT_STREAM_TYPE_E enStreamType,
		DP_RTSP_CLIENT_NET_PROTOCOL_E enNetProtocol, DP_U16 u32FrmNums,
		const DP_C_S8 *pu8UsrName, const DP_C_S8 *pu8UsrPassword) {

	pthread_t tid;
	pthread_cond_t *cond = new pthread_cond_t;
	pthread_mutex_t *mutex = new pthread_mutex_t;
	pthread_cond_init(cond, NULL);
	pthread_mutex_init(mutex, NULL);
	_mutex.Lock();
	_u16ClientNum++;
	if ((_u16ClientNum <= (_u16ClientMaxNum - 1))
			&& (DP_RTSP_CLIENT_ClientNum < DP_RTSP_CLIENT_ClientMaxNum)) {
		efficientAddOrUpdate(mCliCondSet, ++_u16ClientNum, cond);
		efficientAddOrUpdate(mCliMuxSet, _u16ClientNum, mutex);
	}
	_mutex.Unlock();

	DP_U64 sec = 0;
	DP_U64 usec = 0;
	struct timeval tnow;
	struct timespec outtime;
	gettimeofday(&tnow, NULL);
	usec = tnow.tv_usec + 1000 * 1000;
	sec = tnow.tv_sec + usec / 1000000;
	outtime.tv_nsec = (usec % 1000000) * 1000;
	outtime.tv_sec = sec;

	ClientInitArgs_S stCliArgs(_u16ClientNum, ps8URL, enStreamType,
			enNetProtocol, u32FrmNums, pu8UsrName, pu8UsrPassword, this);
	if (pthread_create(&tid, NULL, sClientInit, (void*) &stCliArgs) == 0) {
		Logger::GetInstance().Debug(
				"Thread create successfully............................................!");
		DP_RTSP_CLIENT_DataQueue *dataQueue = new DP_RTSP_CLIENT_DataQueue(
				_u16ClientNum, u32FrmNums);
//		DP_RTSP_CLIENT_CycleQueue *cycQ = new DP_RTSP_CLIENT_CycleQueue(
//				u32FrmNums * sizeof(DP_RTSP_CLIENT_FRAME_DATA_S));
//		cycQ->DSP_QueueInit();
		efficientAddOrUpdate(_mDataQueueSet, _u16ClientNum, dataQueue);
		if (pthread_cond_timedwait(cond, mutex, &outtime) != 0) {
			Logger::GetInstance().Debug(
					"error cond ++++++++++++++++++++++++++++++++++++++++++++++++++++++!");
			//erase a queue
			//check!!!!!!!!!!
			_mDataQueueSet.erase(_u16ClientNum);
			///close connection
			return DP_RetError;
		} else {
			Logger::GetInstance().Info(
					"cond time wait == 0000000000000000000000000000000000000000000 : %d",
					_u16ClientNum);
			//init a queue according a uID.put it in map
		}
		pthread_cond_destroy(cond);
		pthread_mutex_destroy(mutex);
		delete cond;
		cond = NULL;
		delete mutex;
		mutex = NULL;
	} else {
		return DP_RetError;
	}

	return _u16ClientNum;
}

void* DP_RTSP_CLIENT_Client::sClientInit(void*args) {
	ClientInitArgs_S *stCliArgs = (ClientInitArgs_S*) args;
//	stCliArgs->client->_mutex.Lock();
	CommonPlay cp(stCliArgs->clientID);
	cp.setUsrnamePassword(stCliArgs->pu8UsrName, stCliArgs->pu8UsrPassword);
	UsageEnvironment* env;
	TaskScheduler* scheduler = BasicTaskScheduler::createNew(10000, &cp);
	env = BasicUsageEnvironment::createNew(*scheduler, stCliArgs->clientID); //111id?
//return fd
	cp.setEnvURL(*env, stCliArgs->ps8URL);
//	stCliArgs->client->_mutex.Unlock();
//	char const* progName;
//	openRTSP openRtsp;
	Medium* ourClient = cp.createClient(*env, stCliArgs->ps8URL, &cp);
	if (ourClient == NULL) {
		cp.shutdown();
	}
//mutex
//	if (DP_RTSP_CLIENT_ClientNum++ > DP_RTSP_CLIENT_ClientMaxNum - 1) {
//		return NULL;
//	}
	cp.continueAfterClientCreation1();
	Logger::GetInstance().Debug(
			"stCliArgs...IDXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX:%d ",
			env->_cliID);
	env->taskScheduler().doEventLoop(); // does not return
	return NULL;
}

DP_S32 DP_RTSP_CLIENT_Client::DP_RTSP_CLIENT_GetStreamData(
		DP_RTSP_CLIENT_FRAME_DATA_S *stDataRecv, DP_U32 timeout,
		FrameDataMemManage &memManage, DP_Bool &firstPutout) {
//	GetFrameData getFrameData(stDataRecv->u16ClientID);
//	getFrameData.afterGettingFrame(clientData, frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
	DP_U16 cliID = stDataRecv->u16ClientID;

//free mem the ptr
//	if (stDataRecv->pu8Data) {
//		delete[] stDataRecv->pu8Data;
//		stDataRecv->pu8Data = NULL;
//	}

	DP_S32 ret = _mDataQueueSet[cliID]->DP_RTSP_CLIENT_PutoutData(stDataRecv,
			DP_RTSP_CLIENT_STREAM_VIDEO_AND_AUDIO, timeout, firstPutout);
//	cout << "get stream data : "<<cliID<<endl;
//	_mDataQueueSet[cliID]->DSP_GetData(stDataRecv,
//			sizeof(DP_RTSP_CLIENT_FRAME_DATA_S));

	return ret;
}

DP_Bool DP_RTSP_CLIENT_Client::DP_RTSP_CLIENT_GetMediaCodeParam(DP_U16 s32CliID,
		DP_RTSP_CLIENT_CODEC_PARAM_S *stCodeParam) {

	return DP_True;
}
