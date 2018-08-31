/*
 * CycleQueue.h
 *
 *  Created on: Aug 24, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_CYCLEQUEUE_H_
#define INCLUDE_CYCLEQUEUE_H_
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include "DP_RTSP_CLIENT_GlobDefine.h"
using namespace std;

typedef struct _CycleQueue_S {
	void *_buffer;     //缓冲区
	DP_U32 _size;       //大小
	DP_U32 _in;         //入口位置
	DP_U32 _out;        //出口位置
//	pthread_mutex_t *_mutex;    //互斥锁
} CycleQueue_S;

class CycleQueue {
public:
	CycleQueue(DP_U32 size);
	//copy constructor
	virtual ~CycleQueue();

	void DSP_QueueReset();
	DP_U32 DSP_PutData(const void *buff, DP_U32 size);
	DP_U32 DSP_GetData(void *buff, DP_U32 len);
	virtual DP_Bool DSP_QueueInit();
	DP_Bool DSP_QueueIsEmpty();
	DP_Bool DSP_QueueIsFull();
	DP_U32 DSP_QueueDataLen();
	DP_U32 DSP_QueueSpaceLeft();
protected:
	CycleQueue_S *m_cycleQueue;
	DP_U8 *m_buffer;
	DP_U32 m_size;
	static pthread_mutex_t mutex;

	inline DP_Bool queueIsEmpty() {
		return m_cycleQueue->_in == m_cycleQueue->_out;
	}
	inline DP_Bool queueIsFull() {
		return (m_cycleQueue->_in - m_cycleQueue->_out) == m_cycleQueue->_size;
	}
	inline DP_U32 queueDataLen() {
		return m_cycleQueue->_in - m_cycleQueue->_out;
	}
	inline DP_U32 queueSpaceLeft() {
		return m_cycleQueue->_size - m_cycleQueue->_in + m_cycleQueue->_out;
	}

	inline DP_Bool isPowerOfTwo(DP_U32 x) {
		return (DP_Bool) (x != 0 && ((x & (x - 1)) == 0));
	}

	inline DP_U32 minimum(DP_U32 a, DP_U32 b) {
		return ((a < b) ? a : b);
	}
	CycleQueue();
};

#endif /* INCLUDE_CYCLEQUEUE_H_ */
