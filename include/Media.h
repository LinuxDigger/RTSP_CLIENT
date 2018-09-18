/*
 * Media.h
 *
 *  Created on: Jun 12, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_MEDIA_H_
#define INCLUDE_MEDIA_H_

#include "liveMedia_version.h"
#include "HashTable.h"
#include "UsageEnvironment.h"
typedef void* TaskToken;
// Lots of files end up needing the following, so just #include them here:
#ifndef _NET_COMMON_H
#include "NetCommon.h"
#endif
#include <stdio.h>

// The following makes the Borland compiler happy:
#ifdef __BORLANDC__
#define _strnicmp strnicmp
#define fabsf(x) fabs(x)
#endif

#define mediumNameMaxLen 30

class Medium {
public:
	static Boolean lookupByName(UsageEnvironment& env, char const* mediumName,
			Medium*& resultMedium);
	static void close(UsageEnvironment& env, char const* mediumName);
	static void close(Medium* medium); // alternative close() method using ptrs
	// (has no effect if medium == NULL)

	UsageEnvironment& envir() const {
		return fEnviron;
	}

	char const* name() const {
//		cout << "name eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"<<endl;
		return fMediumName;
	}

	// Test for specific types of media:
	virtual Boolean isSource() const;
	virtual Boolean isSink() const;
	virtual Boolean isRTCPInstance() const;
	virtual Boolean isRTSPClient() const;
	virtual Boolean isRTSPServer() const;
	virtual Boolean isMediaSession() const;
	virtual Boolean isServerMediaSession() const;

protected:
	friend class MediaLookupTable;
	Medium(UsageEnvironment& env); // abstract base class
	virtual ~Medium(); // instances are deleted using close() only

	TaskToken& nextTask() {
		return fNextTask;
	}

private:
	UsageEnvironment& fEnviron;
	char fMediumName[mediumNameMaxLen];
	TaskToken fNextTask;
};

// A data structure for looking up a Medium by its string name.
// (It is used only to implement "Medium", but we make it visible here, in case developers want to use it to iterate over
//  the whole set of "Medium" objects that we've created.)
class MediaLookupTable {
public:
	static MediaLookupTable* ourMedia(UsageEnvironment& env);
	HashTable const& getTable() {
		return *fTable;
	}

protected:
	MediaLookupTable(UsageEnvironment& env);
	virtual ~MediaLookupTable();

private:
	friend class Medium;

	Medium* lookup(char const* name) const;
	// Returns NULL if none already exists

	void addNew(Medium* medium, char* mediumName);
	void remove(char const* name);

	void generateNewName(char* mediumName, unsigned maxLen);

private:
	UsageEnvironment& fEnv;
	HashTable* fTable;
	unsigned fNameGenerator;
};

// The structure pointed to by the "liveMediaPriv" UsageEnvironment field:
class _Tables {
public:
	static _Tables* getOurTables(UsageEnvironment& env,
			Boolean createIfNotPresent = True);
	// returns a pointer to a "_Tables" structure (creating it if necessary)
	void reclaimIfPossible();
	// used to delete ourselves when we're no longer used

	MediaLookupTable* mediaTable;
	void* socketTable;

protected:
	_Tables(UsageEnvironment& env);
	virtual ~_Tables();

private:
	UsageEnvironment& fEnv;
};

#endif /* INCLUDE_MEDIA_H_ */
