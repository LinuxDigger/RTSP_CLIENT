/*
 * main.cpp
 *
 *  Created on: Aug 14, 2018
 *      Author: jhb
 */

#include "DP_RTSP_CLIENT_GlobDefine.h"
#include "DP_RTSP_CLIENT_Interface.h"
#include <iostream>
#include <unistd.h>
using namespace std;

DP_S32 main(DP_S32 argc, DP_S8**argv) {
	DP_RTSP_CLIENT_Client client;
	DP_S32 retCli = client.DP_RTSP_CLIENT_Init(argv[1], DP_RTSP_CLIENT_STREAM_VIDEO,
			DP_RTSP_CLIENT_NET_PROTOCOL_TCP, 30, NULL,
			NULL);
	cout << "MAIN!!!!!!!!!!!!!!!!!!!!retCli :::::::: "<<retCli<<endl;
//	DP_S32 retCli2 = client.DP_RTSP_CLIENT_Init("rtsp://172.16.10.107:8554/test.264", DP_RTSP_CLIENT_STREAM_VIDEO,
//				DP_RTSP_CLIENT_NET_PROTOCOL_TCP, 30, NULL,
//				NULL);
//	cout << "retCli2 :::::::: "<<retCli<<endl;
	sleep(100);
	return 0;
}
