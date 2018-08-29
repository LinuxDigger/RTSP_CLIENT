/*
 * UsageEnvironment0.cpp
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#include "UsageEnvironment0.h"
#include "stdio.h"

////////// BasicUsageEnvironment //////////

BasicUsageEnvironment0::BasicUsageEnvironment0(TaskScheduler& taskScheduler,
		unsigned short clientID) :
		UsageEnvironment(taskScheduler, clientID), fBufferMaxSize(
				RESULT_MSG_BUFFER_MAX) {
	reset();
}

BasicUsageEnvironment0::~BasicUsageEnvironment0() {
}

void BasicUsageEnvironment0::reset() {
	fCurBufferSize = 0;
	fResultMsgBuffer[fCurBufferSize] = '\0';
}

// Implementation of virtual functions:

char const* BasicUsageEnvironment0::getResultMsg() const {
	return fResultMsgBuffer;
}

void BasicUsageEnvironment0::setResultMsg(MsgString msg) {
	reset();
	appendToResultMsg(msg);
}

void BasicUsageEnvironment0::setResultMsg(MsgString msg1, MsgString msg2) {
	setResultMsg(msg1);
	appendToResultMsg(msg2);
}

void BasicUsageEnvironment0::setResultMsg(MsgString msg1, MsgString msg2,
		MsgString msg3) {
	setResultMsg(msg1, msg2);
	appendToResultMsg(msg3);
}

void BasicUsageEnvironment0::setResultErrMsg(MsgString msg, int err) {
	setResultMsg(msg);

	if (err == 0)
		err = getErrno();
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
	char errMsg[RESULT_MSG_BUFFER_MAX] = "\0";
	if (0 != FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, errMsg, sizeof(errMsg)/sizeof(errMsg[0]), NULL)) {
		// Remove all trailing '\r', '\n' and '.'
		for (char* p = errMsg + strlen(errMsg); p != errMsg && (*p == '\r' || *p == '\n' || *p == '.' || *p == '\0'); --p) {
			*p = '\0';
		}
	} else
	snprintf(errMsg, sizeof(errMsg)/sizeof(errMsg[0]), "error %d", err);
	appendToResultMsg(errMsg);
#else
	appendToResultMsg(strerror(err));
#endif
}

void BasicUsageEnvironment0::appendToResultMsg(MsgString msg) {
	char* curPtr = &fResultMsgBuffer[fCurBufferSize];
	unsigned spaceAvailable = fBufferMaxSize - fCurBufferSize;
	unsigned msgLength = strlen(msg);

	// Copy only enough of "msg" as will fit:
	if (msgLength > spaceAvailable - 1) {
		msgLength = spaceAvailable - 1;
	}

	memmove(curPtr, (char*) msg, msgLength);
	fCurBufferSize += msgLength;
	fResultMsgBuffer[fCurBufferSize] = '\0';
}

void BasicUsageEnvironment0::reportBackgroundError() {
	fputs(getResultMsg(), stderr);
}

