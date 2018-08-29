/*
 * TaskScheduler.cpp
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#include "TaskScheduler.h"

TaskScheduler::TaskScheduler() :
		pausePlay(false) {
}

TaskScheduler::~TaskScheduler() {
}

void TaskScheduler::rescheduleDelayedTask(TaskToken& task, int64_t microseconds,
		TaskFunc* proc, void* clientData) {
	unscheduleDelayedTask(task);
	task = scheduleDelayedTask(microseconds, proc, clientData);
}

// By default, we handle 'should not occur'-type library errors by calling abort().  Subclasses can redefine this, if desired.
void TaskScheduler::internalError() {
	abort();
}

