/*
 * DP_RTSP_CLIENT_DataQueue.cpp
 *
 *  Created on: Sep 10, 2018
 *      Author: jhb
 */

#include "DP_RTSP_CLIENT_DataQueue.h"
#include "DP_RTSP_CLIENT_Interface.h"
#include "Logger.h"
using namespace FrameWork;
class DP_RTSP_CLIENT_Client;

DP_RTSP_CLIENT_DataQueue::DP_RTSP_CLIENT_DataQueue(DP_U16 u16CliID,
		DP_U32 frameCnt) :
		_u32FrameCnt(frameCnt), _u32FrameIndex(0), _u32IDRFrameIndex(0), _u32AudioFrameIndex(
				0) {
	_vDataRecvSet.reserve(frameCnt);
	DP_RTSP_CLIENT_FRAME_DATA_S frameData(u16CliID);
	for (DP_U32 i = 0; i < frameCnt; i++)
		_vDataRecvSet.push_back(frameData);
}

DP_RTSP_CLIENT_DataQueue::~DP_RTSP_CLIENT_DataQueue() {
	for (DP_U32 i = 0; i < _u32FrameCnt; i++)
		if (_vDataRecvSet[i].pu8Data != NULL) {
			free(_vDataRecvSet[i].pu8Data);
			_vDataRecvSet[i].pu8Data = NULL;
		}
}
//typedef struct _DP_RTSP_CLIENT_FRAME_DATA_S {
//	_DP_RTSP_CLIENT_FRAME_DATA_S(DP_U16 u16CliID) :
//			u16ClientID(u16CliID), pu8Data(NULL), u32Timestamp(0), u32FrameSize(
//					0), u32FrameSequence(0), enFrameType(
//					DP_RTSP_CLINET_CODEC_BUTT), bIsCodecParamChanged(false) {
//		pthread_rwlock_init(&rwLock, NULL);
//	}
//	DP_U16 u16ClientID;
//	DP_U8* pu8Data;
//	DP_U32 u32Timestamp;
//	DP_U32 u32FrameSize;
//	DP_U32 u32FrameSequence;
//	DP_RTSP_CLINET_CODEC_TYPE_E enFrameType;
//	DP_Bool bIsCodecParamChanged;
////	DP_RTSP_CLIENT_CODEC_PARAM_S *stCodecParam;
//	pthread_rwlock_t rwLock;
//} DP_RTSP_CLIENT_FRAME_DATA_S;

DP_S32 DP_RTSP_CLIENT_DataQueue::DP_RTSP_CLIENT_RecvData(DP_U32 timestamp,
		DP_U32 frameSize, DP_U32 &frameSequence,
		DP_RTSP_CLINET_CODEC_TYPE_E frameType, DP_U8 *dataBuff) {
	_u32FrameIndex = frameSequence;
	DP_U32 frameSeqMod = frameSequence % _u32FrameCnt;
//	frameSequence %= _u32FrameCnt;
	_vDataRecvSet[frameSeqMod].WRLock();
	_vDataRecvSet[frameSeqMod].u32Timestamp = timestamp;
	_vDataRecvSet[frameSeqMod].enFrameType = frameType;
	_vDataRecvSet[frameSeqMod].u32FrameSequence = frameSequence;
	if (_vDataRecvSet[frameSeqMod].u32FrameSize < frameSize) {
		_vDataRecvSet[frameSeqMod].pu8Data = (DP_U8 *) realloc(
				_vDataRecvSet[frameSeqMod].pu8Data, frameSize);
		memset(_vDataRecvSet[frameSeqMod].pu8Data, 0, frameSize);
	} else
		memset(_vDataRecvSet[frameSeqMod].pu8Data, 0,
				_vDataRecvSet[frameSeqMod].u32FrameSize);
	_vDataRecvSet[frameSeqMod].u32FrameSize = frameSize;
	memcpy(_vDataRecvSet[frameSeqMod].pu8Data, dataBuff, frameSize);

	_vDataRecvSet[frameSeqMod].UNLock();
// figure out a video frame
	if (_vDataRecvSet[frameSeqMod].enFrameType
			<= DP_RTSP_CLINET_CODEC_H265_IFRAME) {
		//find IDR frame.
		Logger::GetInstance().Debug("dataBuff[4]:: %02x\n", dataBuff[4]);
		if (DP_RTSP_CLIENT_Client::DP_RTSP_CLIENT_H264NaluFlag(dataBuff[4])
				== DP_RTSP_CLIENT_H264_SPS_FRAMETYPE
				|| DP_RTSP_CLIENT_Client::DP_RTSP_CLIENT_H265NaluFlag(
						dataBuff[4]) == DP_RTSP_CLIENT_H265_VPS_FRAMETYPE)
			_u32IDRFrameIndex = _u32FrameIndex;

		//There is no IDR frame in _u32FrameCnt range .
		////remove >=
		if ((_u32FrameIndex - _u32IDRFrameIndex) > _u32FrameCnt) {
			//if 	_u32IDRFrameIndex ==0 return wait a new frame.
			if (_u32IDRFrameIndex != 0)
				_u32IDRFrameIndex = 0;
			else
				return -1;
		}
	}

//figure out a audio frame.
	if (_vDataRecvSet[frameSeqMod].enFrameType
			> DP_RTSP_CLINET_CODEC_H265_IFRAME) {
		_u32AudioFrameIndex = _u32FrameIndex;
		if (_u32FrameIndex - _u32AudioFrameIndex > _u32FrameCnt) {
			Logger::GetInstance().Info("_u32AudioFrameIndex::::::::::::: %d",
					_u32AudioFrameIndex);
			if (_u32AudioFrameIndex != 0)
				_u32AudioFrameIndex = 0;
			else
				return -2;
		}
	}
	Logger::GetInstance().Info(
			"DP_RTSP_CLIENT_RecvData() _u32IDRFrameIndex!!!!!!!!!!!!!!!!!!%d frameSequence :  ",
			_u32IDRFrameIndex, frameSequence);
	frameSequence++;

	return frameSize;
}

DP_U32 DP_RTSP_CLIENT_DataQueue::DP_RTSP_CLIENT_PutoutData(
		DP_RTSP_CLIENT_FRAME_DATA_S *stData,
		DP_RTSP_CLIENT_STREAM_TYPE_E streamType, DP_U32 timeout,
		DP_Bool &firstPutout) {
	Logger::GetInstance().Info(
			"DP_RTSP_CLIENT_PutoutData() :stData->u32FrameSequence :: %d",
			stData->u32FrameSequence);
	//first putout
//	if (stData->u32FrameSequence == 0) {
	if (firstPutout == true) {
		DP_U32 offset = 0;
		DP_U32 nextFrameIndex = 0;
		DP_U32 nextFrameIndexMod = 0;
		DP_U32 timeoutCnt = 0;
		DP_U32 IDRMod = _u32IDRFrameIndex % _u32FrameCnt;
		Logger::GetInstance().Info(
				"1DP_RTSP_CLIENT_PutoutData()111111111111111111111111putoutdata _u32IDRFrameIndex ::%d ",
				_u32IDRFrameIndex);
		//There is a IDR frame in _u32FrameCnt range .//seldom 0
		if (streamType == DP_RTSP_CLIENT_STREAM_VIDEO
				|| streamType == DP_RTSP_CLIENT_STREAM_VIDEO_AND_AUDIO) {
			//modify to 1111111111111111111111111111111111 from 0
			if (_u32IDRFrameIndex != 1) {
				//read lock
				copyData(IDRMod, stData);
				firstPutout = false;
				return stData->u32FrameSize;
			} else {

				while (1) {
					if ((nextFrameIndex = _u32IDRFrameIndex + offset + 1)
							< _u32FrameIndex) {
						nextFrameIndexMod = nextFrameIndex % _u32FrameCnt;
						if (isVideoStartFrame(nextFrameIndexMod)) {
							copyData(nextFrameIndexMod, stData);
							firstPutout = false;
							return stData->u32FrameSize;
						} else {
							//timeout wait
							offset++;
							if (isTimeOut(timeout, timeoutCnt) == true)
								return 0;
							else
								continue;
						}
					} else {
						//timeout wait
						offset++;
						if (isTimeOut(timeout, timeoutCnt) == true)
							return 0;
						else
							continue;
					}
				} //end while
			} //end if (streamType == DP_RTSP_CLIENT_STREAM_VIDEO
		} else if (streamType == DP_RTSP_CLIENT_STREAM_AUDIO
				|| streamType == DP_RTSP_CLIENT_STREAM_VIDEO_AND_AUDIO) {
			DP_U32 AudioMod = _u32AudioFrameIndex % _u32FrameCnt;
			if (_u32AudioFrameIndex != 0) {
				//read lock
				_vDataRecvSet[AudioMod].RDLock();
				memcpy(stData, &_vDataRecvSet[AudioMod],
						sizeof(DP_RTSP_CLIENT_FRAME_DATA_S));
				_vDataRecvSet[AudioMod].UNLock();
				firstPutout = false;
				return stData->u32FrameSize;
			} else {

				while (1) {
					if ((nextFrameIndex = _u32AudioFrameIndex + offset + 1)
							< _u32FrameIndex) {
						nextFrameIndexMod = nextFrameIndex % _u32FrameCnt;
						_vDataRecvSet[nextFrameIndexMod].RDLock();
						if (_vDataRecvSet[nextFrameIndexMod].enFrameType
								> DP_RTSP_CLINET_CODEC_H265_IFRAME) {
							memcpy(stData, &_vDataRecvSet[nextFrameIndexMod],
									sizeof(DP_RTSP_CLIENT_FRAME_DATA_S));
							_vDataRecvSet[nextFrameIndexMod].UNLock();
							firstPutout = false;
							return stData->u32FrameSize;
						} else {
							//timeout wait
							offset++;
							if (isTimeOut(timeout, timeoutCnt) == true)
								return 0;
							else
								continue;
						}
					} else {
						//timeout wait
						offset++;
						if (isTimeOut(timeout, timeoutCnt) == true)
							return 0;
						else
							continue;
					}
				} //end while
			}
		} // end if (streamType == DP_RTSP_CLIENT_STREAM_AUDIO
	} else {  //if (stData->u32FrameSequence == 0)
		//sequence +1 should < index otherwise wait
		DP_U32 offset = 0;
		DP_U32 nextFrmOffsetSeqMod = 0;
		DP_U32 timeoutCnt = 0;
		DP_U32 nextFrmSeq = stData->u32FrameSequence + 1;
		Logger::GetInstance().Info(
				"nextFrmSeq:::::::::::::::::::::::::::::::::::::::: %d _u32FrameIndex : %d",
				nextFrmSeq, _u32FrameIndex);
		while (1) {
			if (nextFrmSeq < _u32FrameIndex) {
				nextFrmOffsetSeqMod = (nextFrmSeq + offset) % _u32FrameCnt;
				//find video frame or audio frame.
				if (isAvailableFrame(nextFrmOffsetSeqMod, streamType)) {
					copyData(nextFrmOffsetSeqMod, stData);
					return stData->u32FrameSize;
				} else if (isUnvalidFrame(nextFrmOffsetSeqMod, streamType)) {
					if (nextFrmSeq + ++offset != _u32FrameIndex)
						continue;
					else {
						offset++;
						if (isTimeOut(timeout, timeoutCnt) == true)
							return 0;
						else
							continue;
					}
				} //end if (isAvailableFrame(nextFrmOffsetSeqMod, streamType))
			} else { //nextFrmSeq = _u32FrameIndex
				//timeout wait
//				offset++;
				if (isTimeOut(timeout, timeoutCnt) == true)
					return 0;
				else
					continue;
			} //if (nextFrmSeq < _u32FrameIndex)
		}  //end while
	}  //end if (stData->u32FrameSequence == 0)
	return 1;
}

void DP_RTSP_CLIENT_DataQueue::copyData(DP_U32 indexMod,
		DP_RTSP_CLIENT_FRAME_DATA_S*stData) {
	_vDataRecvSet[indexMod].RDLock();
	memcpy(stData, &_vDataRecvSet[indexMod],
			sizeof(DP_RTSP_CLIENT_FRAME_DATA_S));
	Logger::GetInstance().Info("copyData()_vDataRecvSet[indexMod].fraseq : %d",
			_vDataRecvSet[indexMod].u32FrameSequence);
	_vDataRecvSet[indexMod].UNLock();
}

DP_Bool DP_RTSP_CLIENT_DataQueue::isVideoStartFrame(DP_U32 nextFrameMod) {
	return (DP_RTSP_CLIENT_Client::DP_RTSP_CLIENT_H264NaluFlag(
			_vDataRecvSet[nextFrameMod].pu8Data[4])
			== DP_RTSP_CLIENT_H264_SPS_FRAMETYPE
			|| DP_RTSP_CLIENT_Client::DP_RTSP_CLIENT_H265NaluFlag(
					_vDataRecvSet[nextFrameMod].pu8Data[4])
					== DP_RTSP_CLIENT_H265_VPS_FRAMETYPE);
}

DP_Bool DP_RTSP_CLIENT_DataQueue::isAvailableFrame(DP_U32 nextFrameMod,
		DP_RTSP_CLIENT_STREAM_TYPE_E streamType) {
	return (((_vDataRecvSet[nextFrameMod].enFrameType < DP_RTSP_CLINET_CODEC_PCM)
			&& (streamType == DP_RTSP_CLIENT_STREAM_VIDEO
					|| streamType == DP_RTSP_CLIENT_STREAM_VIDEO_AND_AUDIO))
			|| ((_vDataRecvSet[nextFrameMod].enFrameType
					>= DP_RTSP_CLINET_CODEC_PCM)
					&& (streamType == DP_RTSP_CLIENT_STREAM_AUDIO
							|| streamType
									== DP_RTSP_CLIENT_STREAM_VIDEO_AND_AUDIO)));
}

DP_Bool DP_RTSP_CLIENT_DataQueue::isUnvalidFrame(DP_U32 nextFrameMod,
		DP_RTSP_CLIENT_STREAM_TYPE_E streamType) {
	return (((_vDataRecvSet[nextFrameMod].enFrameType < DP_RTSP_CLINET_CODEC_PCM)
			&& (streamType == DP_RTSP_CLIENT_STREAM_AUDIO))
			|| ((_vDataRecvSet[nextFrameMod].enFrameType
					>= DP_RTSP_CLINET_CODEC_PCM)
					&& (streamType == DP_RTSP_CLIENT_STREAM_VIDEO)));
}

//
//
//
//
//
//
//
//
//
