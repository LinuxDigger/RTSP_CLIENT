/*
 * HandlerSet.h
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_HANDLERSET_H_
#define INCLUDE_HANDLERSET_H_

#include "NetCommon.h"
#include "TaskScheduler.h"

class HandlerDescriptor {
public:
	HandlerDescriptor(HandlerDescriptor* nextHandler);
	virtual ~HandlerDescriptor();

public:
	int socketNum;
	int conditionSet;
	TaskScheduler::BackgroundHandlerProc* handlerProc;
	void* clientData;

private:
	// Descriptors are linked together in a doubly-linked list:
	friend class HandlerSet;
	friend class HandlerIterator;
	HandlerDescriptor* fNextHandler;
	HandlerDescriptor* fPrevHandler;
};

class HandlerSet {
public:
	HandlerSet();
	virtual ~HandlerSet();

	void assignHandler(int socketNum, int conditionSet,
			TaskScheduler::BackgroundHandlerProc* handlerProc,
			void* clientData);
	void clearHandler(int socketNum);
	void moveHandler(int oldSocketNum, int newSocketNum);

private:
	HandlerDescriptor* lookupHandler(int socketNum);

private:
	friend class HandlerIterator;
	HandlerDescriptor fHandlers;
};

class HandlerIterator {
public:
	HandlerIterator(HandlerSet& handlerSet);
	virtual ~HandlerIterator();

	HandlerDescriptor* next(); // returns NULL if none
	void reset();

private:
	HandlerSet& fOurSet;
	HandlerDescriptor* fNextPtr;
};

#endif /* INCLUDE_HANDLERSET_H_ */
