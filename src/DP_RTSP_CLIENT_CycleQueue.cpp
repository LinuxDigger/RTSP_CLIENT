/*
 * DP_RTSP_CLIENT_CycleQueue.cpp
 *
 *  Created on: Aug 30, 2018
 *      Author: jhb
 */

#include "DP_RTSP_CLIENT_CycleQueue.h"

DP_RTSP_CLIENT_CycleQueue::DP_RTSP_CLIENT_CycleQueue(DP_U32 size) :
		CycleQueue(size) {
}

DP_RTSP_CLIENT_CycleQueue::~DP_RTSP_CLIENT_CycleQueue() {
}

DP_Bool DP_RTSP_CLIENT_CycleQueue::DSP_QueueInit() {
	pthread_mutex_lock(&mutex);
	m_cycleQueue->_buffer = m_buffer;
	m_cycleQueue->_size = m_size;
	m_cycleQueue->_in = 0;
	m_cycleQueue->_out = 0;
//	m_cycleQueue->_mutex = &mutex;
	pthread_mutex_unlock(&mutex);
	return true;
}

DP_U32 DP_RTSP_CLIENT_CycleQueue::DP_RTSP_CLIENT_PutData(const void *buff,
		DP_U32 size) {
	pthread_mutex_lock(&mutex);
	DP_U32 ret = 0;
	DP_U32 len = 0;
	DP_U32 putLen = size;
	if ((m_cycleQueue->_in - m_cycleQueue->_out) == m_cycleQueue->_size) {
		//full
		len =
				minimum(size,
						(m_cycleQueue->_size
								- m_cycleQueue->_in % m_cycleQueue->_size));

		memcpy(
				(DP_U8 *) m_cycleQueue->_buffer
						+ m_cycleQueue->_in % m_cycleQueue->_size, buff, len);
		memcpy(m_cycleQueue->_buffer, (DP_U8 *) buff + len, size - len);
		m_cycleQueue->_in += size;
		m_cycleQueue->_out += size;
		ret = size;
	} else {
		// not full
		size = minimum(size,
				m_cycleQueue->_size - m_cycleQueue->_in + m_cycleQueue->_out);
		/* first put the data starting from fifo->in to buffer end */
		len = minimum(size,
				m_cycleQueue->_size
						- (m_cycleQueue->_in % m_cycleQueue->_size));
		memcpy(
				(DP_U8*) m_cycleQueue->_buffer
						+ (m_cycleQueue->_in % m_cycleQueue->_size), buff, len);
		/* then put the rest (if any) at the beginning of the buffer */
		memcpy(m_cycleQueue->_buffer, (DP_U8*) buff + len, size - len);

		m_cycleQueue->_in += size;
		if (size < putLen) {
			memcpy(
					(DP_U8*) m_cycleQueue->_buffer
							+ m_cycleQueue->_in % m_cycleQueue->_size,
					(DP_U8*) buff + size, putLen - size);
			m_cycleQueue->_in += putLen - size;
			m_cycleQueue->_out += putLen - size;
		}
	}

//	cout << "m_cycleQueue->_in:::::::::::::::::::CCCCCCCCCCCCCCCC "
//			<< m_cycleQueue->_in << " m_cycleQueue-> out : "
//			<< m_cycleQueue->_out << endl;
	ret = size;
	pthread_mutex_unlock(&mutex);
	return ret;
}
DP_U32 DP_RTSP_CLIENT_CycleQueue::DP_RTSP_CLIENT_GetData(void *buff,
		DP_U32 len) {
	DP_U32 ret = 0;
	pthread_mutex_lock(&mutex);
	DP_U32 size = 0;
	len = minimum(len, m_cycleQueue->_in - m_cycleQueue->_out);
	/* first get the data from fifo->out until the end of the buffer */
	size = minimum(len,
			m_cycleQueue->_size
					- (m_cycleQueue->_out & (m_cycleQueue->_size - 1)));
	memcpy(buff,
			(DP_U8*) m_cycleQueue->_buffer
					+ (m_cycleQueue->_out & (m_cycleQueue->_size - 1)), size);
	/* then get the rest (if any) from the beginning of the buffer */
	memcpy((DP_U8*) buff + size, m_cycleQueue->_buffer, len - size);
	m_cycleQueue->_out += len;

	if (m_cycleQueue->_in == m_cycleQueue->_out)
		m_cycleQueue->_in = m_cycleQueue->_out = 0;

	ret = len;

	pthread_mutex_unlock(&mutex);



	return ret;
}
