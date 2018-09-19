/*
 * main.cpp
 *
 *  Created on: Aug 14, 2018
 *      Author: jhb
 */

//#include "DP_RTSP_CLIENT_GlobDefine.h"
#include "DP_RTSP_CLIENT_Interface.h"
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
//server
#include "Rtsp_server.h"
#include "Logger.h"
using namespace FrameWork;
using namespace std;

typedef struct _DEMO_HANDLE_S {
	DP_HANDLE_ID HandleId;
	DP_STREAM_ID A_StreamId;
	DP_STREAM_ID V_StreamId;
	DP_S32 Flage;
} DEMO_HANDLE_S;

DP_S32 main(DP_S32 argc, char**argv) {

	InitLogging(argv[0], WARN, ".");
	//Debug 调试等级设置
	DP_RTSP_SERVER_SetDebugLevel(DP_RTSP_SERVER_DEBUG_LEVEL_ALL);

	DP_RTSP_SERVER_INIT_INFO_S RtspInitInfo = { 0 };
	RtspInitInfo.s8InputQueueDataMaxConut = 10; //设置数据队列缓存帧数
	RtspInitInfo.s16PollThreadMax = 15; //设置线程轮询个数
	RtspInitInfo.enRtspPortocolVersion = DP_RTSP_SERVER_PORTOCOL_VERSION_1; //设置版本
	RtspInitInfo.u16ServerPort = 10554; //设置Server 运行端口
	RtspInitInfo.s16UrlClientRespMax = 150; //设置每个URL最大响应连接数(单播)
	RtspInitInfo.s16UrlMax = 2; //设置添加最大的Url 唯一标识数
	//Rtsp Server 运行环境初始化
	int ret = 0;
	ret = DP_RTSP_SERVER_Init(RtspInitInfo, NULL, NULL);
	if (ret != DP_RTSP_SERVER_RETVAL_SUCCESS) {
		return 0;
	}

	DEMO_HANDLE_S handle_h265 = { 0 };
	DEMO_HANDLE_S handle_h2644_2 = { 0 };
	//添加URL唯一标识
	handle_h265.HandleId = DP_RTSP_SERVER_AddMediaName((DP_U8*) "h2644", 5);
	handle_h2644_2.HandleId = DP_RTSP_SERVER_AddMediaName((DP_U8*) "h2644_2",
			7);
	if (handle_h265.HandleId < DP_RTSP_SERVER_RETVAL_SUCCESS) {
		DP_RTSP_SERVER_UnInit();
		return 0;
	}
	if (handle_h2644_2.HandleId < DP_RTSP_SERVER_RETVAL_SUCCESS) {
		DP_RTSP_SERVER_UnInit();
		return 0;
	}

	//添加编码参数
	DP_RTSP_SERVER_CODE_PARAM_S CodeParam;
//	DP_RTSP_SERVER_MEDIA_H265
	CodeParam.enMediaType = DP_RTSP_SERVER_MEDIA_H264;
	//30 h264
	CodeParam.unMedia.stMediaVideo.s8VideoFarmeRate = 30;
	handle_h265.V_StreamId = DP_RTSP_SERVER_CodeParamBindMediaName(
			handle_h265.HandleId, CodeParam);
	handle_h2644_2.V_StreamId = DP_RTSP_SERVER_CodeParamBindMediaName(
			handle_h2644_2.HandleId, CodeParam);

	//初始化SDP 视频config(SPS/VPS/PPS)
	DP_RTSP_SERVER_MEDIA_VIDEO_CONFIG_S SdpInfo;
//	SdpInfo.pu8VideoVPS = MediaStream.pu8FrameStream + 4;
//	SdpInfo.s16VideoVPSsize = 24;
//	DP_U8 SPSBUFF[128] = { 0x67, 0x64, 0x00, 0x2a, 0xad, 0x84, 0x01, 0x0c, 0x20,
//			0x08, 0x61, 0x00, 0x43, 0x08, 0x02, 0x18, 0x40, 0x10, 0xc2, 0x00,
//			0x84, 0x3b, 0x50, 0x3c, 0x01, 0x13, 0xf2, 0xa0 };
//	DP_U8 PPSBUFF[128] = { 0x68, 0xee, 0x3c, 0xb0 };
//	SdpInfo.pu8VideoSPS = SPSBUFF;
//	SdpInfo.s16VideoSPSsize = 28;
//	SdpInfo.pu8VideoPPS = PPSBUFF;
//	SdpInfo.s16VideoPPSsize = 4;
//	DP_RTSP_SERVER_SetMediaSPS_PPS_VPS(handle_h265.HandleId,
//			handle_h265.V_StreamId, SdpInfo);
//	DP_RTSP_SERVER_SetMediaSPS_PPS_VPS(handle_h2644_2.HandleId,
//			handle_h2644_2.V_StreamId, SdpInfo);
	//开启运行Server
	DP_RTSP_SERVER_Start();
	DP_RTSP_CLIENT_Client client;
	client.DP_RTSP_CLIENT_GlobalSetting(64, 5);
#if 1
	DP_S32 retCli = client.DP_RTSP_CLIENT_Init(argv[1],
			DP_RTSP_CLIENT_STREAM_VIDEO, DP_RTSP_CLIENT_NET_PROTOCOL_TCP, 30,
			argv[2], argv[3]);
	Logger::GetInstance().Warn("MAIN!!!!!!!!!!!!!!!!!!!!retCli :::::::: %d ",
			retCli);
	DP_RTSP_CLIENT_FRAME_DATA_S stGetData(retCli);
	DP_RTSP_SERVER_MEDIA_STREAM_INFO_S MediaStream;
	memset(&MediaStream, 0, sizeof(DP_RTSP_SERVER_MEDIA_STREAM_INFO_S));
	MediaStream.enFrameType = DP_RTSP_SERVER_H264_FRAME_I;
	MediaStream.u32TimeStamp = 0;
	FrameDataMemManage manage;
	DP_Bool firstPutout = true;
	Logger::GetInstance().Debug("firstPutout init : ", firstPutout);

#endif
#if 0
	for (int i = 0; i < 9; i++)
	client.DP_RTSP_CLIENT_Init(argv[1], DP_RTSP_CLIENT_STREAM_VIDEO,
			DP_RTSP_CLIENT_NET_PROTOCOL_TCP, 30, argv[2], argv[3]);

#endif
#if 0
//	rtsp://172.16.10.124:554/h264_2
	DP_S32 retCli2 = client.DP_RTSP_CLIENT_Init(
//			H265_1080P.265
//			test.264
//			Something_Just_Like_This.mp3

			argv[1], DP_RTSP_CLIENT_STREAM_VIDEO,
			DP_RTSP_CLIENT_NET_PROTOCOL_TCP, 30, argv[2], argv[3]);

	cout << "retCli2 :::::::: " << retCli2 << endl;
//	DP_RTSP_CLIENT_FRAME_DATA_S stGetData_2(retCli2);
	DP_S32 retCli3 = client.DP_RTSP_CLIENT_Init(
//			H265_1080P.265
//			test.264
//			Something_Just_Like_This.mp3
//			"rtsp://172.16.10.124:554/h264_2"
			argv[1], DP_RTSP_CLIENT_STREAM_VIDEO,
			DP_RTSP_CLIENT_NET_PROTOCOL_TCP, 30, argv[2], argv[3]);
	cout << "retCli3 :::::::: " << retCli3 << endl;
	DP_S32 retCli4 = client.DP_RTSP_CLIENT_Init(
//			H265_1080P.265
//			test.264
//			Something_Just_Like_This.mp3
			argv[1], DP_RTSP_CLIENT_STREAM_VIDEO,
			DP_RTSP_CLIENT_NET_PROTOCOL_TCP, 30, argv[2], argv[3]);
	cout << "retCli4 :::::::: " << retCli4 << endl;
	DP_S32 retCli5 = client.DP_RTSP_CLIENT_Init(
//			H265_1080P.265
//			test.264
//			Something_Just_Like_This.mp3
			argv[1], DP_RTSP_CLIENT_STREAM_VIDEO,
			DP_RTSP_CLIENT_NET_PROTOCOL_TCP, 30, argv[2], argv[3]);
	cout << "retCli5 :::::::: " << retCli5 << endl;
#endif
	DP_RTSP_SERVER_MEDIA_STREAM_INFO_S MediaStream_2;
	memset(&MediaStream_2, 0, sizeof(DP_RTSP_SERVER_MEDIA_STREAM_INFO_S));
	MediaStream_2.enFrameType = DP_RTSP_SERVER_H264_FRAME_I;
	MediaStream_2.u32TimeStamp = 0;
	FrameDataMemManage manage_2;
//	stGetData.u32FrameSequence = 0xffffffff;
	DP_Bool firstPutout_2 = true;
	usleep(500000);
	while (1) {
//	//		sleep(100);
		DP_S32 ret = client.DP_RTSP_CLIENT_GetStreamData(&stGetData, 200,
				manage, firstPutout);
		MediaStream.s32FrameSize = stGetData.u32FrameSize;
		MediaStream.pu8FrameStream = stGetData.pu8Data;
		Logger::GetInstance().Info(
				"<main() <<<<<<<<<<<<<<<<<<<<<<<<<<<< %d ret:: %d stGetData.frame seq :: %d ",
				stGetData.u32FrameSize, ret, stGetData.u32FrameSequence);
		DP_RTSP_SERVER_MediaStreamInput(0, 0, MediaStream);
#if 0
		////1111111
		cout << "main() stgetdata frameseq :: " << stGetData_2.u32FrameSequence
		<< endl;
		DP_S32 rett = client.DP_RTSP_CLIENT_GetStreamData(&stGetData_2, 200,
				manage_2, firstPutout_2);
		MediaStream.s32FrameSize = stGetData_2.u32FrameSize;
		MediaStream.pu8FrameStream = stGetData_2.pu8Data;
		cout
		<< "<main <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< "
		<< stGetData_2.u32FrameSize << "ret:: " << rett
		<< " firstPutout:: " << firstPutout_2
		<< " stGetData.frame seq :: " << stGetData_2.u32FrameSequence
		<< endl;
		DP_RTSP_SERVER_MediaStreamInput(1, 0, MediaStream_2);
#endif
	}

	while (1)
		sleep(999);
	return 0;
}
