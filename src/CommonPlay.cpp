/*
 * CommonPlay.cpp
 *
 *  Created on: Aug 31, 2018
 *      Author: jhb
 */

#include <string.h>
#include <iostream>
#include "CommonPlay.h"
#include "FileSink.h"
#include "Logger.h"
using namespace FrameWork;
using namespace std;

map<unsigned short, MediaSubsessionIterator *> CommonPlay::mSetupIter;

CommonPlay::CommonPlay(unsigned short cliID) :
		progName(NULL), env(NULL), ourClient(NULL), ourAuthenticator(NULL), streamURL(
		NULL), session(NULL), sessionTimerTask(NULL), sessionTimeoutBrokenServerTask(
		NULL), arrivalCheckTimerTask(NULL), interPacketGapCheckTimerTask(
		NULL), qosMeasurementTimerTask(NULL), periodicFileOutputTask(
		NULL), createReceivers(True), outputQuickTimeFile(False), generateMP4Format(
				False), qtOut(NULL), outputAVIFile(False), aviOut(NULL), audioOnly(
				False), videoOnly(False), singleMedium(NULL), verbosityLevel(1), duration(
				0), durationSlop(-1.0), initialSeekTime(0.0f), initialAbsoluteSeekTime(
		NULL), initialAbsoluteSeekEndTime(NULL), scale(1.0f), endTime(), interPacketGapMaxTime(
				0), totNumPacketsReceived(~0), playContinuously(False), simpleRTPoffsetArg(
				-1), sendOptionsRequest(True), sendOptionsRequestOnly(False), oneFilePerFrame(
				False), notifyOnPacketArrival(False), sendKeepAlivesToBrokenServers(
				False), sessionTimeoutParameter(0), streamUsingTCP(False), forceMulticastOnUnspecified(
				False), desiredPortNum(0), tunnelOverHTTPPortNum(0), username(
		NULL), password(NULL), proxyServerName(NULL), proxyServerPortNum(0), desiredAudioRTPPayloadFormat(
				0), mimeSubtype(NULL), movieWidth(240), movieWidthOptionSet(
				False), movieHeight(180), movieHeightOptionSet(False), movieFPS(
				15), movieFPSOptionSet(False), fileNamePrefix(""), fileSinkBufferSize(
				1000000), socketInputBufferSize(0), packetLossCompensate(False), syncStreams(
				False), generateHintTracks(False), waitForResponseToTEARDOWN(
				True), qosMeasurementIntervalMS(0), userAgent(NULL), fileOutputInterval(
				0), fileOutputSecondsSoFar(0), createHandlerServerForREGISTERCommand(
				False), handlerServerForREGISTERCommandPortNum(0), handlerServerForREGISTERCommand(), usernameForREGISTER(
		NULL), passwordForREGISTER(NULL), authDBForREGISTER(NULL), startTime(), ourRTSPClient(
		NULL), allowProxyServers(False), controlConnectionUsesTCP(True), supportCodecSelection(
				False), clientProtocolName("RTSP"), areAlreadyShuttingDown(
				False), shutdownExitCode(), ourSIPClient(), statusCode(), _fClientID(
				cliID), subsession(NULL), madeProgress(False) {
	MediaSubsessionIterator *setupIter = NULL;
	efficientAddOrUpdate(mSetupIter, cliID, setupIter);
}

CommonPlay::CommonPlay() :
		progName(NULL), env(NULL), ourClient(NULL), ourAuthenticator(NULL), streamURL(
		NULL), session(NULL), sessionTimerTask(NULL), sessionTimeoutBrokenServerTask(
		NULL), arrivalCheckTimerTask(NULL), interPacketGapCheckTimerTask(
		NULL), qosMeasurementTimerTask(NULL), periodicFileOutputTask(
		NULL), createReceivers(True), outputQuickTimeFile(False), generateMP4Format(
				False), qtOut(NULL), outputAVIFile(False), aviOut(NULL), audioOnly(
				False), videoOnly(False), singleMedium(NULL), verbosityLevel(1), duration(
				0), durationSlop(-1.0), initialSeekTime(0.0f), initialAbsoluteSeekTime(
		NULL), initialAbsoluteSeekEndTime(NULL), scale(1.0f), endTime(), interPacketGapMaxTime(
				0), totNumPacketsReceived(~0), playContinuously(False), simpleRTPoffsetArg(
				-1), sendOptionsRequest(True), sendOptionsRequestOnly(False), oneFilePerFrame(
				False), notifyOnPacketArrival(False), sendKeepAlivesToBrokenServers(
				False), sessionTimeoutParameter(0), streamUsingTCP(False), forceMulticastOnUnspecified(
				False), desiredPortNum(0), tunnelOverHTTPPortNum(0), username(
		NULL), password(NULL), proxyServerName(NULL), proxyServerPortNum(0), desiredAudioRTPPayloadFormat(
				0), mimeSubtype(NULL), movieWidth(240), movieWidthOptionSet(
				False), movieHeight(180), movieHeightOptionSet(False), movieFPS(
				15), movieFPSOptionSet(False), fileNamePrefix(""), fileSinkBufferSize(
				1000000), socketInputBufferSize(0), packetLossCompensate(False), syncStreams(
				False), generateHintTracks(False), waitForResponseToTEARDOWN(
				True), qosMeasurementIntervalMS(0), userAgent(NULL), fileOutputInterval(
				0), fileOutputSecondsSoFar(0), createHandlerServerForREGISTERCommand(
				False), handlerServerForREGISTERCommandPortNum(0), handlerServerForREGISTERCommand(), usernameForREGISTER(
		NULL), passwordForREGISTER(NULL), authDBForREGISTER(NULL), startTime(), ourRTSPClient(
		NULL), allowProxyServers(False), controlConnectionUsesTCP(True), supportCodecSelection(
				False), clientProtocolName("RTSP"), areAlreadyShuttingDown(
				False), shutdownExitCode(), ourSIPClient(), statusCode(), _fClientID(
				0), subsession(NULL), madeProgress(False) {
}

CommonPlay::~CommonPlay() {
	if (ourAuthenticator != NULL) {
		delete ourAuthenticator;
		ourAuthenticator = NULL;
	}
}

void CommonPlay::continueAfterClientCreation1() {
	setUserAgentString(userAgent);

	if (sendOptionsRequest) {
		// Begin by sending an "OPTIONS" command:
		//this way
		getOptions(continueAfterOPTIONS);
	} else {
		continueAfterOPTIONS(NULL, 0, NULL, this);
	}
}

void CommonPlay::continueAfterOPTIONS(RTSPClient *, int resultCode,
		char *resultString, CommonPlay *cpObj) {
	Logger::GetInstance().Debug(
			"after options -----------------------------------");
	if (cpObj->sendOptionsRequestOnly) {
		if (resultCode != 0) {
			*cpObj->env << cpObj->clientProtocolName
					<< " \"OPTIONS\" request failed: " << resultString << "\n";
		} else {
			*cpObj->env << cpObj->clientProtocolName
					<< " \"OPTIONS\" request returned: " << resultString
					<< "\n";
		}
		cpObj->shutdown();
	}
	delete[] resultString;
	cout << "cpObj->ourAuthenticator->password(). "
			<< cpObj->ourAuthenticator->password() << endl;
	// Next, get a SDP description for the stream:
	cpObj->getSDPDescription(continueAfterDESCRIBE);
}

void CommonPlay::setEnvURL(UsageEnvironment &envv, const char *URL) {
	env = &envv;
	streamURL = URL;
}

void CommonPlay::continueAfterDESCRIBE(RTSPClient *, int resultCode,
		char *resultString, CommonPlay *cpObj) {
	Logger::GetInstance().Debug(
			"after describe =======================================================================");
	if (resultCode != 0) {
		*cpObj->env << "Failed to get a SDP description for the URL \""
				<< cpObj->streamURL << "\": " << resultString << "\n";
		cout << "cp ..........." << cpObj->_fClientID
				<< cpObj->ourAuthenticator->password() << endl;
		delete[] resultString;
		cpObj->shutdown();
	}

	char *sdpDescription = resultString;
	Logger::GetInstance().Info(
			"Opened URL \" %s \", returning a SDP description:  %s",
			cpObj->streamURL, sdpDescription);

	// Create a media session object from this SDP description:
	//	sleep(10);
	cpObj->session = MediaSession::createNew(*cpObj->env, sdpDescription,
			cpObj);
	delete[] sdpDescription;
	if (cpObj->session == NULL) {
		*cpObj->env
				<< "Failed to create a MediaSession object from the SDP description: "
				<< cpObj->env->getResultMsg() << "\n";
		cpObj->shutdown();
	} else if (!cpObj->session->hasSubsessions()) {
		*cpObj->env
				<< "This session has no media subsessions (i.e., no \"m=\" lines)\n";
		cpObj->shutdown();
	}

	// Then, setup the "RTPSource"s for the session:
	MediaSubsessionIterator iter(*cpObj->session);
	MediaSubsession *subsession;
	Boolean madeProgress = False;
	char const *singleMediumToTest = cpObj->singleMedium;
	while ((subsession = iter.next()) != NULL) {
		// If we've asked to receive only a single medium, then check this now:
		if (singleMediumToTest != NULL) {
			if (strcmp(subsession->mediumName(), singleMediumToTest) != 0) {
				*cpObj->env << "Ignoring \"" << subsession->mediumName() << "/"
						<< subsession->codecName()
						<< "\" subsession, because we've asked to receive a single "
						<< cpObj->singleMedium << " session only\n";
				continue;
			} else {
				// Receive this subsession only
				singleMediumToTest = "xxxxx";
				// this hack ensures that we get only 1 subsession of this type
			}
		}

		if (cpObj->desiredPortNum != 0) {
			subsession->setClientPortNum(cpObj->desiredPortNum);
			cpObj->desiredPortNum += 2;
		}

		if (cpObj->createReceivers) {
			if (!subsession->initiate(cpObj->simpleRTPoffsetArg)) {
				*cpObj->env << "Unable to create receiver for \""
						<< subsession->mediumName() << "/"
						<< subsession->codecName() << "\" subsession: "
						<< cpObj->env->getResultMsg() << "\n";
			} else {
				*cpObj->env << "Created receiver for \""
						<< subsession->mediumName()						// audio
						<< "/" << subsession->codecName() << "\" subsession ("; // MPA
				if (subsession->rtcpIsMuxed()) {
					*cpObj->env << "client port "
							<< subsession->clientPortNum();
				} else {
					*cpObj->env << "client ports "
							<< subsession->clientPortNum() << "-"
							<< subsession->clientPortNum() + 1;
				}
				*cpObj->env << ")\n";

				//				cout << "videoFPSaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
				//						<< subsession->videoFPS();  //0
				//				cout << "videoWidthaaaaaaaaaaaaaaaaaaaaaaa"
				//						<< subsession->videoWidth() << " height "
				//						//0
				//						<< subsession->videoHeight() << " codecName "
				//						<< subsession->codecName();

				madeProgress = True;

				if (subsession->rtpSource() != NULL) {
					// Because we're saving the incoming data, rather than playing
					// it in real time, allow an especially large time threshold
					// (1 second) for reordering misordered incoming packets:
					unsigned const thresh = 1000000; // 1 second
					subsession->rtpSource()->setPacketReorderingThresholdTime(
							thresh);

					// Set the RTP source's OS socket buffer size as appropriate - either if we were explicitly asked (using -B),
					// or if the desired FileSink buffer size happens to be larger than the current OS socket buffer size.
					// (The latter case is a heuristic, on the assumption that if the user asked for a large FileSink buffer size,
					// then the input data rate may be large enough to justify increasing the OS socket buffer size also.)
					int socketNum =
							subsession->rtpSource()->RTPgs()->socketNum();
					unsigned curBufferSize = getReceiveBufferSize(*cpObj->env,
							socketNum);
					if (cpObj->socketInputBufferSize > 0
							|| cpObj->fileSinkBufferSize > curBufferSize) {
						unsigned newBufferSize =
								cpObj->socketInputBufferSize > 0 ?
										cpObj->socketInputBufferSize :
										cpObj->fileSinkBufferSize;
						newBufferSize = setReceiveBufferTo(*cpObj->env,
								socketNum, newBufferSize);
						if (cpObj->socketInputBufferSize > 0) { // The user explicitly asked for the new socket buffer size; announce it:
							*cpObj->env
									<< "Changed socket receive buffer size for the \""
									<< subsession->mediumName() << "/"
									<< subsession->codecName()
									<< "\" subsession from " << curBufferSize
									<< " to " << newBufferSize << " bytes\n";
						}
					}
				}
			}
		} else {
			if (subsession->clientPortNum() == 0) {
				*cpObj->env << "No client port was specified for the \""
						<< subsession->mediumName() << "/"
						<< subsession->codecName()
						<< "\" subsession.  (Try adding the \"-p <portNum>\" option.)\n";
			} else {
				madeProgress = True;
			}
		}
	}
	if (!madeProgress)
		cpObj->shutdown();

	// Perform additional 'setup' on each subsession, before playing them:
	cpObj->setupStreams();
}

//MediaSubsession *subsession;
//Boolean madeProgress = False;
void CommonPlay::continueAfterSETUP(RTSPClient *client, int resultCode,
		char *resultString, CommonPlay *cpObj) {
	Logger::GetInstance().Debug(
			"aftersetup =======================================================================");
	if (resultCode == 0) {
		cout
				<< "seg fault monPlay::continueAfterSETUP(RTSPClient *client, int resul ,,,,,,,,,,,,,,,,"
				<< endl;
		*cpObj->env << "Setup \"" << cpObj->subsession->mediumName() << "/"
				<< cpObj->subsession->codecName() << "\" subsession (";
		if (cpObj->subsession->rtcpIsMuxed()) {
			Logger::GetInstance().Debug("client port %d",
					cpObj->subsession->clientPortNum());
		} else {
			*cpObj->env << "client ports " << cpObj->subsession->clientPortNum()
					<< "-" << cpObj->subsession->clientPortNum() + 1;
		}
		*cpObj->env << ")\n";
		cpObj->madeProgress = True;
	} else {
		*cpObj->env << "Failed to setup \"" << cpObj->subsession->mediumName()
				<< "/" << cpObj->subsession->codecName() << "\" subsession: "
				<< resultString << "\n";
	}
	delete[] resultString;

	if (client != NULL)
		cpObj->sessionTimeoutParameter = client->sessionTimeoutParameter();

	// Set up the next subsession, if any:
	cpObj->setupStreams();
}

void CommonPlay::createOutputFiles(char const *periodicFilenameSuffix) {
	char outFileName[1000];

	if (outputQuickTimeFile || outputAVIFile) {
		if (periodicFilenameSuffix[0] == '\0') {
			// Normally (unless the '-P <interval-in-seconds>' option was given) we output to 'stdout':
			sprintf(outFileName, "stdout");
		} else {
			// Otherwise output to a type-specific file name, containing "periodicFilenameSuffix":
			char const *prefix =
					fileNamePrefix[0] == '\0' ? "output" : fileNamePrefix;
			snprintf(outFileName, sizeof outFileName, "%s%s.%s", prefix,
					periodicFilenameSuffix,
					outputAVIFile ? "avi" : generateMP4Format ? "mp4" : "mov");
		}

		if (outputQuickTimeFile) {
			qtOut = QuickTimeFileSink::createNew(*env, *session, outFileName,
					fileSinkBufferSize, movieWidth, movieHeight, movieFPS,
					packetLossCompensate, syncStreams, generateHintTracks,
					generateMP4Format);
			if (qtOut == NULL) {
				*env
						<< "Failed to create a \"QuickTimeFileSink\" for outputting to \""
						<< outFileName << "\": " << env->getResultMsg() << "\n";
				shutdown();
			} else {
				*env << "Outputting to the file: \"" << outFileName << "\"\n";
			}

			qtOut->startPlaying(sessionAfterPlaying, NULL);
		} else { // outputAVIFile
			aviOut = AVIFileSink::createNew(*env, *session, outFileName,
					fileSinkBufferSize, movieWidth, movieHeight, movieFPS,
					packetLossCompensate, this);
			if (aviOut == NULL) {
				*env
						<< "Failed to create an \"AVIFileSink\" for outputting to \""
						<< outFileName << "\": " << env->getResultMsg() << "\n";
				shutdown();
			} else {
				*env << "Outputting to the file: \"" << outFileName << "\"\n";
			}

			aviOut->startPlaying(sessionAfterPlaying, NULL, this);
		}
	} else {
		// Create and start "FileSink"s for each subsession:
		madeProgress = False;
		MediaSubsessionIterator iter(*session);
		while ((subsession = iter.next()) != NULL) {
			if (subsession->readSource() == NULL)
				continue; // was not initiated

			// Create an output file for each desired stream:
			if (singleMedium == NULL || periodicFilenameSuffix[0] != '\0') {
				// Output file name is
				//     "<filename-prefix><medium_name>-<codec_name>-<counter><periodicFilenameSuffix>"
				static unsigned streamCounter = 0;
				snprintf(outFileName, sizeof outFileName, "%s%s-%s-%d%s",
						fileNamePrefix, subsession->mediumName(),
						subsession->codecName(), ++streamCounter,
						periodicFilenameSuffix);
			} else {
				// When outputting a single medium only, we output to 'stdout
				// (unless the '-P <interval-in-seconds>' option was given):
				sprintf(outFileName, "stdout");
			}

			FileSink *fileSink = NULL;
			Boolean createOggFileSink = False; // by default
			if (strcmp(subsession->mediumName(), "video") == 0) {
				if (strcmp(subsession->codecName(), "H264") == 0) {
					// For H.264 video stream, we use a special sink that adds 'start codes',
					// and (at the start) the SPS and PPS NAL units:
					//					cout
					//							<< "_fClientIDVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV: "
					//							<< _fClientID << endl;
					fileSink = H264VideoFileSink::createNew(*env, _fClientID,
							outFileName, subsession->fmtp_spropparametersets(),
							fileSinkBufferSize, oneFilePerFrame);
				} else if (strcmp(subsession->codecName(), "H265") == 0) {
					// For H.265 video stream, we use a special sink that adds 'start codes',
					// and (at the start) the VPS, SPS, and PPS NAL units:
					/// use this
					fileSink = H265VideoFileSink::createNew(*env, _fClientID,
							outFileName, subsession->fmtp_spropvps(),
							subsession->fmtp_spropsps(),
							subsession->fmtp_sproppps(), fileSinkBufferSize,
							oneFilePerFrame);
				} else if (strcmp(subsession->codecName(), "THEORA") == 0) {
					createOggFileSink = True;
				}
			} else if (strcmp(subsession->mediumName(), "audio") == 0) {
				if (strcmp(subsession->codecName(), "AMR") == 0
						|| strcmp(subsession->codecName(), "AMR-WB") == 0) {
					// For AMR audio streams, we use a special sink that inserts AMR frame hdrs:
					fileSink = AMRAudioFileSink::createNew(*env, outFileName,
							fileSinkBufferSize, oneFilePerFrame, this);
				} else if (strcmp(subsession->codecName(), "VORBIS") == 0
						|| strcmp(subsession->codecName(), "OPUS") == 0) {
					createOggFileSink = True;
				}
			}
			if (createOggFileSink) {
				fileSink = OggFileSink::createNew(*env, outFileName,
						subsession->rtpTimestampFrequency(),
						subsession->fmtp_config());
			} else if (fileSink == NULL) {
				// Normal case:

				fileSink = FileSink::createNew(*env, outFileName,
						fileSinkBufferSize, _fClientID, oneFilePerFrame, this);
			}
			subsession->sink = fileSink;

			if (subsession->sink == NULL) {
				*env << "Failed to create FileSink for \"" << outFileName
						<< "\": " << env->getResultMsg() << "\n";
			} else {
				if (singleMedium == NULL) {
					*env << "Created output file: \"" << outFileName << "\"\n";
				} else {
					*env << "Outputting data from the \""
							<< subsession->mediumName() << "/"
							<< subsession->codecName() << "\" subsession to \""
							<< outFileName << "\"\n";
				}

				if (strcmp(subsession->mediumName(), "video") == 0&&
				strcmp(subsession->codecName(), "MP4V-ES") == 0 &&
				subsession->fmtp_config() != NULL) {
					// For MPEG-4 video RTP streams, the 'config' information
					// from the SDP description contains useful VOL etc. headers.
					// Insert this data at the front of the output file:
					unsigned configLen;
					DP_U8 *configData = parseGeneralConfigStr(
							subsession->fmtp_config(), configLen);
					struct timeval timeNow;
					gettimeofday(&timeNow, NULL);
					fileSink->addData(configData, configLen, timeNow);
					delete[] configData;
				}

				///go continuePlaying
				subsession->sink->startPlaying(*(subsession->readSource()),
						subsessionAfterPlaying, subsession, this);

				// Also set a handler to be called if a RTCP "BYE" arrives
				// for this subsession:
				if (subsession->rtcpInstance() != NULL) {
					subsession->rtcpInstance()->setByeHandler(
							subsessionByeHandler, subsession);
				}

				madeProgress = True;
			}
		}
		if (!madeProgress)
			shutdown();
	}
}

// Create a filename suffix that notes the time interval that's being recorded:
void CommonPlay::createPeriodicOutputFiles() {
	char periodicFileNameSuffix[100];
	snprintf(periodicFileNameSuffix, sizeof periodicFileNameSuffix,
			"-%05d-%05d", fileOutputSecondsSoFar,
			fileOutputSecondsSoFar + fileOutputInterval);
	createOutputFiles(periodicFileNameSuffix);

	// Schedule an event for writing the next output file:
	periodicFileOutputTask =
			env->taskScheduler(_fClientID / 10)->scheduleDelayedTask(
					fileOutputInterval * 1000000,
					(TaskFunc *) periodicFileOutputTimerHandler, (void *) NULL,
					this);
}

void CommonPlay::setupStreams() {
	Logger::GetInstance().Debug(
			"setup stream ======================================================================= / create file env cliID: %d",
			_fClientID);
	//	static MediaSubsessionIterator* setupIter = NULL;

	//	MediaSubsessionIterator* setupIter = NULL; // enter a inf loop
	if (mSetupIter[_fClientID] == NULL)
		mSetupIter[_fClientID] = new MediaSubsessionIterator(*session);
	while ((subsession = mSetupIter[_fClientID]->next()) != NULL) {
		// We have another subsession left to set up:
		if (subsession->clientPortNum() == 0)
			continue; // port # was not set

		//first go
		setupSubsession(subsession, streamUsingTCP, forceMulticastOnUnspecified,
				continueAfterSETUP);
		return;
	}

	//second this
	// We're done setting up subsessions.
	delete mSetupIter[_fClientID];
	mSetupIter.erase(_fClientID);
	if (!madeProgress)
		shutdown();

	// Create output files:
	if (createReceivers) {
		if (fileOutputInterval > 0) {
			//go this
			createPeriodicOutputFiles();
		} else {

			createOutputFiles(""); ///Create a output file ///
		}
	}

	// Finally, start playing each subsession, to start the data flow:
	if (duration == 0) {
		if (scale > 0)
			duration = session->playEndTime() - initialSeekTime; // use SDP end time
		else if (scale < 0)
			duration = initialSeekTime;
	}
	if (duration < 0)
		duration = 0.0;

	endTime = initialSeekTime;
	if (scale > 0) {
		if (duration <= 0)
			endTime = -1.0f;
		else
			endTime = initialSeekTime + duration;
	} else {
		endTime = initialSeekTime - duration;
		if (endTime < 0)
			endTime = 0.0f;
	}

	char const *absStartTime =
			initialAbsoluteSeekTime != NULL ?
					initialAbsoluteSeekTime : session->absStartTime();
	char const *absEndTime =
			initialAbsoluteSeekEndTime != NULL ?
					initialAbsoluteSeekEndTime : session->absEndTime();
	if (absStartTime != NULL) {
		// Either we or the server have specified that seeking should be done by 'absolute' time:
		startPlayingSession(session, absStartTime, absEndTime, scale,
				continueAfterPLAY);
	} else {
		// Normal case: Seek by relative time (NPT):
		///
		startPlayingSession(session, initialSeekTime, endTime, scale,
				continueAfterPLAY);
	}
}

////
void CommonPlay::continueAfterPLAY(RTSPClient *rtspClient, int resultCode,
		char *resultString, CommonPlay *cpObj) {
//	cout << "]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"<<endl;
	Logger::GetInstance().Debug(
			"continueAfterPLAY ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]cliID::: %d ",
			cpObj->_fClientID);
	if (resultCode != 0) {
		*cpObj->env << "Failed to start playing session: " << resultString
				<< "\n";
		delete[] resultString;
		cpObj->shutdown();
		return;
	} else {
		*cpObj->env << "Started playing session\n";

#if 0
		pthread_mutex_lock(
				DP_RTSP_CLIENT_Client::mCliMuxSet[cpObj->_fClientID]);
		pthread_cond_signal(
				DP_RTSP_CLIENT_Client::mCliCondSet[cpObj->_fClientID]);
		pthread_mutex_unlock(
				DP_RTSP_CLIENT_Client::mCliMuxSet[cpObj->_fClientID]);
#endif
	}
	delete[] resultString;

	if (cpObj->qosMeasurementIntervalMS > 0) {
		// Begin periodic QOS measurements:
		cpObj->beginQOSMeasurement();
	}

	// Figure out how long to delay (if at all) before shutting down, or
	// repeating the playing
	Boolean timerIsBeingUsed = False;
	double secondsToDelay = cpObj->duration;
	//........>0 origin
	//	if (cpObj->duration == 0) {
	if (cpObj->duration > 0) {
		// First, adjust "duration" based on any change to the play range (that was specified in the "PLAY" response):
		double rangeAdjustment = (cpObj->session->playEndTime()
				- cpObj->session->playStartTime())
				- (cpObj->endTime - cpObj->initialSeekTime);
		if (cpObj->duration + rangeAdjustment > 0.0)
			cpObj->duration += rangeAdjustment;

		timerIsBeingUsed = True;
		double absScale = cpObj->scale > 0 ? cpObj->scale : -cpObj->scale; // ASSERT: scale != 0
		secondsToDelay = cpObj->duration / absScale + cpObj->durationSlop;

		int64_t uSecsToDelay = (int64_t) (secondsToDelay * 1000000.0);
		cpObj->sessionTimerTask = cpObj->env->taskScheduler(
				cpObj->_fClientID / 10)->scheduleDelayedTask(uSecsToDelay,
				(TaskFunc *) sessionTimerHandler, (void *) NULL, cpObj);
	}

	//	int64_t uSecsToDelay = (int64_t) (secondsToDelay * 1000000.0 * 60);
	int64_t uSecsToDelay = (int64_t) (1000000.0 * 60);

	cpObj->sessionTimerTask =
			cpObj->env->taskScheduler(cpObj->_fClientID / 10)->scheduleDelayedTask(
					uSecsToDelay, (TaskFunc *) sessionTimerHandler, rtspClient,
					cpObj);

	char const *actionString =
			cpObj->createReceivers ?
					"Receiving streamed data" : "Data is being streamed";
	if (timerIsBeingUsed) {
		*cpObj->env << actionString << " (for up to " << secondsToDelay
				<< " seconds)...\n";
	} else {
#ifdef USE_SIGNALS
		pid_t ourPid = getpid();
		*env << actionString << " (signal with \"kill -HUP " << (int)ourPid
		<< "\" or \"kill -USR1 " << (int)ourPid
		<< "\" to terminate)...\n";
#else
		*cpObj->env << actionString << "...\n";
#endif
	}

	cpObj->sessionTimeoutBrokenServerTask = NULL;

	///
	// Watch for incoming packets (if desired):
	CommonPlay::checkForPacketArrival(NULL, cpObj);
	CommonPlay::checkInterPacketGaps(NULL, cpObj);
	CommonPlay::checkSessionTimeoutBrokenServer(NULL, cpObj);
}

void CommonPlay::closeMediaSinks() {
	Medium::close(qtOut);
	qtOut = NULL;
	Medium::close(aviOut);
	aviOut = NULL;

	if (session == NULL)
		return;
	MediaSubsessionIterator iter(*session);
	MediaSubsession *subsession;
	while ((subsession = iter.next()) != NULL) {
		Medium::close(subsession->sink);
		subsession->sink = NULL;
	}
}

void CommonPlay::subsessionAfterPlaying(void *clientData, CommonPlay *cpObj) {
	// Begin by closing this media subsession's stream:
	MediaSubsession *subsession = (MediaSubsession *) clientData;
	Medium::close(subsession->sink);
	subsession->sink = NULL;

	// Next, check whether *all* subsessions' streams have now been closed:
	MediaSession &session = subsession->parentSession();
	MediaSubsessionIterator iter(session);
	while ((subsession = iter.next()) != NULL) {
		if (subsession->sink != NULL)
			return; // this subsession is still active
	}

	// All subsessions' streams have now been closed
	sessionAfterPlaying(NULL, cpObj);
}

void CommonPlay::subsessionByeHandler(void *clientData, CommonPlay *cpObj) {
	struct timeval timeNow;
	gettimeofday(&timeNow, NULL);
	unsigned secsDiff = timeNow.tv_sec - cpObj->startTime.tv_sec;

	MediaSubsession *subsession = (MediaSubsession *) clientData;
	*cpObj->env << "Received RTCP \"BYE\" on \"" << subsession->mediumName()
			<< "/" << subsession->codecName() << "\" subsession (after "
			<< secsDiff << " seconds)\n";

	// Act now as if the subsession had closed:
	subsessionAfterPlaying(subsession, cpObj);
}

void CommonPlay::sessionAfterPlaying(void * /*clientData*/, CommonPlay *cpObj) {
	if (!cpObj->playContinuously) {
		cpObj->shutdown(0);
	} else {
		// We've been asked to play the stream(s) over again.
		// First, reset state from the current session:
		if (cpObj->env != NULL) {
			cpObj->env->taskScheduler(cpObj->_fClientID / 10)->unscheduleDelayedTask(
					cpObj->periodicFileOutputTask);
			cpObj->env->taskScheduler(cpObj->_fClientID / 10)->unscheduleDelayedTask(
					cpObj->sessionTimerTask);
			cpObj->env->taskScheduler(cpObj->_fClientID / 10)->unscheduleDelayedTask(
					cpObj->sessionTimeoutBrokenServerTask);
			cpObj->env->taskScheduler(cpObj->_fClientID / 10)->unscheduleDelayedTask(
					cpObj->arrivalCheckTimerTask);
			cpObj->env->taskScheduler(cpObj->_fClientID / 10)->unscheduleDelayedTask(
					cpObj->interPacketGapCheckTimerTask);
			cpObj->env->taskScheduler(cpObj->_fClientID / 10)->unscheduleDelayedTask(
					cpObj->qosMeasurementTimerTask);
		}
		cpObj->totNumPacketsReceived = ~0;

		cpObj->startPlayingSession(cpObj->session, cpObj->initialSeekTime,
				cpObj->endTime, cpObj->scale, cpObj->continueAfterPLAY);
	}
}

void CommonPlay::sessionTimerHandler(void *clientData, CommonPlay *cpObj) {
	//////////run seg fault !!!!!!!!!!!!!!!!!!!!!!1
	Logger::GetInstance().Debug("333333333333333333333333333333333333333");
	int64_t uSecsToDelay = (int64_t) (1000000.0 * 60);

	RTSPClient *rtspClient = (RTSPClient *) clientData;
	cout << rtspClient->url() << endl;
	cout << "cpObj........................."
			<< cpObj->ourAuthenticator->password() << endl;
//	cpObj->sessionTimerTask = NULL;
	rtspClient->sendGetParameterCommand(*cpObj->session, continueAfterGetParam,
	NULL);
}

void CommonPlay::periodicFileOutputTimerHandler(void * /*clientData*/,
		CommonPlay *cpObj) {
	cpObj->fileOutputSecondsSoFar += cpObj->fileOutputInterval;

	// First, close the existing output files:
	cpObj->closeMediaSinks();

	// Then, create new output files:
	cpObj->createPeriodicOutputFiles();
}

class qosMeasurementRecord {
public:
	qosMeasurementRecord(struct timeval const &startTime, RTPSource *src) :
			fSource(src), fNext(NULL), kbits_per_second_min(1e20), kbits_per_second_max(
					0), kBytesTotal(0.0), packet_loss_fraction_min(1.0), packet_loss_fraction_max(
					0.0), totNumPacketsReceived(0), totNumPacketsExpected(0) {
		measurementEndTime = measurementStartTime = startTime;

		RTPReceptionStatsDB::Iterator statsIter(src->receptionStatsDB());
		// Assume that there's only one SSRC source (usually the case):
		RTPReceptionStats *stats = statsIter.next(True);
		if (stats != NULL) {
			kBytesTotal = stats->totNumKBytesReceived();
			totNumPacketsReceived = stats->totNumPacketsReceived();
			totNumPacketsExpected = stats->totNumPacketsExpected();
		}
	}
	virtual ~qosMeasurementRecord() {
		delete fNext;
	}

	void periodicQOSMeasurement(struct timeval const &timeNow);

public:
	RTPSource *fSource;
	qosMeasurementRecord *fNext;

public:
	struct timeval measurementStartTime, measurementEndTime;
	double kbits_per_second_min, kbits_per_second_max;
	double kBytesTotal;
	double packet_loss_fraction_min, packet_loss_fraction_max;
	unsigned totNumPacketsReceived, totNumPacketsExpected;
};

static qosMeasurementRecord *qosRecordHead = NULL;

static void periodicQOSMeasurement(void *clientData, CommonPlay *cpObj); // forward

static unsigned nextQOSMeasurementUSecs;

void CommonPlay::scheduleNextQOSMeasurement(CommonPlay *cpObj) {
	nextQOSMeasurementUSecs += cpObj->qosMeasurementIntervalMS * 1000;
	struct timeval timeNow;
	gettimeofday(&timeNow, NULL);
	unsigned timeNowUSecs = timeNow.tv_sec * 1000000 + timeNow.tv_usec;
	int usecsToDelay = nextQOSMeasurementUSecs - timeNowUSecs;

	cpObj->qosMeasurementTimerTask = cpObj->env->taskScheduler(
			cpObj->_fClientID / 10)->scheduleDelayedTask(usecsToDelay,
			(TaskFunc *) periodicQOSMeasurement, (void *) NULL, cpObj);
}

void CommonPlay::setUsrnamePassword(const DP_C_S8 *usrname,
		const DP_C_S8 *password) {
	ourAuthenticator = new Authenticator(usrname, password);
}

static void periodicQOSMeasurement(void * /*clientData*/, CommonPlay *cpObj) {
	struct timeval timeNow;
	gettimeofday(&timeNow, NULL);

	for (qosMeasurementRecord *qosRecord = qosRecordHead; qosRecord != NULL;
			qosRecord = qosRecord->fNext) {
		qosRecord->periodicQOSMeasurement(timeNow);
	}

	// Do this again later:
	CommonPlay::scheduleNextQOSMeasurement(cpObj);
}

void qosMeasurementRecord::periodicQOSMeasurement(
		struct timeval const &timeNow) {
	unsigned secsDiff = timeNow.tv_sec - measurementEndTime.tv_sec;
	int usecsDiff = timeNow.tv_usec - measurementEndTime.tv_usec;
	double timeDiff = secsDiff + usecsDiff / 1000000.0;
	measurementEndTime = timeNow;

	RTPReceptionStatsDB::Iterator statsIter(fSource->receptionStatsDB());
	// Assume that there's only one SSRC source (usually the case):
	RTPReceptionStats *stats = statsIter.next(True);
	if (stats != NULL) {
		double kBytesTotalNow = stats->totNumKBytesReceived();
		double kBytesDeltaNow = kBytesTotalNow - kBytesTotal;
		kBytesTotal = kBytesTotalNow;

		double kbpsNow = timeDiff == 0.0 ? 0.0 : 8 * kBytesDeltaNow / timeDiff;
		if (kbpsNow < 0.0)
			kbpsNow = 0.0; // in case of roundoff error
		if (kbpsNow < kbits_per_second_min)
			kbits_per_second_min = kbpsNow;
		if (kbpsNow > kbits_per_second_max)
			kbits_per_second_max = kbpsNow;

		unsigned totReceivedNow = stats->totNumPacketsReceived();
		unsigned totExpectedNow = stats->totNumPacketsExpected();
		unsigned deltaReceivedNow = totReceivedNow - totNumPacketsReceived;
		unsigned deltaExpectedNow = totExpectedNow - totNumPacketsExpected;
		totNumPacketsReceived = totReceivedNow;
		totNumPacketsExpected = totExpectedNow;

		double lossFractionNow =
				deltaExpectedNow == 0 ?
						0.0 :
						1.0 - deltaReceivedNow / (double) deltaExpectedNow;
		//if (lossFractionNow < 0.0) lossFractionNow = 0.0; //reordering can cause
		if (lossFractionNow < packet_loss_fraction_min) {
			packet_loss_fraction_min = lossFractionNow;
		}
		if (lossFractionNow > packet_loss_fraction_max) {
			packet_loss_fraction_max = lossFractionNow;
		}
	}
}

void CommonPlay::beginQOSMeasurement() {
	// Set up a measurement record for each active subsession:
	struct timeval startTime;
	gettimeofday(&startTime, NULL);
	nextQOSMeasurementUSecs = startTime.tv_sec * 1000000 + startTime.tv_usec;
	qosMeasurementRecord *qosRecordTail = NULL;
	MediaSubsessionIterator iter(*session);
	MediaSubsession *subsession;
	while ((subsession = iter.next()) != NULL) {
		RTPSource *src = subsession->rtpSource();
		if (src == NULL)
			continue;

		qosMeasurementRecord *qosRecord = new qosMeasurementRecord(startTime,
				src);
		if (qosRecordHead == NULL)
			qosRecordHead = qosRecord;
		if (qosRecordTail != NULL)
			qosRecordTail->fNext = qosRecord;
		qosRecordTail = qosRecord;
	}

	// Then schedule the first of the periodic measurements:
	scheduleNextQOSMeasurement(this);
}

void CommonPlay::printQOSData(int exitCode) {
	*env << "begin_QOS_statistics\n";

	// Print out stats for each active subsession:
	qosMeasurementRecord *curQOSRecord = qosRecordHead;
	if (session != NULL) {
		MediaSubsessionIterator iter(*session);
		MediaSubsession *subsession;
		while ((subsession = iter.next()) != NULL) {
			RTPSource *src = subsession->rtpSource();
			if (src == NULL)
				continue;

			*env << "subsession\t" << subsession->mediumName() << "/"
					<< subsession->codecName() << "\n";

			unsigned numPacketsReceived = 0, numPacketsExpected = 0;

			if (curQOSRecord != NULL) {
				numPacketsReceived = curQOSRecord->totNumPacketsReceived;
				numPacketsExpected = curQOSRecord->totNumPacketsExpected;
			}
			*env << "num_packets_received\t" << numPacketsReceived << "\n";
			*env << "num_packets_lost\t"
					<< int(numPacketsExpected - numPacketsReceived) << "\n";

			if (curQOSRecord != NULL) {
				unsigned secsDiff = curQOSRecord->measurementEndTime.tv_sec
						- curQOSRecord->measurementStartTime.tv_sec;
				int usecsDiff = curQOSRecord->measurementEndTime.tv_usec
						- curQOSRecord->measurementStartTime.tv_usec;
				double measurementTime = secsDiff + usecsDiff / 1000000.0;
				*env << "elapsed_measurement_time\t" << measurementTime << "\n";

				*env << "kBytes_received_total\t" << curQOSRecord->kBytesTotal
						<< "\n";

				*env << "measurement_sampling_interval_ms\t"
						<< qosMeasurementIntervalMS << "\n";

				if (curQOSRecord->kbits_per_second_max == 0) {
					// special case: we didn't receive any data:
					*env << "kbits_per_second_min\tunavailable\n"
							"kbits_per_second_ave\tunavailable\n"
							"kbits_per_second_max\tunavailable\n";
				} else {
					*env << "kbits_per_second_min\t"
							<< curQOSRecord->kbits_per_second_min << "\n";
					*env << "kbits_per_second_ave\t"
							<< (measurementTime == 0.0 ?
									0.0 :
									8 * curQOSRecord->kBytesTotal
											/ measurementTime) << "\n";
					*env << "kbits_per_second_max\t"
							<< curQOSRecord->kbits_per_second_max << "\n";
				}

				*env << "packet_loss_percentage_min\t"
						<< 100 * curQOSRecord->packet_loss_fraction_min << "\n";
				double packetLossFraction =
						numPacketsExpected == 0 ?
								1.0 :
								1.0
										- numPacketsReceived
												/ (double) numPacketsExpected;
				if (packetLossFraction < 0.0)
					packetLossFraction = 0.0;
				*env << "packet_loss_percentage_ave\t"
						<< 100 * packetLossFraction << "\n";
				*env << "packet_loss_percentage_max\t"
						<< (packetLossFraction == 1.0 ?
								100.0 :
								100 * curQOSRecord->packet_loss_fraction_max)
						<< "\n";

				RTPReceptionStatsDB::Iterator statsIter(
						src->receptionStatsDB());
				// Assume that there's only one SSRC source (usually the case):
				RTPReceptionStats *stats = statsIter.next(True);
				if (stats != NULL) {
					*env << "inter_packet_gap_ms_min\t"
							<< stats->minInterPacketGapUS() / 1000.0 << "\n";
					struct timeval totalGaps = stats->totalInterPacketGaps();
					double totalGapsMS = totalGaps.tv_sec * 1000.0
							+ totalGaps.tv_usec / 1000.0;
					unsigned totNumPacketsReceived =
							stats->totNumPacketsReceived();
					*env << "inter_packet_gap_ms_ave\t"
							<< (totNumPacketsReceived == 0 ?
									0.0 : totalGapsMS / totNumPacketsReceived)
							<< "\n";
					*env << "inter_packet_gap_ms_max\t"
							<< stats->maxInterPacketGapUS() / 1000.0 << "\n";
				}

				curQOSRecord = curQOSRecord->fNext;
			}
		}
	}

	*env << "end_QOS_statistics\n";
	delete qosRecordHead;
}

Boolean areAlreadyShuttingDown = False;
int shutdownExitCode;
void CommonPlay::shutdown(int exitCode) {
	if (areAlreadyShuttingDown)
		return; // in case we're called after receiving a RTCP "BYE" while in the middle of a "TEARDOWN".
	areAlreadyShuttingDown = True;

	shutdownExitCode = exitCode;
	if (env != NULL) {
		env->taskScheduler(_fClientID / 10)->unscheduleDelayedTask(
				periodicFileOutputTask);
		env->taskScheduler(_fClientID / 10)->unscheduleDelayedTask(
				sessionTimerTask);
		env->taskScheduler(_fClientID / 10)->unscheduleDelayedTask(
				sessionTimeoutBrokenServerTask);
		env->taskScheduler(_fClientID / 10)->unscheduleDelayedTask(
				arrivalCheckTimerTask);
		env->taskScheduler(_fClientID / 10)->unscheduleDelayedTask(
				interPacketGapCheckTimerTask);
		env->taskScheduler(_fClientID / 10)->unscheduleDelayedTask(
				qosMeasurementTimerTask);
	}

	if (qosMeasurementIntervalMS > 0) {
		printQOSData(exitCode);
	}

	// Teardown, then shutdown, any outstanding RTP/RTCP subsessions
	Boolean shutdownImmediately = True; // by default
	if (session != NULL) {
		RTSPClient::responseHandler *responseHandlerForTEARDOWN = NULL; // unless:
		if (waitForResponseToTEARDOWN) {
			shutdownImmediately = False;
			responseHandlerForTEARDOWN = continueAfterTEARDOWN;
		}
		tearDownSession(session, responseHandlerForTEARDOWN);
	}

	if (shutdownImmediately)
		continueAfterTEARDOWN(NULL, 0, NULL, this);
}

void CommonPlay::continueAfterTEARDOWN(RTSPClient *, int /*resultCode*/,
		char *resultString, CommonPlay *cpObj) {
	Logger::GetInstance().Debug(
			"continueAfterTEARDOWNTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTmSetupIter : %d",
			CommonPlay::mSetupIter.size());
	delete[] resultString;

	// Now that we've stopped any more incoming data from arriving, close our output files:
	cpObj->closeMediaSinks();
	Medium::close(cpObj->session);

	// Finally, shut down our client:
	delete cpObj->ourAuthenticator;
	delete cpObj->authDBForREGISTER;
	Medium::close(cpObj->ourClient);

	// Adios...
	exit(cpObj->shutdownExitCode);
}

void CommonPlay::continueAfterGetParam(RTSPClient *client, int resultCode,
		char *resultString, CommonPlay *cpObj) {

	struct timeval timeNow;
	gettimeofday(&timeNow, NULL);
	Logger::GetInstance().Warn("Time now : %d : %d", timeNow.tv_sec,
			timeNow.tv_usec);
	int64_t uSecsToDelay = (int64_t) (1000000.0 * 60);
	cpObj->sessionTimerTask =
			cpObj->env->taskScheduler(cpObj->_fClientID / 10)->scheduleDelayedTask(
					uSecsToDelay, (TaskFunc *) sessionTimerHandler, client,
					cpObj);

	//	UsageEnvironment& env = rtspClient->envir(); // alias

	//	StreamClientState& scs = ((ourRTSPClient*) rtspClient)->scs; // alias
	//
	//do {

	// Set a timer to be handled at the end of the stream's expected duration (if the stream does not already signal its end

	// using a RTCP "BYE").  This is optional.  If, instead, you want to keep the stream active - e.g., so you can later

	// 'seek' back within it and do another RTSP "PLAY" - then you can omit this code.

	// (Alternatively, if you don't want to receive the entire stream, you could set this timer for some shorter value.)
	//
	//	if (scs.duration > 0)
	//
	//	{
	//
	//		unsigned uSecsToDelay = (unsigned)(scs.duration*1000000);
	//
	//		scs.streamTimerTask = env.taskScheduler().scheduleDelayedTask(uSecsToDelay, (TaskFunc*)streamTimerHandler, rtspClient);
	//
	//	}
}
void CommonPlay::signalHandlerShutdown(int /*sig*/) {
	*env << "Got shutdown signal\n";
	waitForResponseToTEARDOWN = False; // to ensure that we end, even if the server does not respond to our TEARDOWN
	shutdown(0);
}

void CommonPlay::checkForPacketArrival(void * /*clientData*/,
		CommonPlay *cpObj) {
	if (!cpObj->notifyOnPacketArrival)
		return; // we're not checking

	// Check each subsession, to see whether it has received data packets:
	unsigned numSubsessionsChecked = 0;
	unsigned numSubsessionsWithReceivedData = 0;
	unsigned numSubsessionsThatHaveBeenSynced = 0;

	MediaSubsessionIterator iter(*cpObj->session);
	MediaSubsession *subsession;
	while ((subsession = iter.next()) != NULL) {
		RTPSource *src = subsession->rtpSource();
		if (src == NULL)
			continue;
		++numSubsessionsChecked;

		if (src->receptionStatsDB().numActiveSourcesSinceLastReset() > 0) {
			// At least one data packet has arrived
			++numSubsessionsWithReceivedData;
		}
		if (src->hasBeenSynchronizedUsingRTCP()) {
			++numSubsessionsThatHaveBeenSynced;
		}
	}

	unsigned numSubsessionsToCheck = numSubsessionsChecked;
	// Special case for "QuickTimeFileSink"s and "AVIFileSink"s:
	// They might not use all of the input sources:
	if (cpObj->qtOut != NULL) {
		numSubsessionsToCheck = cpObj->qtOut->numActiveSubsessions();
	} else if (cpObj->aviOut != NULL) {
		numSubsessionsToCheck = cpObj->aviOut->numActiveSubsessions();
	}

	Boolean notifyTheUser;
	if (!cpObj->syncStreams) {
		notifyTheUser = numSubsessionsWithReceivedData > 0; // easy case
	} else {
		notifyTheUser = numSubsessionsWithReceivedData >= numSubsessionsToCheck
				&& numSubsessionsThatHaveBeenSynced == numSubsessionsChecked;
		// Note: A subsession with no active sources is considered to be synced
	}
	if (notifyTheUser) {
		struct timeval timeNow;
		gettimeofday(&timeNow, NULL);
		char timestampStr[100];
		sprintf(timestampStr, "%ld%03ld", timeNow.tv_sec,
				(long) (timeNow.tv_usec / 1000));
		*cpObj->env << (cpObj->syncStreams ? "Synchronized d" : "D")
				<< "ata packets have begun arriving [" << timestampStr
				<< "]\007\n";
		return;
	}

	// No luck, so reschedule this check again, after a delay:
	int uSecsToDelay = 100000; // 100 ms
	cpObj->arrivalCheckTimerTask = cpObj->env->taskScheduler(
			cpObj->_fClientID / 10)->scheduleDelayedTask(uSecsToDelay,
			(TaskFunc *) checkForPacketArrival, NULL, cpObj);
}

void CommonPlay::checkInterPacketGaps(void * /*clientData*/,
		CommonPlay *cpObj) {
	if (cpObj->interPacketGapMaxTime == 0)
		return; // we're not checking

	// Check each subsession, counting up how many packets have been received:
	unsigned newTotNumPacketsReceived = 0;

	MediaSubsessionIterator iter(*cpObj->session);
	MediaSubsession *subsession;
	while ((subsession = iter.next()) != NULL) {
		RTPSource *src = subsession->rtpSource();
		if (src == NULL)
			continue;
		newTotNumPacketsReceived +=
				src->receptionStatsDB().totNumPacketsReceived();
	}

	if (newTotNumPacketsReceived == cpObj->totNumPacketsReceived) {
		// No additional packets have been received since the last time we
		// checked, so end this stream:
		*cpObj->env
				<< "Closing session, because we stopped receiving packets.\n";
		cpObj->interPacketGapCheckTimerTask = NULL;
		sessionAfterPlaying();
	} else {
		cpObj->totNumPacketsReceived = newTotNumPacketsReceived;
		// Check again, after the specified delay:
		cpObj->interPacketGapCheckTimerTask = cpObj->env->taskScheduler(
				cpObj->_fClientID / 10)->scheduleDelayedTask(
				cpObj->interPacketGapMaxTime * 1000000,
				(TaskFunc *) checkInterPacketGaps,
				NULL, cpObj);
	}
}

void CommonPlay::checkSessionTimeoutBrokenServer(void * /*clientData*/,
		CommonPlay *cpObj) {
	if (!cpObj->sendKeepAlivesToBrokenServers)
		return; // we're not checking

	// Send an "OPTIONS" request, starting with the second call
	if (cpObj->sessionTimeoutBrokenServerTask != NULL) {
		cpObj->getOptions(NULL);
	}

	unsigned sessionTimeout =
			cpObj->sessionTimeoutParameter == 0 ?
					60 /*default*/: cpObj->sessionTimeoutParameter;
	unsigned secondsUntilNextKeepAlive =
			sessionTimeout <= 5 ? 1 : sessionTimeout - 5;
	// Reduce the interval a little, to be on the safe side

	cpObj->sessionTimeoutBrokenServerTask = cpObj->env->taskScheduler(
			cpObj->_fClientID / 10)->scheduleDelayedTask(
			secondsUntilNextKeepAlive * 1000000,
			(TaskFunc *) checkSessionTimeoutBrokenServer, NULL, cpObj);
}
