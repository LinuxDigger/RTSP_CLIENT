/*
 * BasicTaskScheduler0.h
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_BASICTASKSCHEDULER0_H_
#define INCLUDE_BASICTASKSCHEDULER0_H_

#include "DelayQueue.h"
#include "HandlerSet.h"
#include "TaskScheduler.h"

#define MAX_NUM_EVENT_TRIGGERS 32

// An abstract base class, useful for subclassing
// (e.g., to redefine the implementation of socket event handling)
class BasicTaskScheduler0: public TaskScheduler {
public:
	virtual ~BasicTaskScheduler0();

	virtual void SingleStep(unsigned maxDelayTime = 0) = 0;
	// "maxDelayTime" is in microseconds.  It allows a subclass to impose a limit
	// on how long "select()" can delay, in case it wants to also do polling.
	// 0 (the default value) means: There's no maximum; just look at the delay queue

public:
	// Redefined virtual functions:
	virtual TaskToken scheduleDelayedTask(int64_t microseconds, TaskFunc* proc,
			void* clientData);
	virtual void unscheduleDelayedTask(TaskToken& prevTask);

	virtual void doEventLoop(char volatile* watchVariable);

	virtual EventTriggerId createEventTrigger(TaskFunc* eventHandlerProc);
	virtual void deleteEventTrigger(EventTriggerId eventTriggerId);
	virtual void triggerEvent(EventTriggerId eventTriggerId, void* clientData =
			NULL);

protected:
	BasicTaskScheduler0();

protected:
	// To implement delayed operations:
	DelayQueue fDelayQueue;

	// To implement background reads:
	HandlerSet* fHandlers;
	int fLastHandledSocketNum;

	// To implement event triggers:
	EventTriggerId volatile fTriggersAwaitingHandling; // implemented as a 32-bit bitmap
	EventTriggerId fLastUsedTriggerMask; // implemented as a 32-bit bitmap
	TaskFunc* fTriggeredEventHandlers[MAX_NUM_EVENT_TRIGGERS];
	void* fTriggeredEventClientDatas[MAX_NUM_EVENT_TRIGGERS];
	unsigned fLastUsedTriggerNum; // in the range [0,MAX_NUM_EVENT_TRIGGERS)
};

#endif /* INCLUDE_BASICTASKSCHEDULER0_H_ */
