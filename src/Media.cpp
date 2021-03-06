/*
 * Media.cpp
 *
 *  Created on: Jun 12, 2018
 *      Author: jhb
 */

#include "Media.h"
#include "HashTable.h"
#include "CommonPlay.h"

////////// Medium //////////

Medium::Medium(UsageEnvironment& env, CommonPlay *cpObj) :
		fEnviron(env), fNextTask(NULL), fcpObj(cpObj) {
	// First generate a name for the new medium:
	MediaLookupTable::ourMedia(env)->generateNewName(fMediumName,
	mediumNameMaxLen);
	env.setResultMsg(fMediumName);

	// Then add it to our table:
	MediaLookupTable::ourMedia(env)->addNew(this, fMediumName);
}

Medium::~Medium() {
	// Remove any tasks that might be pending for us:
	fEnviron.taskScheduler(fcpObj->_fClientID / 100)->unscheduleDelayedTask(
			fNextTask);
}

Boolean Medium::lookupByName(UsageEnvironment& env, char const* mediumName,
		Medium*& resultMedium) {
	resultMedium = MediaLookupTable::ourMedia(env)->lookup(mediumName);
	if (resultMedium == NULL) {
		env.setResultMsg("Medium ", mediumName, " does not exist");
		return False;
	}

	return True;
}

void Medium::close(UsageEnvironment& env, char const* name) {
	MediaLookupTable::ourMedia(env)->remove(name);
}

void Medium::close(Medium* medium) {
	if (medium == NULL)
		return;
//  cout << "close??????????????????????????????????"<<endl;
	close(medium->envir(), medium->name());
}

Boolean Medium::isSource() const {
	return False; // default implementation
}

Boolean Medium::isSink() const {
	return False; // default implementation
}

Boolean Medium::isRTCPInstance() const {
	return False; // default implementation
}

Boolean Medium::isRTSPClient() const {
	return False; // default implementation
}

Boolean Medium::isRTSPServer() const {
	return False; // default implementation
}

Boolean Medium::isMediaSession() const {
	return False; // default implementation
}

Boolean Medium::isServerMediaSession() const {
	return False; // default implementation
}

////////// _Tables implementation //////////

_Tables* _Tables::getOurTables(UsageEnvironment& env,
		Boolean createIfNotPresent) {
	if (env.liveMediaPriv == NULL && createIfNotPresent) {
		env.liveMediaPriv = new _Tables(env);
	}
	return (_Tables*) (env.liveMediaPriv);
}

void _Tables::reclaimIfPossible() {
	if (mediaTable == NULL && socketTable == NULL) {
		fEnv.liveMediaPriv = NULL;
		delete this;
	}
}

_Tables::_Tables(UsageEnvironment& env) :
		mediaTable(NULL), socketTable(NULL), fEnv(env) {
}

_Tables::~_Tables() {
}

////////// MediaLookupTable implementation //////////

MediaLookupTable* MediaLookupTable::ourMedia(UsageEnvironment& env) {
	_Tables* ourTables = _Tables::getOurTables(env);
	if (ourTables->mediaTable == NULL) {
		// Create a new table to record the media that are to be created in
		// this environment:
		ourTables->mediaTable = new MediaLookupTable(env);
	}
	return ourTables->mediaTable;
}

Medium* MediaLookupTable::lookup(char const* name) const {
	return (Medium*) (fTable->Lookup(name));
}

void MediaLookupTable::addNew(Medium* medium, char* mediumName) {
	fTable->Add(mediumName, (void*) medium);
}

void MediaLookupTable::remove(char const* name) {
	Medium* medium = lookup(name);
	if (medium != NULL) {
		fTable->Remove(name);
		if (fTable->IsEmpty()) {
			// We can also delete ourselves (to reclaim space):
			_Tables* ourTables = _Tables::getOurTables(fEnv);
			delete this;
			ourTables->mediaTable = NULL;
			ourTables->reclaimIfPossible();
		}

		delete medium;
	}
}

void MediaLookupTable::generateNewName(char* mediumName, unsigned /*maxLen*/) {
	// We should really use snprintf() here, but not all systems have it
	sprintf(mediumName, "liveMedia%d", fNameGenerator++);
}

MediaLookupTable::MediaLookupTable(UsageEnvironment& env) :
		fEnv(env), fTable(HashTable::create(STRING_HASH_KEYS)), fNameGenerator(
				0) {
}

MediaLookupTable::~MediaLookupTable() {
	delete fTable;
}
