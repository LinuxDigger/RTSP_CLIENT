/*
 * BasicUsageEnvironment.cpp
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */

#include "BasicUsageEnvironment.h"

#include <stdio.h>

////////// BasicUsageEnvironment //////////

#if defined(__WIN32__) || defined(_WIN32)
extern "C" int initializeWinsockIfNecessary();
#endif

BasicUsageEnvironment::BasicUsageEnvironment(TaskScheduler& taskScheduler,
		unsigned short clientID) :
		BasicUsageEnvironment0(taskScheduler, clientID) {
#if defined(__WIN32__) || defined(_WIN32)
	if (!initializeWinsockIfNecessary()) {
		setResultErrMsg("Failed to initialize 'winsock': ");
		reportBackgroundError();
		internalError();
	}
#endif
}

BasicUsageEnvironment::~BasicUsageEnvironment() {
}

BasicUsageEnvironment*
BasicUsageEnvironment::createNew(TaskScheduler& taskScheduler,
		unsigned short clientID) {
	return new BasicUsageEnvironment(taskScheduler, clientID);
}

int BasicUsageEnvironment::getErrno() const {
#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
	return WSAGetLastError();
#else
	return errno;
#endif
}

UsageEnvironment& BasicUsageEnvironment::operator<<(char const* str) {
	if (str == NULL)
		str = "(NULL)"; // sanity check
	fprintf(stderr, "%s", str);
	return *this;
}

UsageEnvironment& BasicUsageEnvironment::operator<<(int i) {
	fprintf(stderr, "%d", i);
	return *this;
}

UsageEnvironment& BasicUsageEnvironment::operator<<(unsigned u) {
	fprintf(stderr, "%u", u);
	return *this;
}

UsageEnvironment& BasicUsageEnvironment::operator<<(double d) {
	fprintf(stderr, "%f", d);
	return *this;
}

UsageEnvironment& BasicUsageEnvironment::operator<<(void* p) {
	fprintf(stderr, "%p", p);
	return *this;
}

