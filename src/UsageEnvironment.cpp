/*
 * UsageEnvironment.cpp
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#include "UsageEnvironment.h"

Boolean UsageEnvironment::reclaim() {
	// We delete ourselves only if we have no remainining state:
	if (liveMediaPriv == NULL && groupsockPriv == NULL) {
		delete this;
		return True;
	}

	return False;
}

UsageEnvironment::UsageEnvironment(vector<TaskScheduler*> *scheduler,
		unsigned short clientID) :
		liveMediaPriv(NULL), groupsockPriv(NULL), _u16CliID(clientID), fScheduler(
				*scheduler) {
}

UsageEnvironment::~UsageEnvironment() {
}

UsageEnvironment &UsageEnvironment::operator=(const UsageEnvironment &env) {
	if (this != &env) {
		liveMediaPriv = env.liveMediaPriv;
		groupsockPriv = env.groupsockPriv;
		//old
//		setFScheduler(env.taskScheduler());
		fScheduler = env.fScheduler;
	}
	return *this;
}

// By default, we handle 'should not occur'-type library errors by calling abort().  Subclasses can redefine this, if desired.
// (If your runtime library doesn't define the "abort()" function, then define your own (e.g., that does nothing).)
void UsageEnvironment::internalError() {
	abort();
}

