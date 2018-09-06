/*
 * SIPClient.h
 *
 *  Created on: Aug 7, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_SIPCLIENT_H_
#define INCLUDE_SIPCLIENT_H_
#include "NetAddress.h"
#include "DigestAuthentication.h"
#include "GroupSock.h"
#include "Media.h"

// Possible states in the "INVITE" transition diagram (RFC 3261, Figure 5)
enum inviteClientState {
	Calling, Proceeding, Completed, Terminated
};

class SIPClient: public Medium {
public:
	static SIPClient* createNew(UsageEnvironment& env,
			DP_U8 desiredAudioRTPPayloadFormat,
			char const* mimeSubtype = NULL, int verbosityLevel = 0,
			char const* applicationName = NULL);

	void setProxyServer(unsigned proxyServerAddress,
			portNumBits proxyServerPortNum);

	void setClientStartPortNum(portNumBits clientStartPortNum) {
		fClientStartPortNum = clientStartPortNum;
	}

	char* invite(char const* url, Authenticator* authenticator = NULL);
	// Issues a SIP "INVITE" command
	// Returns the session SDP description if this command succeeds
	char* inviteWithPassword(char const* url, char const* username,
			char const* password);
	// Uses "invite()" to do an "INVITE" - first
	// without using "password", then (if we get an Unauthorized
	// response) with an authentication response computed from "password"

	Boolean sendACK(); // on current call
	Boolean sendBYE(); // on current call

	static Boolean parseSIPURL(UsageEnvironment& env, char const* url,
			NetAddress& address, portNumBits& portNum);
	// (ignores any "<username>[:<password>]@" in "url")
	static Boolean parseSIPURLUsernamePassword(char const* url, char*& username,
			char*& password);
	char const* getInviteSdpReply() const {
		return fInviteSDPDescriptionReturned;
	}

	void setUserAgentString(char const* userAgentName);
	// sets an alternative string to be used in SIP "User-Agent:" headers

protected:
	virtual ~SIPClient();

private:
	SIPClient(UsageEnvironment& env, DP_U8 desiredAudioRTPPayloadFormat,
			char const* mimeSubtype, int verbosityLevel,
			char const* applicationName);
	// called only by createNew();

	void reset();

	// Routines used to implement invite*():
	char* invite1(Authenticator* authenticator);
	Boolean processURL(char const* url);
	Boolean sendINVITE();
	static void inviteResponseHandler(void* clientData, int mask);
	void doInviteStateMachine(unsigned responseCode);
	void doInviteStateTerminated(unsigned responseCode);
	TaskToken fTimerA, fTimerB, fTimerD;
	static void timerAHandler(void* clientData, CommonPlay *cpObj);
	static void timerBHandler(void* clientData, CommonPlay *cpObj);
	static void timerDHandler(void* clientData, CommonPlay *cpObj);
	unsigned const fT1; // in microseconds
	unsigned fTimerALen; // in microseconds; initially fT1, then doubles
	unsigned fTimerACount;

	// Routines used to implement all commands:
	char* createAuthenticatorString(Authenticator const* authenticator,
			char const* cmd, char const* url);
	Boolean sendRequest(char const* requestString, unsigned requestLength);
	unsigned getResponseCode();
	unsigned getResponse(char*& responseBuffer, unsigned responseBufferSize);
	Boolean parseResponseCode(char const* line, unsigned& responseCode);

private:
	// Set for all calls:
	DP_U8 fDesiredAudioRTPPayloadFormat;
	char* fMIMESubtype;
	unsigned fMIMESubtypeSize;
	int fVerbosityLevel;
	unsigned fCSeq; // sequence number, used in consecutive requests
	char const* fApplicationName;
	unsigned fApplicationNameSize;
	char const* fOurAddressStr;
	unsigned fOurAddressStrSize;
	portNumBits fOurPortNum;
	Groupsock* fOurSocket;
	char* fUserAgentHeaderStr;
	unsigned fUserAgentHeaderStrLen;

	// Set for each call:
	char const* fURL;
	unsigned fURLSize;
	struct in_addr fServerAddress;
	portNumBits fServerPortNum; // in host order
	portNumBits fClientStartPortNum; // in host order
	unsigned fCallId, fFromTag; // set by us
	char const* fToTagStr; // set by the responder
	unsigned fToTagStrSize;
	Authenticator fValidAuthenticator;
	char const* fUserName; // 'user' name used in "From:" & "Contact:" lines
	unsigned fUserNameSize;

	char* fInviteSDPDescription;
	char* fInviteSDPDescriptionReturned;
	char* fInviteCmd;
	unsigned fInviteCmdSize;
	Authenticator* fWorkingAuthenticator;
	inviteClientState fInviteClientState;
	char fEventLoopStopFlag;
};

#endif /* INCLUDE_SIPCLIENT_H_ */
