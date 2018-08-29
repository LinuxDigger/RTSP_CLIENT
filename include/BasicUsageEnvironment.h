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
	static BasicUsageEnvironment* createNew(TaskScheduler& taskScheduler,
			unsigned short clientID);

	// redefined virtual functions:
	virtual int getErrno() const;

	virtual UsageEnvironment& operator<<(char const* str);
	virtual UsageEnvironment& operator<<(int i);
	virtual UsageEnvironment& operator<<(unsigned u);
	virtual UsageEnvironment& operator<<(double d);
	virtual UsageEnvironment& operator<<(void* p);

protected:
	BasicUsageEnvironment(TaskScheduler& taskScheduler,
			unsigned short clientID);
	// called only by "createNew()" (or subclass constructors)
	virtual ~BasicUsageEnvironment();
};

#endif /* INCLUDE_BASICUSAGEENVIRONMENT_H_ */
