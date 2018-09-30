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
	frameData.pu8Data = NULL;
//	frameData.pu8Data = new DP_U8[450000];
	for (DP_U32 i = 0; i < frameCnt; i++)
		_vDataRecvSet.push_back(frameData);
}

DP_RTSP_CLIENT_DataQueue::~DP_RTSP_CLIENT_DataQueue() {
	for (DP_U32 i = 0; i < _u32FrameCnt; i++)
		if (_vDataRecvSet[i].pu8Data != NULL) {
			delete[] _vDataRecvSet[i].pu8Data;
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

DP_S32 DP_RTSP_CLIENT_DataQueue::DP_RTSP_CLIENT_RecvData(DP_U64 timestamp,
		DP_U32 frameSize, DP_U32 &frameSequence,
		DP_RTSP_CLINET_CODEC_TYPE_E frameType, DP_U8 *dataBuff) {
#if 1
	_u32FrameIndex = frameSequence;
	DP_U32 frameSeqMod = frameSequence % _u32FrameCnt;
//	frameSequence %= _u32FrameCnt;
//	cout << "1111111-------------------------------------_vDataRecvSet.size: "
//			<< _vDataRecvSet.size() << " frameSequence: " << frameSequence
//			<< endl;
	_vDataRecvSet[frameSeqMod].WRLock();
//	cout << "-2222222------------------------------------_vDataRecvSet.size: "
//			<< _vDataRecvSet.size() << " frameSequence: " << frameSequence
//			<< endl;
	_vDataRecvSet[frameSeqMod].u32Timestamp = timestamp;
	_vDataRecvSet[frameSeqMod].enFrameType = frameType;
	_vDataRecvSet[frameSeqMod].u32FrameSequence = frameSequence;
//	cout << "_vDataRecvSet[frameSeqMod].u32FrameSize < frameSize : "
//			<< _vDataRecvSet[frameSeqMod].u32FrameSize << " " << frameSize
//			<< endl;
	if (_vDataRecvSet[frameSeqMod].u32MaxFrameSize < frameSize) {
		if (_vDataRecvSet[frameSeqMod].pu8Data != NULL) {
			delete[] _vDataRecvSet[frameSeqMod].pu8Data;
			_vDataRecvSet[frameSeqMod].pu8Data = NULL;
		}
//		cout << "newwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww1:"
//				<< frameSize << " frameSeqMod].u32FrameSize "
//				<< _vDataRecvSet[frameSeqMod].u32FrameSize << " frameSeqMod "<<frameSeqMod<<endl;
//		cout
//				<< "newwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww _vDataRecvSet[frameSeqMod].u32MaxFrameSize : "
//				<< _vDataRecvSet[frameSeqMod].u32MaxFrameSize << " frameSize "
//				<< frameSize << endl;
		_vDataRecvSet[frameSeqMod].pu8Data = new DP_U8[frameSize];
//		_vDataRecvSet[frameSeqMod].pu8Data = (DP_U8 *) realloc(
//				_vDataRecvSet[frameSeqMod].pu8Data, frameSize);
		memset(_vDataRecvSet[frameSeqMod].pu8Data, 0, frameSize);
		_vDataRecvSet[frameSeqMod].u32MaxFrameSize = frameSize;
	} else
		memset(_vDataRecvSet[frameSeqMod].pu8Data, 0,
				_vDataRecvSet[frameSeqMod].u32MaxFrameSize);
	///should be optimized !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!111
	_vDataRecvSet[frameSeqMod].u32FrameSize = frameSize;
	memcpy(_vDataRecvSet[frameSeqMod].pu8Data, dataBuff, frameSize);

	_vDataRecvSet[frameSeqMod].UNLock();
//	cout <<"recvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv : "<<frameSequence<<endl;
	frameSequence++;
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
//	frameSequence++;

#endif
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
	if (firstPutout == true) {
		DP_U32 offset = 0;
		DP_U32 nextFrameIndex = 0;
		DP_U32 nextFrameIndexMod = 0;
		DP_U32 timeoutCnt = 0;
		DP_U32 IDRMod = _u32IDRFrameIndex % _u32FrameCnt;
		Logger::GetInstance().Info(
				"1DP_RTSP_CLIENT_PutoutData()1111putoutdata _u32IDRFrameIndex ::%d ",
				_u32IDRFrameIndex);
		//There is a IDR frame in _u32FrameCnt range .//seldom 0
		if (isAVideoFrame(streamType)) {
			//modify to 1111111111111111111111111111111111 from 0
			if (_u32IDRFrameIndex != 1) {
				//read lock
				copyVideoData(IDRMod, stData);
				firstPutout = false;
				return stData->u32FrameSize;
			} else {
				//_u32IDRFrameIndex == 1
				while (1) {
					if ((nextFrameIndex = _u32IDRFrameIndex + offset + 1)
							<= _u32FrameIndex) {
						nextFrameIndexMod = nextFrameIndex % _u32FrameCnt;
						if (isVideoStartFrame(nextFrameIndexMod)) {
							copyVideoData(nextFrameIndexMod, stData);
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
		} else if (isAAudioFrame(streamType)) {
			DP_U32 AudioMod = _u32AudioFrameIndex % _u32FrameCnt;
			if (_u32AudioFrameIndex != 0) {
				//read lock
				copyAudioData(AudioMod, stData);
				firstPutout = false;
				return stData->u32FrameSize;
			} else {
				while (1) {
					if ((nextFrameIndex = _u32AudioFrameIndex + offset + 1)
							<= _u32FrameIndex) {
						nextFrameIndexMod = nextFrameIndex % _u32FrameCnt;
						if (_vDataRecvSet[nextFrameIndexMod].enFrameType
								> DP_RTSP_CLINET_CODEC_H265_IFRAME) {
							copyAudioData(nextFrameIndexMod, stData);
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
	} else {  //if (stData->u32FrameSequence == 0) 	if (firstPutout == true
		//sequence +1 should < index otherwise waiting
		DP_U32 offset = 0;
		DP_U32 nextFrmOffsetSeqMod = 0;
		DP_U32 timeoutCnt = 0;
		DP_U32 nextFrmSeq = stData->u32FrameSequence + 1;
		Logger::GetInstance().Info(
				"nextFrmSeq:::::::::::::::::::::::::::::::::::::::: %d _u32FrameIndex : %d",
				nextFrmSeq, _u32FrameIndex);
		while (1) {
			if (nextFrmSeq + offset <= _u32FrameIndex) {
//				cout <<"nextFrmSeq + offset11111111 :: "<<nextFrmSeq + offset <<" _u32FrameIndex :: "<<_u32FrameIndex<<endl;
				//can not find next frame
				if ((_u32FrameIndex - nextFrmSeq) >= _u32FrameCnt) {
//					cout <<"nextFrmSeq + offset 22222:: "<<nextFrmSeq + offset <<" _u32FrameIndex :: "<<_u32FrameIndex<<endl;
					if (nextFrmSeq
							!= _vDataRecvSet[nextFrmSeq % _u32FrameCnt].u32FrameSequence)
						cout << "errorrrrrrrrrrrrrrrrrrrrrrrrrrr111111111";
					else
						cout << "erorrrrrrrrrrrrrrrrrrrrrrrrrrrrr222222222222";
					if (isAAudioFrame(streamType)) {
						nextFrmOffsetSeqMod = _u32FrameIndex % _u32FrameCnt;
						copyVideoData(nextFrmOffsetSeqMod, stData);
						return stData->u32FrameSize;
					} else {
						DP_U32 findIDRTimeOut = 0;
						//wait for a IDR
						while (1) {
							if ((_u32FrameIndex - _u32IDRFrameIndex)
									< _u32FrameCnt) {
								nextFrmOffsetSeqMod = _u32IDRFrameIndex
										% _u32FrameCnt;
								copyVideoData(nextFrmOffsetSeqMod, stData);
								return stData->u32FrameSize;
							} //if ((_u32FrameIndex - _u32IDRFrameIndex) < _u32FrameCnt)
							else {
								if (isTimeOut(1000, findIDRTimeOut) == true)
									return 0;
								else
									continue;
							}
						} //end  while (1)
					}  //	if (isAAudioFrame(streamType))
				}  //if ((_u32FrameIndex - nextFrmSeq) >= _u32FrameCnt)
				else {
					nextFrmOffsetSeqMod = (nextFrmSeq + offset) % _u32FrameCnt;
					//find video frame or audio frame.
					if (isAvailableFrame(nextFrmOffsetSeqMod, streamType)) {
						copyVideoData(nextFrmOffsetSeqMod, stData);
						return stData->u32FrameSize;
					} else if (isUnvalidFrame(nextFrmOffsetSeqMod,
							streamType)) {
						if (nextFrmSeq + ++offset <= _u32FrameIndex)
							continue;
						else {
//							offset++;
							if (isTimeOut(timeout, timeoutCnt) == true)
								return 0;
							else
								continue;
						}
					}
				}
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

void DP_RTSP_CLIENT_DataQueue::copyVideoData(DP_U32 indexMod,
		DP_RTSP_CLIENT_FRAME_DATA_S*stData) {
	_vDataRecvSet[indexMod].RDLock();
	DP_U32 frameSize = _vDataRecvSet[indexMod].u32FrameSize;
	if (stData->u32MaxFrameSize < frameSize) {
		delete[] stData->pu8Data;
		stData->pu8Data = NULL;
		stData->pu8Data = new DP_U8[frameSize];
		memset(stData->pu8Data, 0, frameSize);
		stData->u32MaxFrameSize = frameSize;
	} else
		memset(stData->pu8Data, 0, stData->u32MaxFrameSize);

	memcpy(stData->pu8Data, _vDataRecvSet[indexMod].pu8Data, frameSize);
	stData->u32FrameSequence = _vDataRecvSet[indexMod].u32FrameSequence;
	stData->u32FrameSize = frameSize;
	stData->u32Timestamp = _vDataRecvSet[indexMod].u32Timestamp;

	Logger::GetInstance().Info("copyData()_vDataRecvSet[indexMod].fraseq : %d",
			_vDataRecvSet[indexMod].u32FrameSequence);
	_vDataRecvSet[indexMod].UNLock();
}

void DP_RTSP_CLIENT_DataQueue::copyAudioData(DP_U32 indexMod,
		DP_RTSP_CLIENT_FRAME_DATA_S*stData) {
	_vDataRecvSet[indexMod].RDLock();
	DP_U32 frameSize = _vDataRecvSet[indexMod].u32FrameSize;
	if (stData->u32MaxFrameSize < frameSize) {
		delete[] stData->pu8Data;
		stData->pu8Data = NULL;
		stData->pu8Data = new DP_U8[frameSize];
		memset(stData->pu8Data, 0, frameSize);
		stData->u32MaxFrameSize = frameSize;
	} else
		memset(stData->pu8Data, 0, stData->u32MaxFrameSize);
	memcpy(stData->pu8Data, _vDataRecvSet[indexMod].pu8Data, frameSize);
	stData->u32FrameSequence = _vDataRecvSet[indexMod].u32FrameSequence;
	stData->u32Timestamp = _vDataRecvSet[indexMod].u32Timestamp;
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

DP_Bool DP_RTSP_CLIENT_DataQueue::isAAudioFrame(
		DP_RTSP_CLIENT_STREAM_TYPE_E streamType) {
	return (streamType == DP_RTSP_CLIENT_STREAM_AUDIO
			|| streamType == DP_RTSP_CLIENT_STREAM_VIDEO_AND_AUDIO);
}

DP_Bool DP_RTSP_CLIENT_DataQueue::isAVideoFrame(
		DP_RTSP_CLIENT_STREAM_TYPE_E streamType) {
	return (streamType == DP_RTSP_CLIENT_STREAM_VIDEO
			|| streamType == DP_RTSP_CLIENT_STREAM_VIDEO_AND_AUDIO);
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
