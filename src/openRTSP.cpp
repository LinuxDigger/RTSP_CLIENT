/*
 * openRTSP.cpp
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#include "playCommon.h"

RTSPClient* ourRTSPClient = NULL;
Medium* createClient(UsageEnvironment& env, char const* url) {
	extern portNumBits tunnelOverHTTPPortNum;
	return ourRTSPClient = RTSPClient::createNew(env, url,
			tunnelOverHTTPPortNum);
}

void assignClient(Medium* client) {
	ourRTSPClient = (RTSPClient*) client;
}

void getOptions(RTSPClient::responseHandler* afterFunc) {
	ourRTSPClient->sendOptionsCommand(afterFunc, ourAuthenticator);
}

void getSDPDescription(RTSPClient::responseHandler* afterFunc) {
	ourRTSPClient->sendDescribeCommand(afterFunc, ourAuthenticator);
}

void setupSubsession(MediaSubsession* subsession, Boolean streamUsingTCP,
		Boolean forceMulticastOnUnspecified,
		RTSPClient::responseHandler* afterFunc) {

	ourRTSPClient->sendSetupCommand(*subsession, afterFunc, False,
			streamUsingTCP, forceMulticastOnUnspecified, ourAuthenticator);
}

void startPlayingSession(MediaSession* session, double start, double end,
		float scale, RTSPClient::responseHandler* afterFunc) {
	ourRTSPClient->sendPlayCommand(*session, afterFunc, start, end, scale,
			ourAuthenticator);
}

void startPlayingSession(MediaSession* session, char const* absStartTime,
		char const* absEndTime, float scale,
		RTSPClient::responseHandler* afterFunc) {
	ourRTSPClient->sendPlayCommand(*session, afterFunc, absStartTime,
			absEndTime, scale, ourAuthenticator);
}

void tearDownSession(MediaSession* session,
		RTSPClient::responseHandler* afterFunc) {
	ourRTSPClient->sendTeardownCommand(*session, afterFunc, ourAuthenticator);
}

void setUserAgentString(char const* userAgentString) {
	ourRTSPClient->setUserAgentString(userAgentString);
}

Boolean allowProxyServers = False;
Boolean controlConnectionUsesTCP = True;
Boolean supportCodecSelection = False;
char const* clientProtocolName = "RTSP";

