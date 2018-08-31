/*
 * DP_RTSP_CLIENT_CycleQueue.h
 *
 *  Created on: Aug 30, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_DP_RTSP_CLIENT_CYCLEQUEUE_H_
#define INCLUDE_DP_RTSP_CLIENT_CYCLEQUEUE_H_

#include "CycleQueue.h"

class DP_RTSP_CLIENT_CycleQueue: public CycleQueue {
public:
	DP_RTSP_CLIENT_CycleQueue(DP_U32 size);
	~DP_RTSP_CLIENT_CycleQueue();

	DP_U32 DP_RTSP_CLIENT_PutData(const void *buff, DP_U32 size);
	DP_U32 DP_RTSP_CLIENT_GetData(void *buff, DP_U32 len);
	virtual DP_Bool DSP_QueueInit();

};

#endif /* INCLUDE_DP_RTSP_CLIENT_CYCLEQUEUE_H_ */
