/*
 * DP_RTSP_CLIENT_DataQueue.cpp
 *
 *  Created on: Sep 10, 2018
 *      Author: jhb
 */

#include "DP_RTSP_CLIENT_DataQueue.h"

DP_RTSP_CLIENT_DataQueue::DP_RTSP_CLIENT_DataQueue(DP_U16 u16CliID,
		DP_U32 frameCnt) :
		_u32FrameCnt(frameCnt), _u32FrameIndex(0), _u32IDRFrameIndex(0), _u32AudioFrameIndex(
				0) {
//	_stDataRecv = new DP_RTSP_CLIENT_FRAME_DATA_S*[frameCnt];
//	for (DP_U32 i = 0; i < frameCnt; i++) {
//		_stDataRecv[i] = new DP_RTSP_CLIENT_FRAME_DATA_S(u16CliID);
//		_stDataRecv[i]->pu8Data = NULL;
//		_stDataRecv[i]->u32FrameSize = 0;
//	}
	_vDataRecvSet.reserve(frameCnt);
	DP_RTSP_CLIENT_FRAME_DATA_S frameData(u16CliID);
	for (DP_U32 i = 0; i < frameCnt; i++)
		_vDataRecvSet.push_back(frameData);
}

DP_RTSP_CLIENT_DataQueue::~DP_RTSP_CLIENT_DataQueue() {
//	for (DP_U32 i = 0; i < _u32FrameCnt; i++) {
//		delete _stDataRecv[i];
//		_stDataRecv[i] = NULL;
//	}
//	delete[] _stDataRecv;
//	_stDataRecv = NULL;
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
		DP_U32 frameSize, DP_U32 frameSequence,
		DP_RTSP_CLINET_CODEC_TYPE_E frameType, DP_U8 *dataBuff) {
	_u32FrameIndex = frameSequence;
	frameSequence %= _u32FrameCnt;
	_vDataRecvSet[frameSequence].u32Timestamp = timestamp;
	_vDataRecvSet[frameSequence].enFrameType = frameType;
	if (_vDataRecvSet[frameSequence].u32FrameSize < frameSize) {
		_vDataRecvSet[frameSequence].pu8Data = (DP_U8 *) realloc(
				_vDataRecvSet[frameSequence].pu8Data, frameSize);
		memset(_vDataRecvSet[frameSequence].pu8Data, 0, frameSize);
	} else
		memset(_vDataRecvSet[frameSequence].pu8Data, 0,
				_vDataRecvSet[frameSequence].u32FrameSize);
	memcpy(_vDataRecvSet[frameSequence].pu8Data, dataBuff, frameSize);

	// figure out a video frame
	if (_vDataRecvSet[frameSequence].enFrameType
			<= DP_RTSP_CLINET_CODEC_H265_IFRAME) {
		//find IDR frame.
		if (DP_RTSP_CLIENT_Client::DP_RTSP_CLIENT_H264NaluFlag(dataBuff[4])
				== DP_RTSP_CLIENT_H264_SPS_FRAMETYPE
				|| DP_RTSP_CLIENT_Client::DP_RTSP_CLIENT_H265NaluFlag(
						dataBuff[4]) == DP_RTSP_CLIENT_H265_VPS_FRAMETYPE)
			_u32IDRFrameIndex = _u32FrameIndex;
		//There is no IDR frame in _u32FrameCnt range .
		if ((_u32FrameIndex - _u32IDRFrameIndex) >= _u32FrameCnt) {
			//if 	_u32IDRFrameIndex ==0 return wait a new frame.
			if (_u32IDRFrameIndex != 0)
				_u32IDRFrameIndex = 0;
			else
				return -1;
		}
	}

	//figure out a audio frame.
	if (_vDataRecvSet[frameSequence].enFrameType
			> DP_RTSP_CLINET_CODEC_H265_IFRAME)
		_u32AudioFrameIndex = _u32FrameIndex;
	if (_u32FrameIndex - _u32AudioFrameIndex >= _u32FrameCnt) {
		if (_u32AudioFrameIndex != 0)
			_u32AudioFrameIndex = 0;
		else
			return -1;
	}
	return frameSize;
}

DP_U32 DP_RTSP_CLIENT_DataQueue::DP_RTSP_CLIENT_PutoutData(
		DP_RTSP_CLIENT_FRAME_DATA_S *stData,
		DP_RTSP_CLIENT_STREAM_TYPE_E streamType, DP_U32 timeout) {
	//first putout
	if (stData->u32FrameSequence == 0) {
		DP_U32 offset = 0;
		DP_U32 nextFrameIndex = 0;
		DP_U32 nextFrameIndexMod = 0;
		DP_U32 timeoutCnt = 0;
		DP_U32 IDRMod = _u32IDRFrameIndex % _u32FrameCnt;
		//There is a IDR frame in _u32FrameCnt range .//seldom 0
		if (streamType == DP_RTSP_CLIENT_STREAM_VIDEO
				|| streamType == DP_RTSP_CLIENT_STREAM_VIDEO_AND_AUDIO) {
			if (_u32IDRFrameIndex != 0) {
				//read lock
				copyData(IDRMod, stData);
				return _vDataRecvSet[IDRMod].u32FrameSize;
			} else {
				//timeout wait
				while ((nextFrameIndex = _u32IDRFrameIndex + offset + 1)
						< _u32FrameIndex) {
					nextFrameIndexMod = nextFrameIndex % _u32FrameCnt;
					if (isVideoStartFrame(nextFrameIndexMod)) {
						copyData(nextFrameIndexMod, stData);
						return _vDataRecvSet[nextFrameIndexMod].u32FrameSize;
					} else {
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
				return _vDataRecvSet[AudioMod].u32FrameSize;
			} else {
				//timeout wait
				while ((nextFrameIndex = _u32AudioFrameIndex + offset + 1)
						< _u32FrameIndex) {
					nextFrameIndexMod = nextFrameIndex % _u32FrameCnt;
					if (_vDataRecvSet[nextFrameIndexMod].enFrameType
							> DP_RTSP_CLINET_CODEC_H265_IFRAME) {
						copyData(nextFrameIndexMod, stData);
						return _vDataRecvSet[nextFrameIndexMod].u32FrameSize;
					} else {
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
		DP_U32 nextFrmSeq = stData->u32FrameSequence + 1;
		if (nextFrmSeq < _u32FrameIndex) {
			while (1) {
				nextFrmOffsetSeqMod = (nextFrmSeq + offset) % _u32FrameCnt;
				_vDataRecvSet[nextFrmOffsetSeqMod].RDLock();
				//find video frame or audio frame.
				if (((_vDataRecvSet[nextFrmOffsetSeqMod].enFrameType
						< DP_RTSP_CLINET_CODEC_PCM)
						&& (streamType == DP_RTSP_CLIENT_STREAM_VIDEO
								|| streamType
										== DP_RTSP_CLIENT_STREAM_VIDEO_AND_AUDIO))
						|| ((_vDataRecvSet[nextFrmOffsetSeqMod].enFrameType
								>= DP_RTSP_CLINET_CODEC_PCM)
								&& (streamType == DP_RTSP_CLIENT_STREAM_AUDIO
										|| streamType
												== DP_RTSP_CLIENT_STREAM_VIDEO_AND_AUDIO))) {
					memcpy(stData, &_vDataRecvSet[nextFrmOffsetSeqMod],
							_vDataRecvSet[nextFrmOffsetSeqMod].u32FrameSize);
					return stData->u32FrameSize;
//					break;
				}

				if (nextFrmSeq + offset != _u32FrameIndex) {
					_vDataRecvSet[nextFrmOffsetSeqMod].UNLock();
					offset++;
					continue;
				} else {
					//timeout wait
				}
			}

		} else {
			//timeout wait
		} //if (nextFrmSeq < _u32FrameIndex)
//		} else if (streamType == DP_RTSP_CLIENT_STREAM_AUDIO
//				|| streamType == DP_RTSP_CLIENT_STREAM_VIDEO_AND_AUDIO) {
//		}
	}  //if (stData->u32FrameSequence == 0)
}

void DP_RTSP_CLIENT_DataQueue::copyData(DP_U32 indexMod,
		DP_RTSP_CLIENT_FRAME_DATA_S*stData) {
	_vDataRecvSet[indexMod].RDLock();
	memcpy(stData, &_vDataRecvSet[indexMod],
			sizeof(DP_RTSP_CLIENT_FRAME_DATA_S));
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

//
//
//
//
//
//
//
//
//
