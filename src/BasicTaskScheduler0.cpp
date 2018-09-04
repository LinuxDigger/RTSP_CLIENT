/*
 * BasicTaskScheduler0.cpp
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#include "BasicTaskScheduler0.h"
#include <iostream>
using namespace std;
////////// A subclass of DelayQueueEntry,
//////////     used to implement BasicTaskScheduler0::scheduleDelayedTask()

class AlarmHandler: public DelayQueueEntry {
public:
	AlarmHandler(TaskFunc* proc, void* clientData, DelayInterval timeToDelay,
			CommonPlay *cpObj) :
			DelayQueueEntry(timeToDelay), fProc(proc), fClientData(clientData), fcpObj(
					cpObj) {
	}

private:
	// redefined virtual functions
	//cpObj
	virtual void handleTimeout() {
		(*fProc)(fClientData, fcpObj);
		DelayQueueEntry::handleTimeout();
	}

private:
	TaskFunc* fProc;
	void* fClientData;
	CommonPlay *fcpObj;
};

////////// BasicTaskScheduler0 //////////

//when BasicTaskScheduler construction
BasicTaskScheduler0::BasicTaskScheduler0(CommonPlay *cpObj) :
		fLastHandledSocketNum(-1), fTriggersAwaitingHandling(0), fLastUsedTriggerMask(
				1), fLastUsedTriggerNum(MAX_NUM_EVENT_TRIGGERS - 1), fcpObj(
				cpObj) {
	fHandlers = new HandlerSet;  ///first new 111
	for (unsigned i = 0; i < MAX_NUM_EVENT_TRIGGERS; ++i) {
		fTriggeredEventHandlers[i] = NULL;
		fTriggeredEventClientDatas[i] = NULL;
	}
}

BasicTaskScheduler0::~BasicTaskScheduler0() {
	delete fHandlers;
}

TaskToken BasicTaskScheduler0::scheduleDelayedTask(int64_t microseconds,
		TaskFunc* proc, void* clientData,CommonPlay *cpObj) {
	if (microseconds < 0)
		microseconds = 0;
	DelayInterval timeToDelay((long) (microseconds / 1000000),
			(long) (microseconds % 1000000));
	AlarmHandler* alarmHandler = new AlarmHandler(proc, clientData, timeToDelay,
			fcpObj);
	fDelayQueue.addEntry(alarmHandler);

	return (void*) (alarmHandler->token());
}

void BasicTaskScheduler0::unscheduleDelayedTask(TaskToken& prevTask) {
	DelayQueueEntry* alarmHandler = fDelayQueue.removeEntry(
			(intptr_t) prevTask);
	prevTask = NULL;
	delete alarmHandler;
}

void BasicTaskScheduler0::doEventLoop(char volatile* watchVariable) {
	// Repeatedly loop, handling readble sockets and timed events:
	while (!pausePlay) {
		if (watchVariable != NULL && *watchVariable != 0)
			break;
		SingleStep();
	}
}

EventTriggerId BasicTaskScheduler0::createEventTrigger(
		TaskFunc* eventHandlerProc) {
	unsigned i = fLastUsedTriggerNum;
	EventTriggerId mask = fLastUsedTriggerMask;

	do {
		i = (i + 1) % MAX_NUM_EVENT_TRIGGERS;
		mask >>= 1;
		if (mask == 0)
			mask = 0x80000000;

		if (fTriggeredEventHandlers[i] == NULL) {
			// This trigger number is free; use it:
			fTriggeredEventHandlers[i] = eventHandlerProc;
			fTriggeredEventClientDatas[i] = NULL; // sanity

			fLastUsedTriggerMask = mask;
			fLastUsedTriggerNum = i;

			return mask;
		}
	} while (i != fLastUsedTriggerNum);

	// All available event triggers are allocated; return 0 instead:
	return 0;
}

void BasicTaskScheduler0::deleteEventTrigger(EventTriggerId eventTriggerId) {
	fTriggersAwaitingHandling &= ~eventTriggerId;

	if (eventTriggerId == fLastUsedTriggerMask) { // common-case optimization:
		fTriggeredEventHandlers[fLastUsedTriggerNum] = NULL;
		fTriggeredEventClientDatas[fLastUsedTriggerNum] = NULL;
	} else {
		// "eventTriggerId" should have just one bit set.
		// However, we do the reasonable thing if the user happened to 'or' together two or more "EventTriggerId"s:
		EventTriggerId mask = 0x80000000;
		for (unsigned i = 0; i < MAX_NUM_EVENT_TRIGGERS; ++i) {
			if ((eventTriggerId & mask) != 0) {
				fTriggeredEventHandlers[i] = NULL;
				fTriggeredEventClientDatas[i] = NULL;
			}
			mask >>= 1;
		}
	}
}

void BasicTaskScheduler0::triggerEvent(EventTriggerId eventTriggerId,
		void* clientData) {
	// First, record the "clientData".  (Note that we allow "eventTriggerId" to be a combination of bits for multiple events.)
	EventTriggerId mask = 0x80000000;
	for (unsigned i = 0; i < MAX_NUM_EVENT_TRIGGERS; ++i) {
		if ((eventTriggerId & mask) != 0) {
			fTriggeredEventClientDatas[i] = clientData;
		}
		mask >>= 1;
	}

	// Then, note this event as being ready to be handled.
	// (Note that because this function (unlike others in the library) can be called from an external thread, we do this last, to
	//  reduce the risk of a race condition.)
	fTriggersAwaitingHandling |= eventTriggerId;
}

