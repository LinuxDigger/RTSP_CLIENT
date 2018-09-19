/*
 * TaskScheduler.h
 *
 *  Created on: Jun 7, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_TASKSCHEDULER_H_
#define INCLUDE_TASKSCHEDULER_H_

#include <string.h>
#include "NetCommon.h"
//#include "CommonPlay.h"
#include "DP_RTSP_CLIENT_GlobDefine.h"
//typedef unsigned u_int32_t;
#include <map>
#include <pthread.h>
//#include <Media.h>

using namespace std;
class Medium;
class CommonPlay;
typedef void TaskFunc(void* clientData, CommonPlay *cpObj);
typedef void* TaskToken;
typedef u_int32_t EventTriggerId;

class TaskScheduler {
public:
	static pthread_mutex_t mutex;
	virtual ~TaskScheduler();
	bool pausePlay;
	map<int, CommonPlay *> _mSockfdCpSet;
	//start from 1
	map<DP_U16, Medium*> _mClientSet;

	DP_U16 getIdleClientNum() {
		pthread_mutex_lock(&mutex);
		for (DP_U16 i = 1; i <= _mClientSet.size(); i++) {
			if (_mClientSet[i] == NULL)
				return i;
			else if (i == _mClientSet.size())
				_bClientSetIsFull = true;
		}
		pthread_mutex_unlock(&mutex);
		return 0;
	}

	void setAClientMedium(Medium *client, DP_U16 cliID) {
		pthread_mutex_lock(&mutex);
		_mClientSet[cliID] = client;
		pthread_mutex_unlock(&mutex);
	}
	DP_Bool isClientSetFull() {
		return _bClientSetIsFull;
	}

	Medium* getAClient(DP_U16 clientHandle) {
		return _mClientSet[clientHandle];
	}

	DP_Bool taskScheThreadEnable() const {
		return _bScheThreadStatus;
	}

	void setScheThreadStatus(const DP_Bool status) {
		_bScheThreadStatus = status;
	}

	virtual TaskToken scheduleDelayedTask(int64_t microseconds, TaskFunc* proc,
			void* clientData, CommonPlay *cpObj) = 0;
	// Schedules a task to occur (after a delay) when we next
	// reach a scheduling point.
	// (Does not delay if "microseconds" <= 0)
	// Returns a token that can be used in a subsequent call to
	// unscheduleDelayedTask() or rescheduleDelayedTask()
	// (but only if the task has not yet occurred).

	virtual void unscheduleDelayedTask(TaskToken& prevTask) = 0;
	// (Has no effect if "prevTask" == NULL)
	// Sets "prevTask" to NULL afterwards.
	// Note: This MUST NOT be called if the scheduled task has already occurred.

	virtual void rescheduleDelayedTask(TaskToken& task, int64_t microseconds,
			TaskFunc* proc, void* clientData);
	// Combines "unscheduleDelayedTask()" with "scheduleDelayedTask()"
	// (setting "task" to the new task token).
	// Note: This MUST NOT be called if the scheduled task has already occurred.

	// For handling socket operations in the background (from the event loop):
	typedef void BackgroundHandlerProc(void* clientData, int mask,
			CommonPlay *cpObj);
	// Possible bits to set in "mask".  (These are deliberately defined
	// the same as those in Tcl, to make a Tcl-based subclass easy.)
#define SOCKET_READABLE    (1<<1)
#define SOCKET_WRITABLE    (1<<2)
#define SOCKET_EXCEPTION   (1<<3)
	virtual void setBackgroundHandling(int socketNum, int conditionSet,
			BackgroundHandlerProc* handlerProc, void* clientData) = 0;
	void disableBackgroundHandling(int socketNum) {   //NULL
		setBackgroundHandling(socketNum, 0, NULL, NULL);
	}
	virtual void moveSocketHandling(int oldSocketNum, int newSocketNum) = 0;
	// Changes any socket handling for "oldSocketNum" so that occurs with "newSocketNum" instead.

	virtual void doEventLoop(char volatile* watchVariable = NULL) = 0;
	// Causes further execution to take place within the event loop.
	// Delayed tasks, background I/O handling, and other events are handled, sequentially (as a single thread of control).
	// (If "watchVariable" is not NULL, then we return from this routine when *watchVariable != 0)

	virtual EventTriggerId createEventTrigger(TaskFunc* eventHandlerProc) = 0;
	// Creates a 'trigger' for an event, which - if it occurs - will be handled (from the event loop) using "eventHandlerProc".
	// (Returns 0 iff no such trigger can be created (e.g., because of implementation limits on the number of triggers).)
	virtual void deleteEventTrigger(EventTriggerId eventTriggerId) = 0;

	virtual void triggerEvent(EventTriggerId eventTriggerId, void* clientData =
	NULL) = 0;
	// Causes the (previously-registered) handler function for the specified event to be handled (from the event loop).
	// The handler function is called with "clientData" as parameter.
	// Note: This function (unlike other library functions) may be called from an external thread
	// - to signal an external event.  (However, "triggerEvent()" should not be called with the
	// same 'event trigger id' from different threads.)

	// The following two functions are deprecated, and are provided for backwards-compatibility only:
	void turnOnBackgroundReadHandling(int socketNum,
			BackgroundHandlerProc* handlerProc, void* clientData) {
		setBackgroundHandling(socketNum, SOCKET_READABLE, handlerProc,
				clientData);
	}
	void turnOffBackgroundReadHandling(int socketNum) {
		disableBackgroundHandling(socketNum);
	}

	virtual void internalError(); // used to 'handle' a 'should not occur'-type error condition within the library.

protected:
	TaskScheduler(DP_U32 urlNumsEachSche); // abstract base class
	DP_Bool _bClientSetIsFull;
	DP_U32 _u32UrlNumsEachSche;
	DP_Bool _bScheThreadStatus;

};

#endif /* INCLUDE_TASKSCHEDULER_H_ */
