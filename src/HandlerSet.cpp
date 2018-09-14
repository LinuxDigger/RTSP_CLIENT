/*
 * HandlerSet.cpp
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#include "HandlerSet.h"
#include <iostream>
#include "Logger.h"
using namespace FrameWork;
using namespace std;

HandlerDescriptor::HandlerDescriptor(HandlerDescriptor* nextHandler) :
		socketNum(0), conditionSet(0), handlerProc(NULL), clientData(NULL) {
	// Link this descriptor into a doubly-linked list:
	if (nextHandler == this) { // initialization
		Logger::GetInstance().Debug(
				"handlerDescriptor ################################1");
		fNextHandler = fPrevHandler = this;
	} else {
		Logger::GetInstance().Debug(
				"handlerDescriptor ################################2");
		fNextHandler = nextHandler;
		fPrevHandler = nextHandler->fPrevHandler;
		nextHandler->fPrevHandler = this;
		fPrevHandler->fNextHandler = this;
	}
}

HandlerDescriptor::~HandlerDescriptor() {
	// Unlink this descriptor from a doubly-linked list:
	fNextHandler->fPrevHandler = fPrevHandler;
	fPrevHandler->fNextHandler = fNextHandler;
}

HandlerSet::HandlerSet() :
		fHandlers(&fHandlers) {
	fHandlers.socketNum = -1; // shouldn't ever get looked at, but in case...
}

HandlerSet::~HandlerSet() {
	// Delete each handler descriptor:
	while (fHandlers.fNextHandler != &fHandlers) {
		delete fHandlers.fNextHandler; // changes fHandlers->fNextHandler
	}
}

void HandlerSet::assignHandler(int socketNum, int conditionSet,
		TaskScheduler::BackgroundHandlerProc* handlerProc, void* clientData) {
	// First, see if there's already a handler for this socket:
	HandlerDescriptor* handler = lookupHandler(socketNum);
	if (handler == NULL) { // No existing handler, so create a new descr:
		handler = new HandlerDescriptor(fHandlers.fNextHandler);
		handler->socketNum = socketNum;
	}

	handler->conditionSet = conditionSet;
	handler->handlerProc = handlerProc;
	handler->clientData = clientData;
}

void HandlerSet::clearHandler(int socketNum) {
	HandlerDescriptor* handler = lookupHandler(socketNum);
	delete handler;
}

void HandlerSet::moveHandler(int oldSocketNum, int newSocketNum) {
	HandlerDescriptor* handler = lookupHandler(oldSocketNum);
	if (handler != NULL) {
		handler->socketNum = newSocketNum;
	}
}

HandlerDescriptor* HandlerSet::lookupHandler(int socketNum) {
	HandlerDescriptor* handler;
	HandlerIterator iter(*this);
	while ((handler = iter.next()) != NULL) { //have something next handler
		if (handler->socketNum == socketNum)
			break;
	}
	return handler;
}

HandlerIterator::HandlerIterator(HandlerSet& handlerSet) :
		fOurSet(handlerSet) {
	reset();
}

HandlerIterator::~HandlerIterator() {
}

void HandlerIterator::reset() {
	fNextPtr = fOurSet.fHandlers.fNextHandler;
}

HandlerDescriptor* HandlerIterator::next() {
	HandlerDescriptor* result = fNextPtr;
	if (result == &fOurSet.fHandlers) { // no more
		result = NULL;
	} else {
		fNextPtr = fNextPtr->fNextHandler;
	}

	return result;
}

