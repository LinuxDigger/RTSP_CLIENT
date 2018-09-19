/*
 * TaskScheduler.cpp
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#include "TaskScheduler.h"

pthread_mutex_t TaskScheduler::mutex;

TaskScheduler::TaskScheduler(DP_U32 urlNumsEachSche) :
		pausePlay(false), _bClientSetIsFull(false), _u32UrlNumsEachSche(
				urlNumsEachSche), _bScheThreadStatus(false) {
	pthread_mutex_init(&mutex, NULL);
	for (DP_U16 i = 0; i < (DP_U16) _u32UrlNumsEachSche; i++) {
		_mClientSet[i] = NULL;
	}
}

TaskScheduler::~TaskScheduler() {
}

void TaskScheduler::rescheduleDelayedTask(TaskToken& task, int64_t microseconds,
		TaskFunc* proc, void* clientData) {
	unscheduleDelayedTask(task);
	task = scheduleDelayedTask(microseconds, proc, clientData, NULL);
}

// By default, we handle 'should not occur'-type library errors by calling abort().  Subclasses can redefine this, if desired.
void TaskScheduler::internalError() {
	abort();
}

