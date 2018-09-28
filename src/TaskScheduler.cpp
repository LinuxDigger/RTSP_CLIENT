/*
 * TaskScheduler.cpp
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#include <iostream>
#include "TaskScheduler.h"
using namespace std;
pthread_mutex_t TaskScheduler::mutex;

TaskScheduler::TaskScheduler(DP_U16 scheIndex, DP_U32 urlNumsEachSche) :
		pausePlay(false), _bClientSetIsFull(false), _u32UrlNumsEachSche(
				urlNumsEachSche), _bScheThreadStatus(false), _u16ScheIndex(
				scheIndex) {
	pthread_mutex_init(&mutex, NULL);
	for (DP_U16 i = 0; i < (DP_U16) _u32UrlNumsEachSche; i++) {
		_mClientSet[i] = NULL;
	}
}

DP_U16 TaskScheduler::getIdleClientNum() {
	pthread_mutex_lock(&mutex);
	for (DP_U16 i = 1; i <= _u32UrlNumsEachSche; i++) {
		if (_mClientSet[i + _u16ScheIndex * 10] == NULL) {
			if (i == _u32UrlNumsEachSche) {
				_bClientSetIsFull = true;
			}
			pthread_mutex_unlock(&mutex);
			return i;
		} else if (i == _u32UrlNumsEachSche) {
			_bClientSetIsFull = true;
		}
	}
	pthread_mutex_unlock(&mutex);
	return 0;
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

