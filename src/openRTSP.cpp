///*
// * openRTSP.cpp
// *
// *  Created on: Aug 9, 2018
// *      Author: jhb
// */
//
////#include "playCommon.h"
//#include "openRTSP.h"
//
//openRTSP::openRTSP() :
//		ourRTSPClient(NULL) {
//}
//openRTSP::~openRTSP() {
//
//}
//
////Medium* openRTSP::createClient(UsageEnvironment& env, char const* url,
////		CommonPlay *cpObj) {
////	portNumBits tunnelOverHTTPPortNum;
////	return ourRTSPClient = RTSPClient::createNew(env, url, cpObj,
////			tunnelOverHTTPPortNum);
////}
//
//void openRTSP::assignClient(Medium* client) {
//	ourRTSPClient = (RTSPClient*) client;
//}
//#if 0
//void openRTSP::getOptions(RTSPClient::responseHandler* afterFunc) {
//	ourRTSPClient->sendOptionsCommand(afterFunc, ourAuthenticator);
//}
//
//void openRTSP::getSDPDescription(RTSPClient::responseHandler* afterFunc) {
//	ourRTSPClient->sendDescribeCommand(afterFunc, ourAuthenticator);
//}
//
//void openRTSP::setupSubsession(MediaSubsession* subsession,
//		Boolean streamUsingTCP, Boolean forceMulticastOnUnspecified,
//		RTSPClient::responseHandler* afterFunc) {
//
//	ourRTSPClient->sendSetupCommand(*subsession, afterFunc, False,
//			streamUsingTCP, forceMulticastOnUnspecified, ourAuthenticator);
//}
//
//void openRTSP::startPlayingSession(MediaSession* session, double start,
//		double end, float scale, RTSPClient::responseHandler* afterFunc) {
//	ourRTSPClient->sendPlayCommand(*session, afterFunc, start, end, scale,
//			ourAuthenticator);
//}
//
//void openRTSP::startPlayingSession(MediaSession* session,
//		char const* absStartTime, char const* absEndTime, float scale,
//		RTSPClient::responseHandler* afterFunc) {
//	ourRTSPClient->sendPlayCommand(*session, afterFunc, absStartTime,
//			absEndTime, scale, ourAuthenticator);
//}
//
//void openRTSP::tearDownSession(MediaSession* session,
//		RTSPClient::responseHandler* afterFunc) {
//	ourRTSPClient->sendTeardownCommand(*session, afterFunc, ourAuthenticator);
//}
//
//void openRTSP::setUserAgentString(char const* userAgentString) {
//	ourRTSPClient->setUserAgentString(userAgentString);
//}
//#endif
////Boolean allowProxyServers = False;
////Boolean controlConnectionUsesTCP = True;
////Boolean supportCodecSelection = False;
////char const* clientProtocolName = "RTSP";
//
