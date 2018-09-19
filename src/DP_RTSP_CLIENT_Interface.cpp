/*
 * DP_RTSP_CLIENT_Interface.cpp
 *
 *  Created on: Aug 6, 2018
 *      Author: jhb
 */

#include <iostream>
#include <time.h>
#include <errno.h>
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
				0), _vClientIDSet(), _mutex(), _env(NULL) {
}

DP_RTSP_CLIENT_Client::~DP_RTSP_CLIENT_Client() {
}

DP_Bool DP_RTSP_CLIENT_Client::DP_RTSP_CLIENT_GlobalSetting(DP_U16 u16MaxURLNum,
		DP_U16 URLNumsEachSche) {

	vector<TaskScheduler*> * scheduler = BasicTaskScheduler::createNew(
			u16MaxURLNum, URLNumsEachSche, 10000, NULL);
	_env = BasicUsageEnvironment::createNew(scheduler, 0);			//111id?

	return true;
}

//DP_S32 DP_RTSP_CLIENT_Client::DP_RTSP_CLIENT_Init(const char *ps8URL,
//		DP_RTSP_CLIENT_STREAM_TYPE_E enStreamType,
//		DP_RTSP_CLIENT_NET_PROTOCOL_E enNetProtocol, DP_U16 u32FrmNums,
//		const DP_C_S8 *pu8UsrName, const DP_C_S8 *pu8UsrPassword) {
//
//	pthread_t tid;
//	pthread_cond_t *cond = new pthread_cond_t;
//	pthread_mutex_t *mutex = new pthread_mutex_t;
//	pthread_cond_init(cond, NULL);
//	pthread_mutex_init(mutex, NULL);
//	_mutex.Lock();
////	_u16ClientNum++;
//	if ((_u16ClientNum <= (_u16ClientMaxNum - 1))
//			&& (DP_RTSP_CLIENT_ClientNum < DP_RTSP_CLIENT_ClientMaxNum)) {
//		efficientAddOrUpdate(mCliCondSet, ++_u16ClientNum, cond);
//		efficientAddOrUpdate(mCliMuxSet, _u16ClientNum, mutex);
//	}
//	_mutex.Unlock();
//
//	DP_U64 sec = 0;
//	DP_U64 usec = 0;
//	struct timeval tnow;
//	struct timespec outtime;
//	gettimeofday(&tnow, NULL);
//	usec = tnow.tv_usec + 1000 * 1000;
//	sec = tnow.tv_sec + usec / 1000000;
//	outtime.tv_nsec = (usec % 1000000) * 1000;
//	outtime.tv_sec = sec;
//
//	ClientInitArgs_S stCliArgs(_u16ClientNum, ps8URL, enStreamType,
//			enNetProtocol, u32FrmNums, pu8UsrName, pu8UsrPassword, this);
//	if (pthread_create(&tid, NULL, sClientInit, (void*) &stCliArgs) == 0) {
//		Logger::GetInstance().Debug(
//				"Thread create successfully............................................!");
//		DP_RTSP_CLIENT_DataQueue *dataQueue = new DP_RTSP_CLIENT_DataQueue(
//				_u16ClientNum, u32FrmNums);
//		efficientAddOrUpdate(_mDataQueueSet, _u16ClientNum, dataQueue);
//		cout << "_u16ClientNum : " << _u16ClientNum << endl;
//		for (unsigned short i = 2; i < 2; i++) {
//			DP_RTSP_CLIENT_DataQueue *dataQueue = new DP_RTSP_CLIENT_DataQueue(
//					i, u32FrmNums);
//			efficientAddOrUpdate(_mDataQueueSet, i, dataQueue);
//			cout << "i: 1: " << i << endl;
//		}
//#if 0
//		if (pthread_cond_timedwait(cond, mutex, &outtime) != 0) {
//			Logger::GetInstance().Debug(
//					"error cond ++++++++++++++++++++++++++++++++++++++++++++++++++++++!");
//			//erase a queue
//			//check!!!!!!!!!!
//			_mDataQueueSet.erase(_u16ClientNum);
//			///close connection
//			return DP_RetError;
//		} else {
//			Logger::GetInstance().Info(
//					"cond time wait == 0000000000000000000000000000000000000000000 : %d",
//					_u16ClientNum);
//			//init a queue according a uID.put it in map
//		}
//		pthread_cond_destroy(cond);
//		pthread_mutex_destroy(mutex);
//		delete cond;
//		cond = NULL;
//		delete mutex;
//		mutex = NULL;
//#endif
//	} else {
//		return DP_RetError;
//	}
//	sleep(1);
//	return _u16ClientNum;
//}

DP_S32 DP_RTSP_CLIENT_Client::DP_RTSP_CLIENT_Init(const char *ps8URL,
		DP_RTSP_CLIENT_STREAM_TYPE_E enStreamType,
		DP_RTSP_CLIENT_NET_PROTOCOL_E enNetProtocol, DP_U16 u32FrmNums,
		const DP_C_S8 *pu8UsrName, const DP_C_S8 *pu8UsrPassword) {

	DP_U16 cliID = _env->getAIdleClientFromEnv();
	if (cliID == 0) {
		Logger::GetInstance().Error("no cliID");
	} else {
		Logger::GetInstance().Warn("cliID:: in init : %d", cliID);
	}

	CommonPlay *cp = new CommonPlay(cliID);
	Logger::GetInstance().Warn("DP_RTSP_CLIENT_Init::: %s ", ps8URL);
	cp->setUsrnamePassword(pu8UsrName, pu8UsrPassword);
	Logger::GetInstance().Warn(
			"stCliArgs->pu8UsrName %s,  stCliArgs->pu8UsrPassword %s",
			pu8UsrName, pu8UsrPassword);
	_env->taskScheduler(cliID / 10)->_mClientSet[cliID] = cp->createClient(
			*_env, ps8URL, cp);
	cp->setEnvURL(*_env, ps8URL);
	if (_env->taskScheduler(cliID / 10)->_mClientSet[cliID] == NULL) {
		cp->shutdown();
	}
	cp->continueAfterClientCreation1();
//	sleep(2);
#if 1
	if (!_env->taskScheduler(cliID / 10)->taskScheThreadEnable()) {
		_env->taskScheduler(cliID / 10)->setScheThreadStatus(true);
//		thread
		pthread_t tid;
		EnvScheID_S stEnvScheID(_env, cliID / 10);
		if (pthread_create(&tid, NULL, sClientInit, (void*) &stEnvScheID)
				== 0) {
			Logger::GetInstance().Debug(
					"Thread create successfully............................................!");
			DP_RTSP_CLIENT_DataQueue *dataQueue = new DP_RTSP_CLIENT_DataQueue(
					cliID, u32FrmNums);
			efficientAddOrUpdate(_mDataQueueSet, cliID, dataQueue);
			sleep(1);
		} else {
			Logger::GetInstance().Error(
					"Thread create failed !code : %d errno: %s ", errno,
					strerror(errno));
			return 0;
		}
	} else {
		Logger::GetInstance().Info("Thread enable true !");
	}
#endif
	return cliID;
}

void* DP_RTSP_CLIENT_Client::sClientInit(void*args) {
//	ClientInitArgs_S *stCliArgs = (ClientInitArgs_S*) args;
	EnvScheID_S* stEnvScheID = (EnvScheID_S*) args;
//	DP_U8 *url= stCliArgs->ps8URL;
#if 0

	for (unsigned short i = 1; i < 2; i++) {
		cout << "i: 2: " << i << endl;
		CommonPlay *cp = new CommonPlay(i);
		Logger::GetInstance().Warn("stcliarg::: %s ", stCliArgs->ps8URL);
		cp->setUsrnamePassword(stCliArgs->pu8UsrName,
				stCliArgs->pu8UsrPassword);
		Logger::GetInstance().Warn(
				"stCliArgs->pu8UsrName %s,  stCliArgs->pu8UsrPassword %s",
				stCliArgs->pu8UsrName, stCliArgs->pu8UsrPassword);
		Medium* ourClient = cp->createClient(*env, stCliArgs->ps8URL, cp);
		cp->setEnvURL(*env, stCliArgs->ps8URL);
		if (ourClient == NULL) {
			cp->shutdown();
		}
		cp->continueAfterClientCreation1();
	}
	sleep(1);

#endif
#if 0
	UsageEnvironment* env;
	TaskScheduler* scheduler = BasicTaskScheduler::createNew(10000, NULL);
	env = BasicUsageEnvironment::createNew(*scheduler, 0);			//111id?
	for (unsigned short i = 1; i < 2; i++) {
		cout << "i: 2: " << i << endl;
		CommonPlay *cp = new CommonPlay(i);
		Logger::GetInstance().Warn("stcliarg::: %s ", stCliArgs->ps8URL);
		cp->setUsrnamePassword(stCliArgs->pu8UsrName,
				stCliArgs->pu8UsrPassword);
		Logger::GetInstance().Warn(
				"stCliArgs->pu8UsrName %s,  stCliArgs->pu8UsrPassword %s",
				stCliArgs->pu8UsrName, stCliArgs->pu8UsrPassword);
		Medium* ourClient = cp->createClient(*env, stCliArgs->ps8URL, cp);
		cp->setEnvURL(*env, stCliArgs->ps8URL);
		if (ourClient == NULL) {
			cp->shutdown();
		}
		cp->continueAfterClientCreation1();
	}
	sleep(1);
#endif

#if 0
	CommonPlay cp(stCliArgs->clientID);
	cp.setUsrnamePassword(stCliArgs->pu8UsrName, stCliArgs->pu8UsrPassword);
	UsageEnvironment* env;
	cout << "stCliArgs->clientID--------------------------------------------:: "
	<< stCliArgs->clientID << " stCliArgs->ps8URL:: "
	<< stCliArgs->ps8URL << endl;
	TaskScheduler* scheduler = BasicTaskScheduler::createNew(10000, &cp);
	env = BasicUsageEnvironment::createNew(*scheduler, stCliArgs->clientID); //111id?
//return fd
	Medium* ourClient = cp.createClient(*env, stCliArgs->ps8URL, &cp);
	cp.setEnvURL(*env, stCliArgs->ps8URL);
	if (ourClient == NULL) {
		cp.shutdown();
	}
	cp.continueAfterClientCreation1();
#endif
	stEnvScheID->_env->taskScheduler(stEnvScheID->clientID)->doEventLoop(); // does not return
	return NULL;
}

DP_S32 DP_RTSP_CLIENT_Client::DP_RTSP_CLIENT_GetStreamData(
		DP_RTSP_CLIENT_FRAME_DATA_S *stDataRecv, DP_U32 timeout,
		FrameDataMemManage &memManage, DP_Bool &firstPutout) {
	DP_U16 cliID = stDataRecv->u16ClientID;
	if (_mDataQueueSet[cliID] == NULL)
		return -1;
	DP_S32 ret = _mDataQueueSet[cliID]->DP_RTSP_CLIENT_PutoutData(stDataRecv,
			DP_RTSP_CLIENT_STREAM_VIDEO_AND_AUDIO, timeout, firstPutout);
	return ret;
}

DP_Bool DP_RTSP_CLIENT_Client::DP_RTSP_CLIENT_GetMediaCodeParam(DP_U16 s32CliID,
		DP_RTSP_CLIENT_CODEC_PARAM_S *stCodeParam) {

	return DP_True;
}
