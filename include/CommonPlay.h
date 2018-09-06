/*
 * CommonPlay.h
 *
 *  Created on: Aug 31, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_COMMONPLAY_H_
#define INCLUDE_COMMONPLAY_H_
#include "SIPClient.h"
#include "RTSPClient.h"
#include "Media.h"
#include "MediaSession.h"
#include "AVIFileSink.h"
#include "GroupsockHelper.h"
#include "QuickTimeFileSink.h"
#include "MPEG4LATMAudioRTPSource.h"
#include "AMRAudioFileSink.h"
#include "H264VideoFileSink.h"
#include "H265VideoFileSink.h"
#include "OggFileSink.h"
#include "UsageEnvironment.h"
#include "TaskScheduler.h"
#include "DP_RTSP_CLIENT_Interface.h"
#include "QuickTimeFileSink.h"
#include "DigestAuthentication.h"

typedef void TaskFunc(void* clientData, CommonPlay *cpObj);
typedef void* TaskToken;
//class AVIFileSink;
//class UsageEnvironment;
class CommonPlay {
public:

	CommonPlay(unsigned short cliID);
	~CommonPlay();

	// Forward function definitions:
	void continueAfterClientCreation0(RTSPClient* client,
			Boolean requestStreamingOverTCP);
	void continueAfterClientCreation1();
	static void continueAfterOPTIONS(RTSPClient* client, int resultCode,
			char* resultString, CommonPlay *cpObj);
	static void continueAfterDESCRIBE(RTSPClient* client, int resultCode,
			char* resultString, CommonPlay *cpObj);
	void setEnvURL(UsageEnvironment& env, const char*URL);
	static void continueAfterSETUP(RTSPClient* client, int resultCode,
			char* resultString, CommonPlay *cpObj);
	static void continueAfterPLAY(RTSPClient* client, int resultCode,
			char* resultString, CommonPlay *cpObj);
	static void continueAfterTEARDOWN(RTSPClient* client, int resultCode,
			char* resultString, CommonPlay *cpObj);

	void createOutputFiles(char const* periodicFilenameSuffix);
	void createPeriodicOutputFiles();
	void setupStreams();
	void closeMediaSinks();
	static void subsessionAfterPlaying(void* clientData, CommonPlay *cpObj);
	static void subsessionByeHandler(void* clientData, CommonPlay *cpObj);
	static void sessionAfterPlaying(void* clientData = NULL, CommonPlay *cpObj =
	NULL);
	static void sessionTimerHandler(void* clientData, CommonPlay *cpObj);
	static void periodicFileOutputTimerHandler(void* clientData,
			CommonPlay *cpObj);
	//void shutdown(int exitCode = 1);
	void signalHandlerShutdown(int sig);
	static void checkForPacketArrival(void* clientData, CommonPlay *cpObj);
	static void checkInterPacketGaps(void* clientData, CommonPlay *cpObj);
	static void checkSessionTimeoutBrokenServer(void* clientData,
			CommonPlay *cpObj);
	void beginQOSMeasurement();
	void printQOSData(int exitCode);

	void shutdown(int exitCode = 1);

	Medium* createClient(UsageEnvironment& env, char const* url,
			CommonPlay *cpObj) {
		return ourRTSPClient = RTSPClient::createNew(env, url, cpObj, -1, NULL,
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
		ourRTSPClient->sendTeardownCommand(*session, afterFunc,
				ourAuthenticator);
	}

	void setUserAgentString(char const* userAgentString) {
		ourRTSPClient->setUserAgentString(userAgentString);
	}

	static void scheduleNextQOSMeasurement(CommonPlay *cpObj);
public:

	char const* progName;
	UsageEnvironment* env;
	Medium* ourClient;
	Authenticator* ourAuthenticator;
	char const* streamURL;
	MediaSession* session;
	TaskToken sessionTimerTask;
	TaskToken sessionTimeoutBrokenServerTask;
	TaskToken arrivalCheckTimerTask;
	TaskToken interPacketGapCheckTimerTask;
	TaskToken qosMeasurementTimerTask;
	TaskToken periodicFileOutputTask;
	Boolean createReceivers;
	Boolean outputQuickTimeFile;
	Boolean generateMP4Format;
	QuickTimeFileSink* qtOut;
	Boolean outputAVIFile;
	AVIFileSink* aviOut;
	Boolean audioOnly;
	Boolean videoOnly;
	char const* singleMedium;
	int verbosityLevel; // by default, print verbose output
	double duration;
	double durationSlop; // extra seconds to play at the end
	double initialSeekTime;
	char* initialAbsoluteSeekTime;
	char* initialAbsoluteSeekEndTime;
	float scale;
	double endTime;
	unsigned interPacketGapMaxTime;
	unsigned totNumPacketsReceived; // used if checking inter-packet gaps
	Boolean playContinuously;
	int simpleRTPoffsetArg;
	Boolean sendOptionsRequest;
	Boolean sendOptionsRequestOnly;
	Boolean oneFilePerFrame;
	Boolean notifyOnPacketArrival;
	Boolean sendKeepAlivesToBrokenServers;
	unsigned sessionTimeoutParameter;
	Boolean streamUsingTCP;
	Boolean forceMulticastOnUnspecified;

	unsigned short desiredPortNum;
	portNumBits tunnelOverHTTPPortNum;
	char* username;
	char* password;
	char* proxyServerName;
	unsigned short proxyServerPortNum;
	DP_U8 desiredAudioRTPPayloadFormat;
	char* mimeSubtype;
	unsigned short movieWidth; // default
	Boolean movieWidthOptionSet;
	unsigned short movieHeight; // default
	Boolean movieHeightOptionSet;
	unsigned movieFPS; // default
	Boolean movieFPSOptionSet;
	char const* fileNamePrefix;
	unsigned fileSinkBufferSize; //////////////////////////////////100000is origin
	unsigned socketInputBufferSize;
	Boolean packetLossCompensate;
	Boolean syncStreams;
	Boolean generateHintTracks;
	Boolean waitForResponseToTEARDOWN;
	unsigned qosMeasurementIntervalMS; // 0 means: Don't output QOS data
	char* userAgent;
	unsigned fileOutputInterval; // seconds
	unsigned fileOutputSecondsSoFar; // seconds
	Boolean createHandlerServerForREGISTERCommand;
	portNumBits handlerServerForREGISTERCommandPortNum;
	HandlerServerForREGISTERCommand* handlerServerForREGISTERCommand;
	char* usernameForREGISTER;
	char* passwordForREGISTER;
	UserAuthenticationDatabase* authDBForREGISTER;

	struct timeval startTime;

//openRTSP
	RTSPClient* ourRTSPClient;
	Boolean allowProxyServers;
	Boolean controlConnectionUsesTCP;
	Boolean supportCodecSelection;
	char const* clientProtocolName;

//playCommon.cpp
	Boolean areAlreadyShuttingDown;
	int shutdownExitCode;

	//extern
	SIPClient* ourSIPClient;

	unsigned statusCode;

	static map<unsigned short, MediaSubsessionIterator*> mSetupIter;

};

/////
//extern Medium* createClient(UsageEnvironment& env, char const* URL);
//extern void assignClient(Medium* client);
//
//void shutdown(int exitCode = 1);
//void continueAfterClientCreation1();
//
//extern void getOptions(RTSPClient::responseHandler* afterFunc);
//
//extern void getSDPDescription(RTSPClient::responseHandler* afterFunc);
//
//extern void setupSubsession(MediaSubsession* subsession, Boolean streamUsingTCP,
//		Boolean forceMulticastOnUnspecified,
//		RTSPClient::responseHandler* afterFunc);
//
//extern void startPlayingSession(MediaSession* session, double start, double end,
//		float scale, RTSPClient::responseHandler* afterFunc);
//
//extern void startPlayingSession(MediaSession* session, char const* absStartTime,
//		char const* absEndTime, float scale,
//		RTSPClient::responseHandler* afterFunc);
//// For playing by 'absolute' time (using strings of the form "YYYYMMDDTHHMMSSZ" or "YYYYMMDDTHHMMSS.<frac>Z"
//
//extern void tearDownSession(MediaSession* session,
//		RTSPClient::responseHandler* afterFunc);
//
//extern void setUserAgentString(char const* userAgentString);

#endif /* INCLUDE_COMMONPLAY_H_ */
