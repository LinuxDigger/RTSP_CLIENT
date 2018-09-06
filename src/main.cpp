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
using namespace std;

typedef struct _DEMO_HANDLE_S {
	DP_HANDLE_ID HandleId;
	DP_STREAM_ID A_StreamId;
	DP_STREAM_ID V_StreamId;
	DP_S32 Flage;
} DEMO_HANDLE_S;

DP_S32 main(DP_S32 argc, char**argv) {
	//Debug 调试等级设置
	DP_RTSP_SERVER_SetDebugLevel(DP_RTSP_SERVER_DEBUG_LEVEL_ALL);

	DP_RTSP_SERVER_INIT_INFO_S RtspInitInfo = { 0 };
	RtspInitInfo.s8InputQueueDataMaxConut = 10; //设置数据队列缓存帧数
	RtspInitInfo.s16PollThreadMax = 15; //设置线程轮询个数
	RtspInitInfo.enRtspPortocolVersion = DP_RTSP_SERVER_PORTOCOL_VERSION_1; //设置版本
	RtspInitInfo.u16ServerPort = 10554; //设置Server 运行端口
	RtspInitInfo.s16UrlClientRespMax = 150; //设置每个URL最大响应连接数(单播)
	RtspInitInfo.s16UrlMax = 1; //设置添加最大的Url 唯一标识数
	//Rtsp Server 运行环境初始化
	int ret = 0;
	ret = DP_RTSP_SERVER_Init(RtspInitInfo, NULL, NULL);
	if (ret != DP_RTSP_SERVER_RETVAL_SUCCESS) {
		return 0;
	}

	DEMO_HANDLE_S handle_h265 = { 0 };
	//添加URL唯一标识
	handle_h265.HandleId = DP_RTSP_SERVER_AddMediaName((DP_U8*) "h2644",
			5);
	if (handle_h265.HandleId < DP_RTSP_SERVER_RETVAL_SUCCESS) {
		DP_RTSP_SERVER_UnInit();
		return 0;
	}

	//添加编码参数
	DP_RTSP_SERVER_CODE_PARAM_S CodeParam ;
	CodeParam.enMediaType = DP_RTSP_SERVER_MEDIA_H264;
	CodeParam.unMedia.stMediaVideo.s8VideoFarmeRate = 30;
	handle_h265.V_StreamId = DP_RTSP_SERVER_CodeParamBindMediaName(
			handle_h265.HandleId, CodeParam);

	//初始化SDP 视频config(SPS/VPS/PPS)
	DP_RTSP_SERVER_MEDIA_VIDEO_CONFIG_S SdpInfo;
//	SdpInfo.pu8VideoVPS = MediaStream.pu8FrameStream + 4;
//	SdpInfo.s16VideoVPSsize = 24;
	DP_U8 SPSBUFF[128] = { 0x67, 0x64, 0x00, 0x2a, 0xad, 0x84, 0x01, 0x0c, 0x20,
			0x08, 0x61, 0x00, 0x43, 0x08, 0x02, 0x18, 0x40, 0x10, 0xc2, 0x00,
			0x84, 0x3b, 0x50, 0x3c, 0x01, 0x13, 0xf2, 0xa0 };
	DP_U8 PPSBUFF[128] = { 0x68, 0xee, 0x3c, 0xb0 };
	SdpInfo.pu8VideoSPS = SPSBUFF;
	SdpInfo.s16VideoSPSsize = 28;
	SdpInfo.pu8VideoPPS = PPSBUFF;
	SdpInfo.s16VideoPPSsize = 4;
	DP_RTSP_SERVER_SetMediaSPS_PPS_VPS(handle_h265.HandleId,
			handle_h265.V_StreamId, SdpInfo);
	//开启运行Server
//	DP_RTSP_SERVER_MEDIA_STREAM_INFO_S
	DP_RTSP_SERVER_Start();
	DP_RTSP_CLIENT_Client client;
#if 1
	DP_S32 retCli = client.DP_RTSP_CLIENT_Init(argv[1],
			DP_RTSP_CLIENT_STREAM_VIDEO, DP_RTSP_CLIENT_NET_PROTOCOL_TCP, 30,
			NULL,
			NULL);
	cout << "MAIN!!!!!!!!!!!!!!!!!!!!retCli :::::::: " << retCli << endl;
#endif
#if 0
	DP_S32 retCli2 = client.DP_RTSP_CLIENT_Init(
//			H265_1080P.265
//			test.264
//			Something_Just_Like_This.mp3
			"rtsp://172.16.10.107:8554/test.264", DP_RTSP_CLIENT_STREAM_VIDEO,
			DP_RTSP_CLIENT_NET_PROTOCOL_TCP, 30, NULL,
			NULL);
	cout << "retCli2 :::::::: " << retCli2 << endl;
	DP_S32 retCli3 = client.DP_RTSP_CLIENT_Init(
//			H265_1080P.265
//			test.264
//			Something_Just_Like_This.mp3
			"rtsp://172.16.10.107:8554/test.264", DP_RTSP_CLIENT_STREAM_VIDEO,
			DP_RTSP_CLIENT_NET_PROTOCOL_TCP, 30, NULL,
			NULL);
	cout << "retCli3 :::::::: " << retCli3 << endl;
	DP_S32 retCli4 = client.DP_RTSP_CLIENT_Init(
//			H265_1080P.265
//			test.264
//			Something_Just_Like_This.mp3
			"rtsp://172.16.10.107:8554/test.264", DP_RTSP_CLIENT_STREAM_VIDEO,
			DP_RTSP_CLIENT_NET_PROTOCOL_TCP, 30, NULL,
			NULL);
	cout << "retCli3 :::::::: " << retCli4 << endl;
#endif
	while (1)
		sleep(999);
	return 0;
}
