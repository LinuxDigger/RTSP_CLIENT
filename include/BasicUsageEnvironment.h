/*
 * BasicUsageEnvironment.h
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_BASICUSAGEENVIRONMENT_H_
#define INCLUDE_BASICUSAGEENVIRONMENT_H_

#include "UsageEnvironment0.h"

class BasicUsageEnvironment: public BasicUsageEnvironment0 {
public:
	//old
//	static BasicUsageEnvironment* createNew(TaskScheduler& taskScheduler,
//			unsigned short clientID);

	static BasicUsageEnvironment* createNew(
			vector<TaskScheduler*> * taskScheduler, unsigned short clientID);

//	DP_U16 getAIdleClientFromEnv() {
//		for (DP_U16 i = 0; i < fScheduler.size(); i++) {
//			if (!fScheduler[i].isClientSetFull()) {
//				return fScheduler[i].getIdleClientNum();
//			} else if (i == fScheduler.size() - 1)
//				return 0;
//		}
//		return 0;
//	}

	// redefined virtual functions:
	virtual int getErrno() const;

	virtual UsageEnvironment& operator<<(char const* str);
	virtual UsageEnvironment& operator<<(int i);
	virtual UsageEnvironment& operator<<(unsigned u);
	virtual UsageEnvironment& operator<<(double d);
	virtual UsageEnvironment& operator<<(void* p);

protected:
	BasicUsageEnvironment(vector<TaskScheduler*> * taskScheduler,
			unsigned short clientID);
	// called only by "createNew()" (or subclass constructors)
	virtual ~BasicUsageEnvironment();
};

#endif /* INCLUDE_BASICUSAGEENVIRONMENT_H_ */
