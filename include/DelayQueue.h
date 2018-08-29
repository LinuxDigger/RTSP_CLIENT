/*
 * DelayQueue.h
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_DELAYQUEUE_H_
#define INCLUDE_DELAYQUEUE_H_
#include "NetCommon.h"

#ifdef TIME_BASE
typedef TIME_BASE time_base_seconds;
#else
typedef long time_base_seconds;
#endif

///// A "Timeval" can be either an absolute time, or a time interval /////

class Timeval {
public:
	time_base_seconds seconds() const {
		return fTv.tv_sec;
	}
	time_base_seconds seconds() {
		return fTv.tv_sec;
	}
	time_base_seconds useconds() const {
		return fTv.tv_usec;
	}
	time_base_seconds useconds() {
		return fTv.tv_usec;
	}

	int operator>=(Timeval const& arg2) const;
	int operator<=(Timeval const& arg2) const {
		return arg2 >= *this;
	}
	int operator<(Timeval const& arg2) const {
		return !(*this >= arg2);
	}
	int operator>(Timeval const& arg2) const {
		return arg2 < *this;
	}
	int operator==(Timeval const& arg2) const {
		return *this >= arg2 && arg2 >= *this;
	}
	int operator!=(Timeval const& arg2) const {
		return !(*this == arg2);
	}

	void operator+=(class DelayInterval const& arg2);
	void operator-=(class DelayInterval const& arg2);
	// returns ZERO iff arg2 >= arg1

protected:
	Timeval(time_base_seconds seconds, time_base_seconds useconds) {
		fTv.tv_sec = seconds;
		fTv.tv_usec = useconds;
	}

private:
	time_base_seconds& secs() {
		return (time_base_seconds&) fTv.tv_sec;
	}
	time_base_seconds& usecs() {
		return (time_base_seconds&) fTv.tv_usec;
	}

	struct timeval fTv;
};

#ifndef max
inline Timeval max(Timeval const& arg1, Timeval const& arg2) {
	return arg1 >= arg2 ? arg1 : arg2;
}
#endif
#ifndef min
inline Timeval min(Timeval const& arg1, Timeval const& arg2) {
	return arg1 <= arg2 ? arg1 : arg2;
}
#endif

class DelayInterval operator-(Timeval const& arg1, Timeval const& arg2);

///// DelayInterval /////

class DelayInterval: public Timeval {
public:
	DelayInterval(time_base_seconds seconds, time_base_seconds useconds) :
			Timeval(seconds, useconds) {
	}
};

DelayInterval operator*(short arg1, DelayInterval const& arg2);

extern DelayInterval const DELAY_ZERO;
extern DelayInterval const DELAY_SECOND;
extern DelayInterval const DELAY_MINUTE;
extern DelayInterval const DELAY_H;

///// _EventTime /////

class _EventTime: public Timeval {
public:
	_EventTime(unsigned secondsSinceEpoch = 0, unsigned usecondsSinceEpoch = 0)
	// We use the Unix standard epoch: January 1, 1970
	:
			Timeval(secondsSinceEpoch, usecondsSinceEpoch) {
	}
};

_EventTime TimeNow();

extern _EventTime const THE_END_OF_TIME;

///// DelayQueueEntry /////

class DelayQueueEntry {
public:
	virtual ~DelayQueueEntry();

	intptr_t token() {
		return fToken;
	}

protected:
	// abstract base class
	DelayQueueEntry(DelayInterval delay);

	virtual void handleTimeout();

private:
	friend class DelayQueue;
	DelayQueueEntry* fNext;
	DelayQueueEntry* fPrev;
	DelayInterval fDeltaTimeRemaining;

	intptr_t fToken;
	static intptr_t tokenCounter;
};

class DelayQueue: public DelayQueueEntry {
public:
	DelayQueue();
	virtual ~DelayQueue();

	void addEntry(DelayQueueEntry* newEntry); // returns a token for the entry
	void updateEntry(DelayQueueEntry* entry, DelayInterval newDelay);
	void updateEntry(intptr_t tokenToFind, DelayInterval newDelay);
	void removeEntry(DelayQueueEntry* entry); // but doesn't delete it
	DelayQueueEntry* removeEntry(intptr_t tokenToFind); // but doesn't delete it

	DelayInterval const& timeToNextAlarm();
	void handleAlarm();

private:
	DelayQueueEntry* head() {
		return fNext;
	}
	DelayQueueEntry* findEntryByToken(intptr_t token);
	void synchronize(); // bring the 'time remaining' fields up-to-date

	_EventTime fLastSyncTime;
};

#endif /* INCLUDE_DELAYQUEUE_H_ */
