///*
// * openRTSP.h
// *
// *  Created on: Sep 3, 2018
// *      Author: jhb
// */
//
//#ifndef INCLUDE_OPENRTSP_H_
//#define INCLUDE_OPENRTSP_H_
//
//#include "CommonPlay.h"
//
//class openRTSP {
//public:
//	openRTSP();
//	~openRTSP();
//public:
//	RTSPClient* ourRTSPClient;
//
//	Medium* createClient(UsageEnvironment& env, char const* url,
//			CommonPlay *cpObj);
//
//	void assignClient(Medium* client);
//	void getOptions(RTSPClient::responseHandler* afterFunc);
//	void getSDPDescription(RTSPClient::responseHandler* afterFunc);
//	void setupSubsession(MediaSubsession* subsession, Boolean streamUsingTCP,
//			Boolean forceMulticastOnUnspecified,
//			RTSPClient::responseHandler* afterFunc);
//	void startPlayingSession(MediaSession* session, double start, double end,
//			float scale, RTSPClient::responseHandler* afterFunc);
//	void startPlayingSession(MediaSession* session, char const* absStartTime,
//			char const* absEndTime, float scale,
//			RTSPClient::responseHandler* afterFunc);
//	void tearDownSession(MediaSession* session,
//			RTSPClient::responseHandler* afterFunc);
//	void setUserAgentString(char const* userAgentString);
//};
//
//#endif /* INCLUDE_OPENRTSP_H_ */
