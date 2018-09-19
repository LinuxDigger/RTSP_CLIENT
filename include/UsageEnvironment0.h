/*
 * UsageEnvironment0.h
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_USAGEENVIRONMENT0_H_
#define INCLUDE_USAGEENVIRONMENT0_H_

#include "UsageEnvironment.h"

#define RESULT_MSG_BUFFER_MAX 1000

// An abstract base class, useful for subclassing
// (e.g., to redefine the implementation of "operator<<")
class BasicUsageEnvironment0: public UsageEnvironment {
public:
  // redefined virtual functions:
  virtual MsgString getResultMsg() const;

  virtual void setResultMsg(MsgString msg);
  virtual void setResultMsg(MsgString msg1,
		    MsgString msg2);
  virtual void setResultMsg(MsgString msg1,
		    MsgString msg2,
		    MsgString msg3);
  virtual void setResultErrMsg(MsgString msg, int err = 0);

  virtual void appendToResultMsg(MsgString msg);

  virtual void reportBackgroundError();

protected:
  BasicUsageEnvironment0(vector<TaskScheduler*> *taskScheduler,unsigned short clientID);
  virtual ~BasicUsageEnvironment0();

private:
  void reset();

  char fResultMsgBuffer[RESULT_MSG_BUFFER_MAX];
  unsigned fCurBufferSize;
  unsigned fBufferMaxSize;
};



#endif /* INCLUDE_USAGEENVIRONMENT0_H_ */
