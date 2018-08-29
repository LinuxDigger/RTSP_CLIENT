/*
 * playCommon.h
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_PLAYCOMMON_H_
#define INCLUDE_PLAYCOMMON_H_

//#include "liveMedia.h"
#include "SIPClient.h"
#include "RTSPClient.h"
#include "MediaSession.h"

extern Medium* createClient(UsageEnvironment& env,char const* URL);
extern void assignClient(Medium* client);
extern RTSPClient* ourRTSPClient;
extern SIPClient* ourSIPClient;

void setEnvURL(UsageEnvironment& env,const char*URL);

void shutdown(int exitCode = 1);
void continueAfterClientCreation1();

extern void getOptions(RTSPClient::responseHandler* afterFunc);

extern void getSDPDescription(RTSPClient::responseHandler* afterFunc);

extern void setupSubsession(MediaSubsession* subsession, Boolean streamUsingTCP,
		Boolean forceMulticastOnUnspecified,
		RTSPClient::responseHandler* afterFunc);

extern void startPlayingSession(MediaSession* session, double start, double end,
		float scale, RTSPClient::responseHandler* afterFunc);

extern void startPlayingSession(MediaSession* session, char const* absStartTime,
		char const* absEndTime, float scale,
		RTSPClient::responseHandler* afterFunc);
// For playing by 'absolute' time (using strings of the form "YYYYMMDDTHHMMSSZ" or "YYYYMMDDTHHMMSS.<frac>Z"

extern void tearDownSession(MediaSession* session,
		RTSPClient::responseHandler* afterFunc);

extern void setUserAgentString(char const* userAgentString);

extern Authenticator* ourAuthenticator;
extern Boolean allowProxyServers;
extern Boolean controlConnectionUsesTCP;
extern Boolean supportCodecSelection;
extern char const* clientProtocolName;
extern unsigned statusCode;

#endif /* INCLUDE_PLAYCOMMON_H_ */
