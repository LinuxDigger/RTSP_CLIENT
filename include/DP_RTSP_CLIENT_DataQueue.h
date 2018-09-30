/*
 * DP_RTSP_CLIENT_DataQueue.h
 *
 *  Created on: Sep 10, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_DP_RTSP_CLIENT_DATAQUEUE_H_
#define INCLUDE_DP_RTSP_CLIENT_DATAQUEUE_H_

#include "DP_RTSP_CLIENT_GlobDefine.h"
#include <unistd.h>
#include "DP_RTSP_CLIENT_DataStructureDef.h"
#include <vector>
using namespace std;

class DP_RTSP_CLIENT_DataQueue {
public:
	DP_RTSP_CLIENT_DataQueue(DP_U16 u16CliID, DP_U32 frameCnt);
	~DP_RTSP_CLIENT_DataQueue();

	DP_S32 DP_RTSP_CLIENT_RecvData(DP_U64 timestamp, DP_U32 frameSize,
			DP_U32 &frameSequence, DP_RTSP_CLINET_CODEC_TYPE_E frameType,
			DP_U8 *dataBuff);
	DP_U32 DP_RTSP_CLIENT_PutoutData(DP_RTSP_CLIENT_FRAME_DATA_S *stData,
			DP_RTSP_CLIENT_STREAM_TYPE_E streamType, DP_U32 timeout,
			DP_Bool &firstPutout);

private:

	DP_U32 _u32FrameCnt;
//	DP_RTSP_CLIENT_FRAME_DATA_S **_stDataRecv;
	DP_U32 _u32FrameIndex;
	DP_U32 _u32IDRFrameIndex;
	DP_U32 _u32AudioFrameIndex;
	vector<DP_RTSP_CLIENT_FRAME_DATA_S> _vDataRecvSet;

	inline DP_Bool isTimeOut(DP_U32 timeout, DP_U32 &cnt) {
		usleep(5000);
		cnt++;
		if (cnt * 5000 >= timeout * 1000)
			return true;
		else
			return false;
	}

	void copyVideoData(DP_U32 indexMod, DP_RTSP_CLIENT_FRAME_DATA_S*stData);
	void copyAudioData(DP_U32 indexMod, DP_RTSP_CLIENT_FRAME_DATA_S*stData);
	DP_Bool isVideoStartFrame(DP_U32 nextFrameMod);
	DP_Bool isAvailableFrame(DP_U32 nextFrameMod,
			DP_RTSP_CLIENT_STREAM_TYPE_E streamType);
	DP_Bool isUnvalidFrame(DP_U32 nextFrameMod,
			DP_RTSP_CLIENT_STREAM_TYPE_E streamType);
	DP_Bool isAAudioFrame(DP_RTSP_CLIENT_STREAM_TYPE_E streamType);
	DP_Bool isAVideoFrame(DP_RTSP_CLIENT_STREAM_TYPE_E streamType);

};

#endif /* INCLUDE_DP_RTSP_CLIENT_DATAQUEUE_H_ */
