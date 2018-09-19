/*
 * UsageEnvironment.h
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_USAGEENVIRONMENT_H_
#define INCLUDE_USAGEENVIRONMENT_H_

#include "NetCommon.h"
#include "Boolean.h"
#include "TaskScheduler.h"
//#include "CommonPlay.h"
#include <iostream>
#include <vector>
using namespace std;

class UsageEnvironment {
public:
	Boolean reclaim();
	// returns True iff we were actually able to delete our object

	//old
//	TaskScheduler& taskScheduler() const {
//		return fScheduler;
//	}
	TaskScheduler* taskScheduler(DP_U32 schedulerNum) const {
		if (schedulerNum < fScheduler.size())
			return fScheduler[schedulerNum];
		else
			return (TaskScheduler*) NULL;
	}

	void setFScheduler(const vector<TaskScheduler*> *scheduler) {
		fScheduler = *scheduler;
	}

	DP_U16 getAIdleClientFromEnv() {
		for (DP_U16 i = 0; i < fScheduler.size(); i++) {
			if (!fScheduler[i]->isClientSetFull()) {
				return (fScheduler[i]->getIdleClientNum() + i * 10);
			} else if (i == fScheduler.size() - 1)
				return 0;
		}
		return 0;
	}

	// result message handling:
	typedef char const* MsgString;
	virtual MsgString getResultMsg() const = 0;

	virtual void setResultMsg(MsgString msg) = 0;
	virtual void setResultMsg(MsgString msg1, MsgString msg2) = 0;
	virtual void setResultMsg(MsgString msg1, MsgString msg2,
			MsgString msg3) = 0;
	virtual void setResultErrMsg(MsgString msg, int err = 0) = 0;
	// like setResultMsg(), except that an 'errno' message is appended.  (If "err == 0", the "getErrno()" code is used instead.)

	virtual void appendToResultMsg(MsgString msg) = 0;

	virtual void reportBackgroundError() = 0;
	// used to report a (previously set) error message within
	// a background event

	virtual void internalError(); // used to 'handle' a 'should not occur'-type error condition within the library.

	// 'errno'
	virtual int getErrno() const = 0;

	// 'console' output:
	virtual UsageEnvironment& operator<<(char const* str) = 0;
	virtual UsageEnvironment& operator<<(int i) = 0;
	virtual UsageEnvironment& operator<<(unsigned u) = 0;
	virtual UsageEnvironment& operator<<(double d) = 0;
	virtual UsageEnvironment& operator<<(void* p) = 0;

	UsageEnvironment &operator=(const UsageEnvironment &env);

	// a pointer to additional, optional, client-specific state
	void* liveMediaPriv;
	void* groupsockPriv;

	unsigned short _u16CliID;

	//old
//	TaskScheduler& fScheduler;

	vector<TaskScheduler*> fScheduler;

protected:
	UsageEnvironment(vector<TaskScheduler*> * scheduler,
			unsigned short clientID); // abstract base class
	virtual ~UsageEnvironment(); // we are deleted only by reclaim()

private:

};

#endif /* INCLUDE_USAGEENVIRONMENT_H_ */
