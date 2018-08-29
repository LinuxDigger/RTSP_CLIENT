/*
 * Thread.h
 *
 *  Created on: Aug 6, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_THREAD_H_
#define INCLUDE_THREAD_H_

#include "Mutex.h"
#include <pthread.h>
#include <unistd.h>
class Thread {
public:
	Thread() {
		_thread = 0;
		_running = false;
	}
	virtual ~Thread() {
	}

	virtual void run() = 0;

	//開始線程
	void start() {
		int ret;
		{
			AutoLock lock(&_stateMutex);
//			if (_running)
//				throw SystemException("error:thread is running");
			_running = true;
		}
		ret = pthread_create(&_thread, 0, doThread, this);
//		if (ret != 0)
//			throw SystemException(__FILE__, __FUNCTION__, __LINE__);
	}

	bool isAlive() {
		AutoLock lock(&_stateMutex);
		return _running;
	}

	//等待線程推出
	void waitForExit() {
		if (isAlive()) {
			pthread_join(_thread, 0);
		}
	}

	static void* doThread(void* pVoid) {
		Thread* pThis = (Thread*) pVoid;
		pThis->run();
		pThis->_done();
		return 0;
	}

	void _done() {
		AutoLock lock(&_stateMutex);
		_running = false;
	}

	static void Sleep(int ms) {
		usleep(ms * 1000);
	}
private:
	pthread_t _thread;
	Mutex _stateMutex;
	bool _running;
};

#endif /* INCLUDE_THREAD_H_ */
