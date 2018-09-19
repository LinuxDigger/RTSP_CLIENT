/*
 * BasicTaskScheduler.h
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_BASICTASKSCHEDULER_H_
#define INCLUDE_BASICTASKSCHEDULER_H_

#include "NetCommon.h"
#include "BasicTaskScheduler0.h"
#include <vector>

class BasicTaskScheduler: public BasicTaskScheduler0 {
public:
//	static BasicTaskScheduler* createNew(unsigned maxSchedulerGranularity =
//			10000/*microseconds*/, CommonPlay *cpObj = NULL);
	// "maxSchedulerGranularity" (default value: 10 ms) specifies the maximum time that we wait (in "select()") before
	// returning to the event loop to handle non-socket or non-timer-based events, such as 'triggered events'.
	// You can change this is you wish (but only if you know what you're doing!), or set it to 0, to specify no such maximum time.
	// (You should set it to 0 only if you know that you will not be using 'event triggers'.)
	static vector<TaskScheduler*> *createNew(DP_U32 maxConnectionCnt,
			DP_U32 urlNumsEachSche,
			unsigned maxSchedulerGranularity = 10000/*microseconds*/,
			CommonPlay *cpObj = NULL);
	virtual ~BasicTaskScheduler();

protected:
	BasicTaskScheduler(DP_U32 urlNumsEachSche, unsigned maxSchedulerGranularity,
			CommonPlay *cpObj);
	// called only by "createNew()"

	static void schedulerTickTask(void* clientData, CommonPlay *cpObj);
	void schedulerTickTask();
public:
protected:
	// Redefined virtual functions:
	virtual void SingleStep(unsigned maxDelayTime); //protected

	virtual void setBackgroundHandling(int socketNum, int conditionSet,
			BackgroundHandlerProc* handlerProc, void* clientData);
	virtual void moveSocketHandling(int oldSocketNum, int newSocketNum);

protected:
	unsigned fMaxSchedulerGranularity;

	// To implement background operations:
	int fMaxNumSockets;
	fd_set fReadSet;
	fd_set fWriteSet;
	fd_set fExceptionSet;

private:
#if defined(__WIN32__) || defined(_WIN32)
	// Hack to work around a bug in Windows' "select()" implementation:
	int fDummySocketNum;
#endif
};
#endif /* INCLUDE_BASICTASKSCHEDULER_H_ */
